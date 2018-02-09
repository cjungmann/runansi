#ifndef RUNANSI_LLINES_HPP_SOURCE
#define RUNANSI_LLINES_HPP_SOURCE

#include <stdarg.h>
#include <assert.h>
#include <string.h>  // for memset

struct llines
{
   llines     *next;
   const char *line;
   int        position;
};

inline void init(llines *ll) { memset(ll, 0, sizeof(llines)); }

// Interface for callback using llines
class ILines_Callback
{
public:
   virtual ~ILines_Callback() { }
   virtual void operator()(const llines *) const = 0;
};

// Implementation for callback using llines
template <typename Func>
class Lines_User : public ILines_Callback
{
protected:
   Func &m_f;
public:
   Lines_User(Func &f) : m_f(f) { }
   virtual ~Lines_User()        { }
   virtual void operator()(const llines *ll) const { m_f(ll); }
};

int count_lines(const llines *);

/**
 * @brief Makes a llines list from list of strings
 *
 * Terminate the parameter list with a nullptr to signal the end of the strings list.
 *
 * ~~~cpp
 * auto f = [](const llines *ll)
 * {
 *    const llines *llsel = select_line(ll,2);
 *    std::cout << "\n\nYou selected \"" << llsel->line << "\"\n";
 * };
 * Lines_User(decltype(f)> lu(f);
 *
 * make_llines(lu, "first", "second", "third", nullptr);
 * ~~~
 */
void make_llines(ILines_Callback &cb, ...);


/** Print Lines (pl) Info **/
struct pl_info
{
   int lines_in_list;
   int sum_top_bottom_margins;
   int highlight;
   int top_to_print;
   int max_to_print;
};

void init(pl_info &pli, const llines *ll, int sum_top_bottom_margins);

typedef std::ostream& (*LL_Streamer)(std::ostream &os, const llines &ll);

/** Returns highlighted line. **/
const llines* print_lines(const llines *ll, const pl_info &pli);


#endif
