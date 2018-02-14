#include <stdarg.h>
#include <iostream>
#include <alloca.h>
#include <string.h>  // for strlen
#include "runansi_llines.hpp"


#define HL_ON "\033[104m"
#define HL_OFF "\033[0m"



int count_lines(const LLines *ll)
{
   while (ll)
   {
      if (ll->next)
         ll = ll->next;
      else
         return ll->position;
   }

   return 0;
}

void init(pl_info &pli, const LLines *ll, int highlight, int sum_top_bottom_margins=0)
{
   pli.lines_in_list = count_lines(ll);
   pli.sum_top_bottom_margins=sum_top_bottom_margins;
   pli.top_to_print = 1;
   pli.highlight = highlight;
   pli.max_to_print = 0;   // should be calculated each time in case the screen size has changed
}

void make_String_LLines(ILines_Callback &cb, ...)
{
   LLines *head=nullptr, *tail=nullptr;

   va_list clines;
   va_start(clines, cb);

   const char *val;
   while((val=va_arg(clines,const char*)))
   {
      LLines *ptr = static_cast<LLines*>(alloca(sizeof(LLines)));
      ptr->next = nullptr;
      ptr->line = val;

      if (tail)
      {
         ptr->position = tail->position + 1;
         tail->next = ptr;
         tail = ptr;
      }
      else
      {
         ptr->position = 1;
         head = tail = ptr;
      }
   }

   va_end(clines);

   cb(head);
}

const LLines* print_lines(const LLines *ll, const pl_info &pli, LL_Streamer pstreamer)
{
   const LLines *ptr = ll;
   const LLines *rval = nullptr;

   int count = 0;
   while (ptr && count<pli.max_to_print)
   {
      if (ptr->position >= pli.top_to_print)
      {
         ++count;

         if (ptr->position==pli.highlight)
         {
            std::cout << HL_ON;
            rval = ptr;
         }

         (*pstreamer)(std::cout, *ptr) << std::endl;
         // std::cout << (*pstreamer)(*ptr, std::cout) << std::endl;

         if (ptr->position==pli.highlight)
            std::cout << HL_OFF;
      }

      ptr = ptr->next;
   }

   return rval;
}

