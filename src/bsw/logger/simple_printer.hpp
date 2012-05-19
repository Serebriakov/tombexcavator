#ifndef __BSW_LOGGER_SIMPLE_PRINTER_HPP__
#define __BSW_LOGGER_SIMPLE_PRINTER_HPP__

#include "bsw/logger/abstract_printer.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    class BSW_API simple_printer_c : public abstract_printer_c
    {
    public:
      void print_function_name (std::ostream& stream, const record_c* record);
      void print_space         (std::ostream& stream, unsigned int count);
      void print_level         (std::ostream& stream, const record_c* record);
      void print_file_name     (std::ostream& stream, const record_c* record);
      void print_line_number   (std::ostream& stream, const record_c* record);
      void print_thread_id     (std::ostream& stream, const record_c* record);
      void print_time_value    (std::ostream& stream, const record_c* record);
      void print_module_name   (std::ostream& stream, const record_c* record);
      void print_args          (std::ostream& stream, const record_c* record);
      void print_free_text     (std::ostream& stream, const std::string& text);


      void print_prologue (std::ostream& stream);
      void print_epilogue (std::ostream& stream);
      void before_log_record   (std::ostream& stream, const record_c* record);
      void after_log_record    (std::ostream& stream, const record_c* record);
      void before_print_args   (std::ostream& stream, const record_c* record);
      void after_print_args    (std::ostream& stream, const record_c* record);
    };
  } // ns logger
} // ns bsw

#endif
