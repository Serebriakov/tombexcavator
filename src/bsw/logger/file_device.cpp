#include <iostream>
#include <fstream>

#include "bsw/logger/file_device.hpp"
#include "bsw/logger/device.hpp"
#include "bsw/logger/standard_layout.hpp"
#include "bsw/logger/simple_printer.hpp"

namespace bsw
{
  namespace logger
  {
    class file_c : public device_c
    {
    public:
      file_c (formatter_ptr_t formatter, predicate_ptr_t predicate, const char* file);
    private:
      bool     _do_open  ();
      bool     _do_print_string (const std::string& data);
      void     _do_close ();
    private:
      std::string m_path;
      std::ofstream* m_ofs;
    };
    
    file_c::file_c (formatter_ptr_t formatter, predicate_ptr_t predicate, const char* path)
      : device_c (formatter, predicate),
	m_path   (path)
    {
    }
    // -----------------------------------------------------------------
    bool file_c::_do_open  ()
    {
      m_ofs = new std::ofstream;
      
      m_ofs->open (m_path.c_str ());
      
      if (*m_ofs)
	{
	  return true;
	}
      return false;
    }
    // -----------------------------------------------------------------
    bool file_c::_do_print_string (const std::string& data)
    {
        if (!data.empty ())
        {
	  (*m_ofs) << data << std::endl;
        }
      m_ofs->flush ();  
      return true;
    }
    // -----------------------------------------------------------------
    void file_c::_do_close ()
    {
      if (m_ofs)
	{
	  m_ofs->flush ();
	  delete m_ofs;
	  m_ofs = 0;
	}
    }
    // -----------------------------------------------------------------
    device_c* file_device (const char* path, predicate_c* predicate)
    {
      log_record_layout_c* layout = standard_layout ();
      log_record_layout_ptr_t layout_ptr (layout);
      
      abstract_printer_c* printer = new simple_printer_c;
      
      formatter_c* formatter = new formatter_c;
      formatter_ptr_t formatter_ptr (formatter);

      formatter->init (layout_ptr, printer);

      predicate_ptr_t predicate_ptr (predicate);

      file_c* console = new file_c (formatter_ptr, predicate_ptr, path);
      return console;
      
    }
    // ----------------------------------------------------------------
    device_c* file_device (const char* path)
    {
        return file_device (path, 0);
    }
  } // ns logger
} // ns bsw
