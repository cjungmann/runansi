#include <iostream>
#include <exception>
#include <alloca.h>
#include <mysqlcb.hpp>
#include "runansi.hpp"

using namespace mysqlcb;

// struct Bind_Lines : public LLines
// {
//    const char *line;
//    Bind_Lines *extra;
// };


void summon_first_column_list(ILines_Callback &cb, PullPack &pp)
{
   LLines *head=nullptr, *tail=nullptr;
   Bind_Data *bdata = pp.binder.bind_data;
   while (pp.puller(false))
   {
      size_t len_name = get_size(bdata);
      char *buffer = static_cast<char*>(alloca(len_name));
      set_with_value(bdata, buffer, len_name);

      LLines *ptr = static_cast<LLines*>(alloca(sizeof(LLines)));
      ptr->next = nullptr;
      ptr->line = buffer;
      
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

   if (head)
      cb(head);
}

void summon_column_lines(ILines_Callback &cb, PullPack &pp)
{
   int columns = pp.binder.field_count;
   size_t *col_sizes = static_cast<size_t*>(alloca(columns * sizeof(size_t)));
   const char **cols = static_cast<const char**>(alloca((columns+1) * sizeof(const char*)));
   cols[columns] = nullptr;

   LLines *head=nullptr, *tail=nullptr;
   Bind_Data *bdata = pp.binder.bind_data;
   while (pp.puller(false))
   {
      size_t line_len = 0;
      Bind_Data *pb = bdata;
      size_t *col_size = col_sizes;
      while (valid(pb))
      {
         *col_size = get_string_length(pb);
         line_len += 1 + *col_size;
         ++col_size;
         ++pb;
      }

      char *buffer = static_cast<char*>(alloca(line_len));

      pb = bdata;
      col_size = col_sizes;
      char *cptr = buffer;
      while (valid(pb))
      {
         if (cptr>buffer)
            *cptr++ = ' ';

         get_string_value(pb, cptr, *col_size);
         cptr += *col_size;
         ++col_size;
         ++pb;
      }
      if (cptr==buffer+line_len-1)
         *cptr = '\0';
      

      LLines *ptr = static_cast<LLines*>(alloca(sizeof(LLines)));
      ptr->next = nullptr;
      ptr->line = buffer;
      
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

   if (head)
      cb(head);
}

void show_columns(MYSQL &mysql, const char *dbase_name, const char *table_name)
{
   auto fpuller = [&dbase_name, &table_name](PullPack &pp)
   {
      auto flines = [&dbase_name, &table_name](const LLines *ll)
      {
         while (true)
         {
            const LLines *selected_line = select_line(ll, 1);
            if (selected_line)
            {
               const char *column = selected_line->line;
               std::cout << column;
            }
            else
               break;
         }
      };
      Lines_User<decltype(flines)> lu(flines);

      summon_column_lines(lu, pp);
   };
   MParam params[3] = {dbase_name, table_name};

   execute_query_pull(mysql,
                      fpuller,
                      "SELECT COLUMN_NAME,DATA_TYPE,COLUMN_TYPE"
                      "  FROM information_schema.COLUMNS"
                      " WHERE TABLE_SCHEMA=?"
                      "   AND TABLE_NAME=?",
                      params);
}

void show_table_names(MYSQL &mysql, const char *dbase_name)
{
   auto fpuller = [&dbase_name](PullPack &pp)
      {
         auto flines = [&pp, &dbase_name](const LLines *ll)
         {
            while (true)
            {
               const LLines *selected_line = select_line(ll, 1);
               if (selected_line)
               {
                  const char *table_name = selected_line->line;
                  show_columns(pp, dbase_name, table_name);
               }
               else
                  break;
            }
         };
         Lines_User<decltype(flines)> lu(flines);

         summon_first_column_list(lu, pp);
      };

   MParam params[2] = { dbase_name };

   execute_query_pull(mysql,
                      fpuller,
                      "SELECT TABLE_NAME "
                      "  FROM information_schema.TABLES"
                      " WHERE TABLE_SCHEMA = ?",
                      params);
}

void show_database_names(MYSQL &mysql)
{
   auto fpuller = [](PullPack &pp)
      {
         auto flines = [&pp](const LLines *ll)
         {
            while (true)
            {
               const LLines *selected_line = select_line(ll, 1);
               if (selected_line)
               {
                  const char *dbase_name = selected_line->line;
                  show_table_names(pp, dbase_name);
               }
               else
                  break;
            }
         };
         Lines_User<decltype(flines)> lu(flines);

         summon_first_column_list(lu, pp);
      };

   execute_query_pull(mysql,
                      fpuller,
                      "SELECT SCHEMA_NAME FROM information_schema.SCHEMATA");
}


void pull_database_tables(PullPack &pp, const char *dbase_name)
{
   auto f = [&pp, &dbase_name](const LLines *ll)
   {
      while (true)
      {
         const LLines *table_line = select_line(ll, 1);
         if (table_line)
         {
            const char *table_name = table_line->line;
            std::cout << "Selected table " << table_name << std::endl;
            get_keyp();
         }
         else
            break;
      }
   };
   Lines_User<decltype(f)> lu(f);

   summon_first_column_list(lu, pp);
}

void pull_database_names(PullPack &pp)
{
   auto f = [&pp](const LLines *ll)
   {
      while (true)
      {
         const LLines *dbase_line = select_line(ll, 1);
         if (dbase_line)
         {
            const char *dbase_name = dbase_line->line;

            auto fpuller = [&dbase_name](PullPack &pp)
               {
                  pull_database_tables(pp, dbase_name);
               };
            PullPack_User<decltype(fpuller)> pu(fpuller);

            auto fbinder = [&pp, &pu](const Binder &binder)
               {
                  execute_query_pull(pp,
                                     pu,
                                     "SELECT TABLE_NAME "
                                     "  FROM information_schema.TABLES"
                                     " WHERE TABLE_SCHEMA = ?",
                                     &binder);
               };
            Binder_User<decltype(fbinder)> bu(fbinder);

            summon_binder(bu,
                          StringParam(dbase_name),
                          VoidParam());
         }
         else
            break;
      }
   };
   Lines_User<decltype(f)> lu(f);

   summon_first_column_list(lu, pp);
}

      

int main(int argc, char **argv)
{
   start_resize_handler();
   auto f = [](MYSQL &mysql)
   {
      show_database_names(mysql);
   };
   
   try
   {
      start_mysql(f);
   }
   catch(std::exception &e)
   {
      std::cout << "Caught an exception: " << e.what() << std::endl;
   }

   return 0;
}
