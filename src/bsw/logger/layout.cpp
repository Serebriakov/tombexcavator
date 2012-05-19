#include "bsw/logger/layout.hpp"

namespace bsw
{
  namespace logger
  {
    // ----------------------------------------------------------------------------
    applyer_c::~applyer_c ()
    {
      for (iterator_t i = m_commands.begin (); i!= m_commands.end (); i++)
	{
	  const abstract_layout_command_c* cmd = *i;
	  delete cmd;
	}
    }
    // ----------------------------------------------------------------------------
    void applyer_c::apply (abstract_printer_c* printer, std::ostream& stream, 
			   const record_c* record) const
    {

      for (iterator_t i = m_commands.begin (); i!= m_commands.end (); i++)
	{
	  const abstract_layout_command_c* cmd = *i;
	  cmd->call (printer, stream, record);
	}
    }
    // ----------------------------------------------------------------------------
    void applyer_c::_add_command (const abstract_layout_command_c* cmd)
    {
      m_commands.push_back (cmd);
    }

  } // ns logger
} // ns bsw

