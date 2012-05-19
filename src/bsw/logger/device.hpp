#ifndef __BSW_LOGGER_DEVICE_HPP__
#define __BSW_LOGGER_DEVICE_HPP__

#include "bsw/logger/predicate.hpp"
#include "bsw/logger/formatter.hpp"
#include "bsw/logger/record.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw 
{
  namespace logger
  {
    struct device_impl_s;

    class BSW_API device_c
    {
    public:
      device_c (formatter_ptr_t formatter, predicate_ptr_t predicate);

      virtual ~device_c ();

      bool open ();
      bool print (shared_log_record_t record);
      void close ();

      device_c* next () const;
      void next (device_c* device);

      bool is_opened () const;
    protected:
      formatter_ptr_t  _formatter () const;
      predicate_ptr_t  _predicate () const;
      bool             _filter  (const record_c* record);
    protected:
      virtual bool     _do_open  () = 0;
      virtual bool     _do_print (shared_log_record_t record);
      virtual bool     _do_print_string (const std::string& data) = 0;
      virtual void     _do_close () = 0;
    private:
      device_impl_s* m_pimpl;
    };
  } // ns logger
} // ns bsw

#endif 
