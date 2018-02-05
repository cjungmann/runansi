#include <stdarg.h>
#include <iostream>
#include <alloca.h>
#include "llines.hpp"


#define HL_ON "\033[104m"
#define HL_OFF "\033[0m"

int count_lines(const llines *ll)
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

void init(pl_info &pli, const llines *ll, int sum_top_bottom_margins=0)
{
   pli.lines_in_list = count_lines(ll);
   pli.sum_top_bottom_margins=sum_top_bottom_margins;
   pli.top_to_print = 1;
   pli.highlight = 1;
   pli.max_to_print = 0;   // should be calculated each time in case the screen size has changed
}

void make_llines(ILines_Callback &cb, ...)
{
   llines *head=nullptr, *tail=nullptr;

   va_list clines;
   va_start(clines, cb);

   const char *val;
   while((val=va_arg(clines,const char*)))
   {
      llines *ptr = static_cast<llines*>(alloca(sizeof(llines)));
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

const llines* print_lines(const llines *ll, const pl_info *pli)
{
   const llines *ptr = ll;
   const llines *rval = nullptr;

   int top_to_print;
   int max_to_print;
   int highlight;

   if (pli)
   {
      top_to_print = pli->top_to_print;
      max_to_print = pli->max_to_print;
      highlight = pli->highlight;
   }
   else
   {
      top_to_print = 1;
      max_to_print = 9999;
      highlight = 1;
   }

   int count = 0;
   while (ptr && count<max_to_print)
   {
      if (ptr->position >= top_to_print)
      {
         ++count;

         if (ptr->position==highlight)
         {
            std::cout << HL_ON;
            rval = ptr;
         }

         std::cout << ptr->line << std::endl;

         if (ptr->position==highlight)
            std::cout << HL_OFF;
      }

      ptr = ptr->next;
   }

   return rval;
}

