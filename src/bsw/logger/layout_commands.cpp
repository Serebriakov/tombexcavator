#include "bsw/logger/layout_commands.hpp"

namespace bsw
{
  namespace logger
  {

    abstract_layout_command_c::~abstract_layout_command_c ()
    {

    }

    space_command_c::space_command_c (unsigned int count)
      : m_count (count)
    {
    }

    void space_command_c::call (abstract_printer_c* printer,
				std::ostream& stream, 
				const record_c* /*record*/) const
    {
      printer->print_space (stream, m_count);
    }

    // FileNameCommand

    void file_name_command_c::call (abstract_printer_c* printer,
				    std::ostream& stream, 
				    const record_c* record) const
    {
      printer->print_file_name (stream, record);
    }

    // FuncNameCommand

    void func_name_command_c::call (abstract_printer_c* printer,
				    std::ostream& stream, 
				    const record_c* record) const
    {
      printer->print_function_name (stream, record);
    }

    // LineNumberCommand

    void line_number_command_c::call (abstract_printer_c* printer,
				      std::ostream& stream, 
				      const record_c* record) const
    {
      printer->print_line_number (stream, record);
    }

    // TreadIDCommand

    void thread_id_command_c::call (abstract_printer_c* printer,
				    std::ostream& stream, 
				    const record_c* record) const
    {
      printer->print_thread_id (stream, record);
    }

    // TimeValueCommand

    void time_value_command_c::call (abstract_printer_c* printer,
				     std::ostream& stream, 
				     const record_c* record) const
    {
      printer->print_time_value (stream, record);
    }

    // ModuleNameCommand

    void module_name_command_c::call (abstract_printer_c* printer,
				      std::ostream& stream, 
				      const record_c* record) const
    {
      printer->print_module_name (stream, record);
    }

    // TextCommand

    void text_command_c::call (abstract_printer_c* printer,
			       std::ostream& stream, 
			       const record_c* record) const
    {
      printer->before_print_args (stream, record);
      printer->print_args (stream, record);
      printer->after_print_args (stream, record);
    }

    // FreeTextCommand
    free_text_command_c::free_text_command_c (const std::string& txt)
      : m_txt (txt)
    {
    }

    void free_text_command_c::call (abstract_printer_c* printer,
				    std::ostream& stream, 
				    const record_c* /*record*/) const
    {
      printer->print_free_text (stream, m_txt);
    }


  } // ns logger
} // ns bsw

