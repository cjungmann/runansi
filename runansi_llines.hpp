#ifndef RUNANSI_LLINES_HPP_SOURCE
#define RUNANSI_LLINES_HPP_SOURCE

#include <stdarg.h>
#include <assert.h>
#include <string.h>  // for memset

struct LLines
{
   LLines     *next;
   const char *line;
   int        position;
};

inline void init(LLines *ll) { memset(ll, 0, sizeof(LLines)); }

// Interface for callback using LLines
class ILines_Callback
{
public:
   virtual ~ILines_Callback() { }
   virtual void operator()(const LLines *) const = 0;
};

// Implementation for callback using LLines
template <typename Func>
class Lines_User : public ILines_Callback
{
protected:
   Func &m_f;
public:
   Lines_User(Func &f) : m_f(f) { }
   virtual ~Lines_User()        { }
   virtual void operator()(const LLines *ll) const { m_f(ll); }
};

int count_lines(const LLines *);

/**
 * @brief Makes a LLines list from list of strings
 *
 * Terminate the parameter list with a nullptr to signal the end of the strings list.
 *
 * ~~~cpp
 * auto f = [](const LLines *ll)
 * {
 *    const LLines *llsel = select_line(ll,2);
 *    std::cout << "\n\nYou selected \"" << llsel->line << "\"\n";
 * };
 * Lines_User(decltype(f)> lu(f);
 *
 * make_LLines(lu, "first", "second", "third", nullptr);
 * ~~~
 */
void make_LLines(ILines_Callback &cb, ...);


/** Print Lines (pl) Info **/
struct pl_info
{
   int lines_in_list;
   int sum_top_bottom_margins;
   int highlight;
   int top_to_print;
   int max_to_print;
};

void init(pl_info &pli, const LLines *ll, int sum_top_bottom_margins);

typedef std::ostream& (*LL_Streamer)(std::ostream &os, const LLines &ll);

std::ostream &default_streamer(std::ostream &os, const LLines &ll);

/** Returns highlighted line. **/
const LLines* print_lines(const LLines *ll,
                          const pl_info &pli,
                          LL_Streamer pstreamer=default_streamer);


#endif
