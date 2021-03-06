#include <iostream>
#include "runansi.hpp"


int main(int argc, char **argv)
{
   auto f = [](const LLines *ll)
   {
      const LLines *llsel = select_line(ll,2, LLines_String_streamer);
      if (llsel)
      {
         std::cout << "\n\nYou selected \"";
         LLines_String_streamer(std::cout,*llsel) << "\"\n";
      }
   };
   Lines_User<decltype(f)> lu(f);

   start_resize_handler();

   make_String_LLines(lu,
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
               "This is the eleventh  line",
               "This is the twelfth line",
               "This is the thirteenth line",
               "This is the fourteenth line",
               "This is the fifteenth line",
               "This is the sixteenth line",
               "This is the seventeenth line",
               "This is the eighteenth line",
               "This is the nineteenth line",
               "This is the twentieth line",
               "This is the twenty-first line",
               "This is the twenty-second line",
               "This is the twenty-third line",
               "This is the twenty-fourth line",
               "This is the twenty-fifth line",
               "This is the twenty-sixth line",
               "This is the twenty-seventh line",
               "This is the twenty-eighth line",
               "This is the twenty-ninth line",
               "This is the thirtieth line",
               "This is the thirty-first line",
               "This is the thirty-second line",
               "This is the thirty-third line",
               "This is the thirty-fourth line",
               "This is the thirty-fifth line",
               "This is the thirty-sixth line",
               "This is the thirty-seventh line",
               "This is the thirty-eighth line",
               "This is the thirty-ninth line",
               "This is the fortieth line",
               "This is the forty-first line",
               "This is the forty-second line",
               "This is the forty-third line",
               "This is the forty-fourth line",
               "This is the forty-fifth line",
               "This is the forty-sixth line",
               "This is the forty-seventh line",
               "This is the forty-eighth line",
               "This is the forty-ninth line",
               "This is the fiftieth line",
               nullptr);

   return 0;
}
