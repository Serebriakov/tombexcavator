#include <iostream>

#include "bsw/logger/console.hpp"
#include "bsw/logger/device.hpp"
#include "bsw/logger/standard_layout.hpp"
#include "bsw/logger/console_printer.hpp"

namespace bsw
{
  namespace logger
  {
    class console_c : public device_c
    {
    public:
      console_c (formatter_ptr_t formatter, predicate_ptr_t predicate);
    private:
      bool     _do_open  ();
      bool     _do_print_string (const std::string& data);
      void     _do_close ();
    };
    
    console_c::console_c (formatter_ptr_t formatter, predicate_ptr_t predicate)
      : device_c (formatter, predicate)
    {
    }
    // -----------------------------------------------------------------
    bool console_c::_do_open  ()
    {
      return true;
    }
    // -----------------------------------------------------------------
    bool console_c::_do_print_string (const std::string& data)
    {
        if (!data.empty ())
        {
            std::cout << data << std::endl;
        }
      std::cout.flush ();  
      return true;
    }
    // -----------------------------------------------------------------
    void console_c::_do_close ()
    {
    }
    // -----------------------------------------------------------------
    device_c* console_device ()
    {
      return console_device (0);
    }
    // ------------------------------------------------------------------
    device_c* console_device (predicate_c* predicate)
    {
      log_record_layout_c* layout = standard_layout ();
      log_record_layout_ptr_t layout_ptr (layout);
      
      abstract_printer_c* printer = new console_printer_c;
      
      formatter_c* formatter = new formatter_c;
      formatter_ptr_t formatter_ptr (formatter);

      formatter->init (layout_ptr, printer);

      predicate_ptr_t predicate_ptr (predicate);

      console_c* console = new console_c (formatter_ptr, predicate_ptr);
      return console;
      
    }
  } // ns logger
} // ns bsw
