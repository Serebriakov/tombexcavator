#ifndef __BSW_LOGGER_ABSTRACT_PRINTER_HPP__
#define __BSW_LOGGER_ABSTRACT_PRINTER_HPP__

#include <string>
#include <iosfwd>

#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    class BSW_API record_c;

    class BSW_API abstract_printer_c
    {
    public:
      abstract_printer_c ();
      virtual ~abstract_printer_c ();

      virtual void print_prologue (std::ostream& stream) = 0;
      virtual void print_epilogue (std::ostream& stream) = 0;
      
      virtual void before_log_record   (std::ostream& stream, const record_c* record) = 0;
      virtual void after_log_record    (std::ostream& stream, const record_c* record) = 0;
      virtual void print_function_name (std::ostream& stream, const record_c* record) = 0;
      virtual void print_space         (std::ostream& stream, unsigned int count) = 0;
      virtual void print_level         (std::ostream& stream, const record_c* record) = 0;
      virtual void print_file_name     (std::ostream& stream, const record_c* record) = 0;
      virtual void print_line_number   (std::ostream& stream, const record_c* record) = 0;
      virtual void print_thread_id     (std::ostream& stream, const record_c* record) = 0;
      virtual void print_time_value    (std::ostream& stream, const record_c* record) = 0;
      virtual void print_module_name   (std::ostream& stream, const record_c* record) = 0;
      virtual void before_print_args   (std::ostream& stream, const record_c* record) = 0;
      virtual void print_args          (std::ostream& stream, const record_c* record) = 0;
      virtual void after_print_args    (std::ostream& stream, const record_c* record) = 0;
      virtual void print_free_text     (std::ostream& stream, const std::string& text) = 0;
    };
  } // ns logger
} // ns bsw


#endif
