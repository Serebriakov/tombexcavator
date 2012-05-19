#include "bsw/logger/console_printer.hpp"
#include "bsw/logger/record.hpp"

namespace bsw
{
  namespace logger
  {
    void console_printer_c::print_prologue (std::ostream& /*stream*/)
    {
    }
    // ----------------------------------------------------------------------------------------
    void console_printer_c::print_epilogue (std::ostream& /*stream*/)
    {
    }
    // ----------------------------------------------------------------------------------------
    void console_printer_c::before_log_record (std::ostream& /*stream*/, const record_c* /*record*/)
    {

    }
    // ----------------------------------------------------------------------------------------
    void console_printer_c::after_log_record (std::ostream& /*stream*/, const record_c* /*record*/)
    {

    }
    // ----------------------------------------------------------------------------------------
    void console_printer_c::before_print_args (std::ostream& /*stream*/, const record_c* /*record*/)
    {

    }
    // ----------------------------------------------------------------------------------------
    void console_printer_c::after_print_args (std::ostream& /*stream*/, const record_c* /*record*/)
    {
      
    }
  } // ns logger
} // ns bsw
