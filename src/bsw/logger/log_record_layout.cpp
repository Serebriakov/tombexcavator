#include "bsw/logger/log_record_layout.hpp"
#include "bsw/logger/record.hpp"

namespace bsw
{
  namespace logger
  {
    namespace
    {
      class severity_command_c : public abstract_layout_command_c
      {
      public:
	virtual void call (abstract_printer_c* printer, 
			   std::ostream& stream, 
			   const record_c* record) const;
      };
      // -----------------------------------------------------------
      void severity_command_c::call (abstract_printer_c* printer,
				     std::ostream& stream, 
				     const record_c* record) const
      {
	printer->print_level (stream, record);
      }
    } // unnamed ns
    // ====================================================================
    log_record_layout_c* log_record_layout_c::append_level ()
    {
      this->_add_command ( new severity_command_c);
      return this;
    }
  } // ns logger
} // ns bsw
