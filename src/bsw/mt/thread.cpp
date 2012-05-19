#include <stdexcept>
#include <memory>
#include "bsw/mt/thread.hpp"
#include "bsw/mt/os_config.hpp"

#if !defined(BSW_USE_PTHREAD)
#if defined (_WIN32_WINNT)
#undef _WIN32_WINNT
#endif 
#define  _WIN32_WINNT 0x0403
#include <windows.h>
#include <process.h>    /* _beginthread */
#else
#include <pthread.h>
#endif

#include "bsw/mt/mutex.hpp"

namespace bsw
{
    namespace mt
    {
        struct thread_impl_s
        {
            thread_impl_s ()
                : m_id (0),
                  m_is_running (false)
            {}
#if !defined(BSW_USE_PTHREAD)
            HANDLE m_handle;
#else
            pthread_t m_handle;
#endif
            thread_id_t m_id;
            bool        m_is_running;
            mutable mutex_c m_mtx;
        };
        // ==================================================
        struct thread_start_info_s 
        {
            void (*m_function)(void *);
            void * m_arg;              
            thread_impl_s * m_thread;       
        };
        // ==================================================
        thread_c::thread_ret_t thread_c::wrapper_function (void* arg)
        {
            // Get thread startup information
            thread_start_info_s* ti = (thread_start_info_s *) arg;
            // The thread is responsible for freeing the startup information
            std::auto_ptr <thread_start_info_s> ti_guard (ti);
            try
            {
                // Call the actual client thread function
                ti->m_thread->m_id = current_thread_id ();

                ti->m_function (ti->m_arg);
            }
            catch(...)
            {
                // Uncaught exceptions will terminate the application (default behavior
                // according to the C++0x draft)
                std::terminate();
            }

            // The thread is no longer executing
            lock_guard_c <mutex_c> guard(ti->m_thread->m_mtx);
            ti->m_thread->m_is_running = false;
            return 0;
        }
        // ==================================================================================
        thread_c::thread_c ()
        {
            m_pimpl = new thread_impl_s;
        }
        // ----------------------------------------------------------------------------------
        thread_c::thread_c (void (*function)(void *), void * arg)
        {
            m_pimpl = new thread_impl_s;
            thread_start_info_s* ti = new thread_start_info_s;
            ti->m_function = function;
            ti->m_arg      = arg;
            ti->m_thread   = m_pimpl;
            m_pimpl->m_is_running = true;
             // Create the thread
#if !defined(BSW_USE_PTHREAD)
            unsigned int win32_id;
            
            m_pimpl->m_handle = (HANDLE) _beginthreadex (0, 
                                                         0, 
                                                         &thread_c::wrapper_function, 
                                                         (void *) ti, 
                                                         0, 
                                                         &win32_id);
#else 
            if(pthread_create(&m_pimpl->m_handle, NULL, wrapper_function, (void *) ti) != 0)
            {
                m_pimpl->m_handle = 0;
            }
#endif
            if (m_pimpl->m_handle == 0)
            {
                delete m_pimpl;
                delete ti;
                throw std::runtime_error ("Failed to create thread");
            }
        }
        // ----------------------------------------------------------------------------------
        thread_c::~thread_c ()
        {
            if (joinable ())
            {
                std::terminate();
            }
            delete m_pimpl;
        }
        // ----------------------------------------------------------------------------------
        void thread_c::join()
        {
             if (joinable ())
                {
#if !defined(BSW_USE_PTHREAD)
                    WaitForSingleObject (m_pimpl->m_handle, INFINITE);
#else
                    pthread_join (m_pimpl->m_handle, NULL);
#endif
  }
        }
        // ----------------------------------------------------------------------------------
        bool thread_c::joinable() const
        {
            m_pimpl->m_mtx.lock();
            const bool result = m_pimpl->m_is_running;
            m_pimpl->m_mtx.unlock();
            return result;
        }
        // ----------------------------------------------------------------------------------
        thread_id_t thread_c::get_id() const
        {
            return m_pimpl->m_id;
        }
        // ----------------------------------------------------------------------------------            
        unsigned thread_c::hardware_concurrency ()
        {
            return 0;
        }
    } // ns mt
} // ns bsw

