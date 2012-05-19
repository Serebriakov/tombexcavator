#ifndef __BSW_LOGGER_CONSOLE_PRINTER_HPP__
#define __BSW_LOGGER_CONSOLE_PRINTER_HPP__

#include "bsw/logger/simple_printer.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    class BSW_API console_printer_c : public simple_printer_c
    {
    public:
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
