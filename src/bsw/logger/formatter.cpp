#include "bsw/logger/formatter.hpp"

#include "bsw/logger/abstract_printer.hpp"

namespace bsw
{
  namespace logger
  {
    formatter_c::formatter_c  ()
      : m_printer (0)
    {
    }
    // ---------------------------------------------------------
    formatter_c::~formatter_c (void)
    {
      if (m_printer)
	{
	  delete m_printer;
	}
    }
    // ---------------------------------------------------------
    void formatter_c::init (log_record_layout_ptr_t layout,
			    abstract_printer_c* printer)
    {
      m_layout    = layout;
      m_printer   = printer;
    }
    // ---------------------------------------------------------
    void formatter_c::start (std::ostream& stream)
    {
      m_printer->print_prologue (stream);
    }
    // ---------------------------------------------------------
    void formatter_c::done  (std::ostream& stream)
    {
      m_printer->print_epilogue (stream);
    }
    // ---------------------------------------------------------
    void formatter_c::print (const record_c* record, std::ostream& stream)
    {
      m_printer->before_log_record (stream, record);
      m_layout->apply (m_printer, stream, record);
      m_printer->after_log_record (stream, record);
    }
  } // ns logger
} // ns bsw

