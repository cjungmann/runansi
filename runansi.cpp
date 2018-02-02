#include <stdio.h>    // for perror()
#include <termios.h>  // for tcgetattr()
#include <unistd.h>   // for tcgetattr()
#include <iostream>
#include "runansi.hpp"
#include "llines.hpp"

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

   int readlen;
   if ((readlen=read(0, buff, bufflen-1)) < 0)
      perror ("read()");
   else
      buff[readlen] = '\0';

   if (tcsetattr(0, TCSADRAIN, &term_old) < 0)
      perror ("tcsetattr ~ICANON");

   return (buff);
}

uint32_t get_screen_size(void)
{
   push_cursor_position();
   std::cout << CSI << "999;999H";

   std::cin.clear();

   std::cout << CSI << "6n" << std::flush;

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
               if (newhl>0 && newhl<=pli.line_count)
                  pli.highlight = newhl;
            }
            break;
      }
   }

   return 0;
}

      
const llines *select_line(const llines *ll, int highlight)
{
   scrsize size = get_screen_size();
   int rows = row_count(size);
   // int cols = col_count(size);

   int limit = rows - 8 ;

   pl_info pli;
   init(pli, ll, limit);

   const llines *selected = nullptr;
   const char *ptr = nullptr;

   char buff[10];

   do
   {
      clear_screen();
      std::cout << std::endl << std::endl;
      std::cout << "Use arrow keys to move highlight, ENTER to select highlighted line.\n\n";

      selected = print_lines(ll, &pli);

      // Using local buffer to preserve keypress after later get_keyp():
      ptr = get_keyp(buff, 10);

      if (adjust_pl_info(pli, ptr))
         return nullptr;
   }
   while (*ptr!=10);

   return selected;
}

