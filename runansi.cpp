#include <stdio.h>    // for perror()
#include <termios.h>  // for tcgetattr()
#include <unistd.h>   // for tcgetattr()
#include <iostream>
#include "runansi.hpp"

// Global screen size variable to avoid unnecessary screen size checks
// for each screen replot.

scrsize  g_scrsize = 0;



/**
 * Returns the number of character rows and columns in the current console window.
 *
 * Returns an integer (disguised as a type for type-mangling) that packs the rows
 * and columns count.
 *
 * Use row_count() and col_count() functions to unpack the values.
 */
scrsize poll_screen_size(void)
{
   push_cursor_position();
   std::cout << "\033[999;999H";

   std::cin.clear();

   std::cout << "\033[6n" << std::flush;

   char buff[80];
   get_keyp(buff,80);

   pop_cursor_position();

   uint32_t row=0, col=0;
   uint32_t *cur = &row;
   char *ptr = buff+1;
   while (*ptr)
   {
      switch(*ptr)
      {
         case ';':
            cur = &col;
            break;
         case 'R':
            return (row * SCRINT_MULTIPLIER) + col;
         default:
            if (*ptr>='0' && *ptr<='9')
            {
               *cur *= 10;
               *cur += *ptr - '0';
            }
         case '[':
            break;
            
      }
      ++ptr;
   }
   return 0;
}

void resize_handler(int signo)
{
   if (signo==SIGWINCH)
   {
      std::cerr << "Received a screen resize command!\n";
      g_scrsize = poll_screen_size();
   }
}

void start_resize_handler(void)
{
   if (signal(SIGWINCH, resize_handler)==SIG_ERR)
      std::cerr << "Window resize detection not available.\n";
   else
      g_scrsize = poll_screen_size();
}

scrsize get_screen_size(void)
{
   if (g_scrsize)
      return g_scrsize;
   else
      return poll_screen_size();
}



/**
 * Returns a keypress without waiting for ENTER.
 *
 * Stores the keypress in a buffer because many keypresses
 * return several characters, starting with "ESC[".
 *
 * Pass your own buffer if you need to preserve the value,
 * or just call the function without arguments to use
 * the built-in buffer.
 */
const char* get_keyp(char *buff, int bufflen)
{
   static char sbuff[40];
   static const int sbufflen = 10;

   // Use static buffer if no buffer provided:
   if (buff==nullptr)
   {
      buff = sbuff;
      bufflen = sbufflen;
   }

   termios term_old, term_new;
   if (tcgetattr(0, &term_old) < 0)
      perror("tcsetattr()");

   term_new = term_old;

   term_new.c_lflag &= ~(ICANON|ECHO);
   term_new.c_cc[VMIN] = 1;
   term_new.c_cc[VTIME] = 0;
   if (tcsetattr(0, TCSANOW, &term_new) < 0)
      perror("tcsetattr ICANON");

   // char c;
   // while((c=read(0,&c,1))!=EOF)
   //    ;

   int readlen;
   if ((readlen=read(0, buff, bufflen-1)) < 0)
      perror ("read()");
   else
      buff[readlen] = '\0';

   if (tcsetattr(0, TCSADRAIN, &term_old) < 0)
      perror ("tcsetattr ~ICANON");

   return (buff);
}

void adjust_pli_to_screen(pl_info &pli)
{
   scrsize srs = get_screen_size();
   int screen_height_in_rows = row_count(srs);

   pli.max_to_print = screen_height_in_rows - pli.sum_top_bottom_margins;

   if (pli.highlight < pli.top_to_print)
      pli.top_to_print = pli.highlight;
   else if (pli.highlight - pli.top_to_print >= pli.max_to_print)
      pli.top_to_print = 1 + pli.highlight - pli.max_to_print;
}

/**
 * Adjusts several values in the struct upon a keypress.
 *
 * This function will check and may adjust several variables in the
 * struct to move the highlight and scroll if necessary to keep the
 * highlighted row in view.
 */
bool adjust_pl_info(pl_info &pli, const char *keyp)
{
   int dir = 0;
   if (*keyp==27)
   {
      switch(*++keyp)
      {
         case 0:
            return 1;
         case '[':
         case 'O':
            dir = 0;
            switch(*++keyp)
            {
               case 'A':
                  --dir;
                  break;
               case 'B':
                  ++dir;
                  break;
            }
            if (dir)
            {
               int newhl = pli.highlight + dir;
               if (newhl>0 && newhl<=pli.lines_in_list)
               {
                  pli.highlight = newhl;
                  adjust_pli_to_screen(pli);
               }
            }
            break;
      }
   }

   return 0;
}

/**
 * This is the entry into the utility.  Run a selectable list of text lines.
 *
 * Pass a linked list of lines, along with the initially highlighted line number.
 *
 * The function returns a pointer to the selected line when ENTER is pressed,
 * or nullptr if ESCAPE is pressed.
 */
const LLines *select_line(const LLines *ll, int highlight)
{
   pl_info pli;
   init(pli, ll, 8);
   adjust_pli_to_screen(pli);

   const LLines *selected = nullptr;
   const char *ptr = nullptr;

   char buff[10];

   do
   {
      clear_screen();

      std::cout << std::endl << std::endl;
      std::cout << "Use arrow keys to move highlight, ENTER to select highlighted line.\n\n";

      selected = print_lines(ll, pli);

      // Using local buffer to preserve keypress after later get_keyp():
      ptr = get_keyp(buff, 10);

      if (adjust_pl_info(pli, ptr))
         return nullptr;
   }
   while (*ptr!=10);

   return selected;
}

