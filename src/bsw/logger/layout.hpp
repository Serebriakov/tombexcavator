#ifndef __BSW_LOGGER_LAYOUT_HPP__
#define __BSW_LOGGER_LAYOUT_HPP__

#include <string>
#include <iosfwd>

#include <list>
#include "bsw/logger/layout_commands.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw 
{
  namespace logger
  {

    class BSW_API applyer_c
    {
    public:
      virtual ~applyer_c ();
      void apply (abstract_printer_c* printer, 
		  std::ostream& stream, 
		  const record_c* record) const;
    protected:
      void _add_command (const abstract_layout_command_c* cmd);
    private:
      typedef std::list <const abstract_layout_command_c*> commands_list_t;
      typedef commands_list_t::const_iterator iterator_t;
    private:
#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 4251)
#endif
      commands_list_t m_commands;
#if defined(_MSC_VER)
#pragma warning (pop)
#endif  
    };

    template <class Derived>
    class basic_layout_c : public applyer_c
    {
    public:
      Derived* append_space        (unsigned int count=1);
      Derived* append_file_name    (void);
      Derived* append_func_name    (void);
      Derived* append_line_number  (void);
      Derived* append_thread_id    (void);
      Derived* append_time_value   (void);
      Derived* append_module_name  (void);
      Derived* append_text         (void);
      Derived* append_text         (const std::string& text);
    };

  } // ns logger
} // ns bsw

// ====================================================================
// Implementation
// ====================================================================
namespace bsw
{
  namespace logger
  {

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_space       (unsigned int count)
    {
      this->_add_command (new space_command_c (count));
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_file_name    (void)
    {
      this->_add_command (new file_name_command_c);
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_func_name    (void)
    {
      this->_add_command (new func_name_command_c);
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_line_number  (void)
    {
      this->_add_command (new line_number_command_c);
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_thread_id    (void)
    {
      this->_add_command (new thread_id_command_c);
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_time_value   (void)
    {
      this->_add_command (new time_value_command_c);
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_module_name  (void)
    {
      this->_add_command (new module_name_command_c);
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_text (void)
    {
      this->_add_command (new text_command_c);
      return dynamic_cast<Derived*>(this);
    }

    template <class Derived>
    Derived* basic_layout_c<Derived>::append_text (const std::string& text)
    {
      this->_add_command (new free_text_command_c (text));
      return dynamic_cast<Derived*>(this);
    }
  } // ns logger
} // ns bsw
#endif


