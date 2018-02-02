#include <iostream>
#include "runansi.hpp"
#include "llines.hpp"



int main(int argc, char **argv)
{
   auto f = [](const llines *ll)
   {
      const llines *llsel = select_line(ll,2);
      if (llsel)
         std::cout << "\n\nYou selected \"" << llsel->line << "\"\n";
   };
   Lines_User<decltype(f)> lu(f);

   make_llines(lu,
               "This is the first line",
               "This is the second line",
               "This is the third line",
               "This is the fourth line",
               "This is the fifth line",
               "This is the sixth line",
               "This is the seventh line",
               "This is the eighth line",
               "This is the ninth line",
               "This is the tenth line",
               nullptr);

   return 0;
}
