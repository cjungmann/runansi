#ifndef RUNANSI_HPP_SOURCE
#define RUNANSI_HPP_SOURCE

#include <signal.h>
#include <stdint.h>  // for uint32_t
#include "runansi_llines.hpp"


#define SCRINT_MULTIPLIER (1 << 12)
#define CSI "\033["

inline void push_cursor_position(void) { std::cout << CSI << "s"; }
inline void pop_cursor_position(void)  { std::cout << CSI << "u"; }
inline void clear_screen(void)         { std::cout << CSI << "2J" << CSI << "H"; }
inline void conceal_text(void)         { std::cout << CSI << "8m"; }
inline void reveal_text(void)          { std::cout << CSI << "0m"; }

typedef uint32_t scrsize;
scrsize get_screen_size(void);
inline uint32_t row_count(scrsize val) { return val / (SCRINT_MULTIPLIER); }
inline uint32_t col_count(scrsize val) { return val % (SCRINT_MULTIPLIER); }

void start_resize_handler(void);


/**
 * Gets the screen size to determine the number of lines to print and
 * which line should be the top line.
 */
void adjust_pli_to_screen(pl_info &pli);

/**
 * Returns true if solitary ESC, false otherwise.
 *
 * Use return value to signal a break from the event loop.
 */
bool adjust_pl_info(pl_info &pli, const char *keyp);

const char *get_keyp(char *buff=nullptr, int buffen=0);
const LLines *select_line(const LLines *ll, int highlight, LL_Streamer streamer=LLines_String_streamer);




#endif

