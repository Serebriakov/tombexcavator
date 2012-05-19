#ifndef __BSW_LOGGER_FORMATTER_HPP__
#define __BSW_LOGGER_FORMATTER_HPP__

#include <iosfwd>

#include "bsw/logger/log_record_layout.hpp"
#include "bsw/mt/mutex.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw 
{
  namespace logger
  {
    class abstract_printer_c;
    class BSW_API record_c;

    class BSW_API formatter_c
    {
    public:
      formatter_c  ();
      ~formatter_c ();

      void init (log_record_layout_ptr_t layout,
		 abstract_printer_c* printer);

      void start (std::ostream& stream);
      void done  (std::ostream& stream);

      void print (const record_c* record, std::ostream& stream);
    private:
#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 4251)
#endif
      log_record_layout_ptr_t m_layout;
#if defined(_MSC_VER)
#pragma warning (pop)
#endif  
      abstract_printer_c*     m_printer;
    }; 

    typedef utils::shared_ptr_c <formatter_c, mt::mutex_c> formatter_ptr_t;
  } // ns logger
} // ns bsw

#endif
