#ifndef __BSW_LOGGER_SYSTEM_HPP__
#define __BSW_LOGGER_SYSTEM_HPP__

#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {

    class device_c;
    class record_c;

    class BSW_API system_c
    {
    public:
      static system_c* instance ();
      
      bool add_device (device_c* device);

      void start (bool use_threads);
      static void join  ();

      void post (record_c* record);
    private:
      system_c ();
      ~system_c ();
    private:
      void _start ();
      void _start_mt ();

      void _join ();
      void _join_mt ();

      void _post (record_c* record);
      void _post_mt (record_c* record);
    private:
      system_c (const system_c&);
      system_c& operator = (const system_c&);
    private:
      static system_c* s_instance;
    private:
      bool m_use_threads;
      device_c* m_devices_head;
      device_c* m_devices_tail;
      device_c* m_default_device;
    };
  } // ns logger
} // ns bsw


#endif
