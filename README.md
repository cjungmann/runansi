# Project runansi

This project is designed to work on an ansi terminal to present pages
of lines that are selectable by moving a cursor up and down on a list.

It includes a couple of demonstration programs, _ratest_ and, more
significantly, _sqltest_ that use this library and can serve as inspiration
for other implementations.

In keeping with my general current partiality to designing programs to run
on the stack (for long-lived programs), this project is designed to use
stack memory for everything.  This is why I'm using a linked string list rather
that an array/vector that might be more efficient, and it works for my main
usage case of extracting MySQL records, the count of which are usually
unknown until the query has been fully fetched.

## Description

The project currently supports and demonstrates (through the demo programs)
the creation and display of a linked-list of strings.  There are tools for
creating the linked-list from a variable-length argument list using
`make_String_LLines(ILines_Callback &cb, ...)`.  Look to _sqltest.cpp_
for ideas on how to customize the building of a LLine linked list.


## Plans for This Project

I am setting this project aside for a while.  There are several ideas that
I haven't fully explored, but should be feasible using the current project
state as a starting point.

- Subclass a LLines object that has an additional pointer for extra strings
  in a line.  This would make it possible to present that multi-value lines
  in justified columnar form.

- The message for the printed lines should handle custom messages at the
  top and bottom of the line list.  The current default was included mainly to
  work on scrolling lists too long to fit on the screen.

- Prepare an option to read the lines from a file.
  - File-based lines would be pre-packaged interactions.
  - Along with the file, perhaps a line that begins with a value in
    magic braces could be saved as an additional field in a new LLines
    object...for example
    ~~~txt
    <[1]> Jump in the lake
    <[2]> Get lost
    <[3]> Take a vacation
    ~~~

    ~~~cpp
    // Add a value member to LLines to save the value in <[]>:
    struct LLines_Value : public LLines
    {
       char *value;
    };
    ~~~
