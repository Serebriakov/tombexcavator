#ifndef __BSW_LOGGER_LAYOUT_COMMANDS_HPP__
#define __BSW_LOGGER_LAYOUT_COMMANDS_HPP__

#include <string>

#include "bsw/logger/abstract_printer.hpp"

namespace bsw 
{
  namespace logger
  {
    class abstract_layout_command_c
    {
    public:
      virtual ~abstract_layout_command_c ();
      virtual void call (abstract_printer_c* printer, 
			 std::ostream& stream, 
			 const record_c* record) const = 0;
    };

    class space_command_c : public abstract_layout_command_c
    {
    public:
      space_command_c (unsigned int count);
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    private:
      unsigned int m_count;
    };

    class file_name_command_c : public abstract_layout_command_c
    {
    public:
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    };

    class func_name_command_c : public abstract_layout_command_c
    {
    public:
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    };

    class line_number_command_c : public abstract_layout_command_c
    {
    public:
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    };

    class thread_id_command_c : public abstract_layout_command_c
    {
    public:
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    };

    class time_value_command_c : public abstract_layout_command_c
    {
    public:
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    };

    class module_name_command_c : public abstract_layout_command_c
    {
    public:
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    };

    class text_command_c : public abstract_layout_command_c
    {
    public:
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    };

    class free_text_command_c : public abstract_layout_command_c
    {
    public:
      free_text_command_c (const std::string& txt);
      virtual void call (abstract_printer_c* printer, std::ostream& stream, 
			 const record_c* record) const;
    private:
      std::string m_txt;
    };

  } // ns logger
} // ns bsw

#endif

