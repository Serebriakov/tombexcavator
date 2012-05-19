#include <time.h>
#include <stdio.h>
#include <ostream>

#include "bsw/logger/simple_printer.hpp"
#include "bsw/logger/record.hpp"

#if defined(_MSC_VER)
    #define snprintf _snprintf
#endif

namespace bsw
{
  namespace logger
  {
    void simple_printer_c::print_function_name (std::ostream& stream, const record_c* record)
    {
      stream << record->locus ().function ();
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_space (std::ostream& stream, unsigned int count)
    {
      for (unsigned int i=0; i<count; i++)
	{
	  stream << ' ';
	}
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_level (std::ostream& stream, const record_c* record)
    {
      stream << record->level ();
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_file_name (std::ostream& stream, const record_c* record)
    {
        const std::string& src = record->locus ().source ();
        std::size_t p = src.find ("src");
        if (p == std::string::npos)
        {
            stream << src;
        }
        else
        {
            stream << src.c_str () + p;
        }
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_line_number (std::ostream& stream, const record_c* record)
    {
      stream << record->locus ().line ();
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_thread_id (std::ostream& stream, const record_c* record)
    {
      stream << record->thread_id ();
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_time_value (std::ostream& stream, const record_c* record)
    {
      const time_s& tv = record->time ();
      struct tm tms;
      time_t secs = tv.seconds;
      if (localtime_r (&secs, &tms))
	{
	  char tmbuf[64], buf[64];
	  strftime (tmbuf, sizeof tmbuf, "%m-%d %H:%M:%S", &tms);
	  snprintf (buf, sizeof buf, "%s.%06ld", tmbuf, tv.useconds);
	  stream << buf;
	}
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_module_name (std::ostream& stream, const record_c* record)
    {
      stream << record->locus ().module ();
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_args (std::ostream& stream, const record_c* record)
    {
      stream << record->message ();
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_free_text (std::ostream& stream, const std::string& text)
    {
      stream << text;
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_prologue (std::ostream& /*stream*/)
    {
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::print_epilogue (std::ostream& /*stream*/)
    {
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::before_log_record   (std::ostream& /*stream*/, const record_c* /*record*/)
    {
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::after_log_record    (std::ostream& /*stream*/, const record_c* /*record*/)
    {
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::before_print_args   (std::ostream& /*stream*/, const record_c* /*record*/)
    {
    }
    // -----------------------------------------------------------------------------------------
    void simple_printer_c::after_print_args    (std::ostream& /*stream*/, const record_c* /*record*/)
    {
    }
  } // ns logger
} // ns bsw
