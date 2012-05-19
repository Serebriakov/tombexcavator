#include "thirdparty/predef.h"

#include "bsw/mt/os_config.hpp"
// Check if we can support the assembly language level implementation (otherwise
// revert to the system API)
#if (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))) || \
    (defined(_MSC_VER) && ( (_MSC_VER > 1400) && (defined(_M_IX86) || defined(_M_X64) )) || ((_MSC_VER == 1400) && (defined(_M_IX86)))) || \
    (defined(__GNUC__) && (defined(__ppc__)))
  #define BSW_FAST_MUTEX_ASM
#else
  #define BSW_FAST_MUTEX_SYS
#endif  

#if !defined(BSW_USE_PTHREAD)
#if defined (_WIN32_WINNT)
#undef _WIN32_WINNT
#endif 
#define  _WIN32_WINNT 0x0403
#include <windows.h>
#define _CONDITION_EVENT_ONE 0
#define _CONDITION_EVENT_ALL 1
#else
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#if defined(BSW_FAST_MUTEX_ASM)
#include <sched.h>
#endif
#endif

#include "bsw/mt/mutex.hpp"

namespace bsw
{
    namespace mt
    {
        struct mutex_impl_s
        {
#if !defined(BSW_USE_PTHREAD)
            CRITICAL_SECTION m_handle;
            bool             m_locked;
            mutex_c*         m_owner;
#else
            pthread_mutex_t m_handle;
#endif
        };
        // ================================================================
        mutex_c::mutex_c()
        {
            m_pimpl = new mutex_impl_s;
#if !defined(BSW_USE_PTHREAD)
            m_pimpl->m_locked = false;
            InitializeCriticalSection(&m_pimpl->m_handle);
            m_pimpl->m_owner = this;
#else
            pthread_mutex_init(&m_pimpl->m_handle, NULL);
#endif
        }
        // ---------------------------------------------------------------
        mutex_c::~mutex_c ()
        {
#if !defined(BSW_USE_PTHREAD)
            DeleteCriticalSection(&m_pimpl->m_handle);
#else
            pthread_mutex_destroy(&m_pimpl->m_handle);
#endif
            delete m_pimpl;
        }
        // ---------------------------------------------------------------
        void mutex_c::lock()
        {
#if !defined(BSW_USE_PTHREAD)
            EnterCriticalSection(&m_pimpl->m_handle);
            while (m_pimpl->m_locked) Sleep(1000); // Simulate deadlock...
            m_pimpl->m_locked = true;
#else
            pthread_mutex_lock(&m_pimpl->m_handle);
#endif
        }
        // ---------------------------------------------------------------
        bool mutex_c::try_lock()
        {
#if !defined(BSW_USE_PTHREAD)

            bool ret = (TryEnterCriticalSection(&m_pimpl->m_handle) ? true : false);
            if(ret && m_pimpl->m_locked)
            {
                LeaveCriticalSection(&m_pimpl->m_handle);
                ret = false;
            }
            return ret;
#else
            return (pthread_mutex_trylock(&m_pimpl->m_handle) == 0) ? true : false;
#endif
        }
        // ---------------------------------------------------------------
        void mutex_c::unlock()
        {
#if !defined(BSW_USE_PTHREAD)
            m_pimpl->m_locked = false;
            LeaveCriticalSection (&m_pimpl->m_handle);
#else
            pthread_mutex_unlock(&m_pimpl->m_handle);
#endif
        }
        // ====================================================================
        struct recursive_mutex_impl_s
        {
#if !defined(BSW_USE_PTHREAD)
            CRITICAL_SECTION m_handle;
            recursive_mutex_c* m_owner;
#else
            pthread_mutex_t m_handle;
#endif
        };
        // ================================================================
        recursive_mutex_c::recursive_mutex_c()
        {
            m_pimpl = new recursive_mutex_impl_s;
#if !defined(BSW_USE_PTHREAD)
            InitializeCriticalSection (&m_pimpl->m_handle);
            m_pimpl->m_owner = this;
#else
            pthread_mutexattr_t attr;
            pthread_mutexattr_init (&attr);
            pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init (&m_pimpl->m_handle, &attr);
#endif
        }
        // ---------------------------------------------------------------
        recursive_mutex_c::~recursive_mutex_c ()
        {
#if !defined(BSW_USE_PTHREAD)
            DeleteCriticalSection (&m_pimpl->m_handle);
#else
            pthread_mutex_destroy (&m_pimpl->m_handle);
#endif
            delete m_pimpl;
        }
        // ---------------------------------------------------------------
        void recursive_mutex_c::lock()
        {
#if !defined(BSW_USE_PTHREAD)
            EnterCriticalSection (&m_pimpl->m_handle);
#else
            pthread_mutex_lock (&m_pimpl->m_handle);
#endif
        }
        // ---------------------------------------------------------------
        bool recursive_mutex_c::try_lock()
        {
#if !defined(BSW_USE_PTHREAD)

            const bool ret = (TryEnterCriticalSection (&m_pimpl->m_handle) ? true : false);
            return ret;
#else
            return (pthread_mutex_trylock (&m_pimpl->m_handle) == 0) ? true : false;
#endif
        }
        // ---------------------------------------------------------------
        void recursive_mutex_c::unlock()
        {
#if !defined(BSW_USE_PTHREAD)
            LeaveCriticalSection (&m_pimpl->m_handle);
#else
            pthread_mutex_unlock (&m_pimpl->m_handle);
#endif
        }
        // ==================================================================
        struct conditional_variable_impl_s
        {
#if !defined(BSW_USE_PTHREAD)
            void _wait()
            {
                // Wait for either event to become signaled due to signal() or
                // broadcast() being called
                const int result = WaitForMultipleObjects(2, m_events, FALSE, INFINITE);

                // Check if we are the last waiter
                EnterCriticalSection(&m_waiters_lock);
                -- m_waiters;
                const bool last_waiter = (result == (WAIT_OBJECT_0 + _CONDITION_EVENT_ALL)) && (m_waiters == 0);
                LeaveCriticalSection(&m_waiters_lock);

                // If we are the last waiter to be notified to stop waiting, reset the event
                if(last_waiter)
                {
                    ResetEvent(m_events [_CONDITION_EVENT_ALL]);
                }
            }
            // ---------------------------------------------------------------------
            int _wait (DWORD ms)
            {
                // Wait for either event to become signaled due to signal() or
                // broadcast() being called
                const int result = WaitForMultipleObjects(2, m_events, FALSE, ms);
                if (result == WAIT_TIMEOUT)
                {
                    return 0;
                }
                // Check if we are the last waiter
                EnterCriticalSection(&m_waiters_lock);
                -- m_waiters;
                const bool last_waiter = (result == (WAIT_OBJECT_0 + _CONDITION_EVENT_ALL)) && (m_waiters == 0);
                LeaveCriticalSection(&m_waiters_lock);

                // If we are the last waiter to be notified to stop waiting, reset the event
                if(last_waiter)
                {
                    ResetEvent(m_events [_CONDITION_EVENT_ALL]);
                }
                return 1;
            }
            HANDLE m_events[2];         
            unsigned int m_waiters;     
            CRITICAL_SECTION m_waiters_lock; 
#else
            pthread_cond_t m_handle;
#endif
        };
        // ----------------------------------------------------------------
        conditional_variable_c::conditional_variable_c ()
        {
            m_pimpl = new conditional_variable_impl_s;
#if !defined(BSW_USE_PTHREAD)
            m_pimpl->m_waiters = 0;
            m_pimpl->m_events [_CONDITION_EVENT_ONE] = CreateEvent (NULL, FALSE, FALSE, NULL);
            m_pimpl->m_events [_CONDITION_EVENT_ALL] = CreateEvent (NULL, TRUE, FALSE, NULL);
            InitializeCriticalSection(&m_pimpl->m_waiters_lock);
#else
            pthread_cond_init (&m_pimpl->m_handle, NULL);
#endif
        }
        // -----------------------------------------------------------------
        conditional_variable_c::~conditional_variable_c ()
        {
#if !defined(BSW_USE_PTHREAD)
            CloseHandle (m_pimpl->m_events [_CONDITION_EVENT_ONE]);
            CloseHandle (m_pimpl->m_events [_CONDITION_EVENT_ALL]);
            DeleteCriticalSection(&m_pimpl->m_waiters_lock);
#else
            pthread_cond_destroy (&m_pimpl->m_handle);
#endif
            delete m_pimpl;
        }
        // -----------------------------------------------------------------
        template <class mutexT>
        inline void impl_wait(conditional_variable_impl_s* impl, mutexT* mtx)
        {
#if !defined(BSW_USE_PTHREAD)
            // Increment number of waiters
            EnterCriticalSection(&impl->m_waiters_lock);
            ++ impl->m_waiters;
            LeaveCriticalSection(&impl->m_waiters_lock);

            // Release the mutex while waiting for the condition (will decrease
            // the number of waiters when done)...
            mtx->m_owner->unlock();
            impl->_wait();
            mtx->m_owner->lock();
#else
            pthread_cond_wait(&impl->m_handle, &mtx->m_handle);
#endif
        }
        // -----------------------------------------------------------------
        template <class mutexT>
        inline conditional_variable_c::status_t impl_wait(conditional_variable_impl_s* impl, mutexT* mtx, uint32_t ms)
        {
#if !defined(BSW_USE_PTHREAD)
            // Increment number of waiters
            EnterCriticalSection(&impl->m_waiters_lock);
            ++ impl->m_waiters;
            LeaveCriticalSection(&impl->m_waiters_lock);

            // Release the mutex while waiting for the condition (will decrease
            // the number of waiters when done)...
            mtx->m_owner->unlock();
            int wrc = impl->_wait(ms);
            mtx->m_owner->lock();
            if (wrc == 1)
            {
                return conditional_variable_c::eOK;
            }
            return conditional_variable_c::eEXPIRED;
#else
            int retval;
            struct timeval delta;
            struct timespec abstime; 
            gettimeofday(&delta, NULL);

            abstime.tv_sec = delta.tv_sec + (ms/1000);
            abstime.tv_nsec = (delta.tv_usec + (ms%1000) * 1000) * 1000;
            if ( abstime.tv_nsec > 1000000000 ) 
            {
                abstime.tv_sec += 1;
                abstime.tv_nsec -= 1000000000;
            } 
tryagain:
            retval = pthread_cond_timedwait(&impl->m_handle, &mtx->m_handle, &abstime);
            switch (retval) 
            {
            case EINTR:
                goto tryagain;
                break;
            case ETIMEDOUT:
                return conditional_variable_c::eEXPIRED;
                break;
            case 0:
                return conditional_variable_c::eOK;
            default:
                break;
            }
            return conditional_variable_c::eFAILED;

#endif
        }
        // ---------------------------------------------------------------
        void conditional_variable_c::signal ()
        {
#if !defined(BSW_USE_PTHREAD)
            // Are there any waiters?
            EnterCriticalSection (&m_pimpl->m_waiters_lock);
            const bool have_waiters = (m_pimpl->m_waiters > 0);
            LeaveCriticalSection(&m_pimpl->m_waiters_lock);

            // If we have any waiting threads, send them a signal
            if(have_waiters)
            {
                SetEvent(m_pimpl->m_events [_CONDITION_EVENT_ONE]);
            }
#else
            pthread_cond_signal(&m_pimpl->m_handle);
#endif
        }
        // ---------------------------------------------------------------
        void conditional_variable_c::broadcast ()
        {
#if !defined(BSW_USE_PTHREAD)
            // Are there any waiters?
            EnterCriticalSection (&m_pimpl->m_waiters_lock);
            const bool have_waiters = (m_pimpl->m_waiters > 0);
            LeaveCriticalSection (&m_pimpl->m_waiters_lock);

            // If we have any waiting threads, send them a signal
            if(have_waiters)
            {
                SetEvent (m_pimpl->m_events [_CONDITION_EVENT_ALL]);
            }
#else
            pthread_cond_broadcast (&m_pimpl->m_handle);
#endif
        } 
        // --------------------------------------------------------------
        void conditional_variable_c::wait (mutex_c& mutex)
        {
            impl_wait (m_pimpl, mutex.m_pimpl);
        }
        // --------------------------------------------------------------
        void conditional_variable_c::wait (recursive_mutex_c& mutex)
        {
            impl_wait (m_pimpl, mutex.m_pimpl);
        }
        // --------------------------------------------------------------
        conditional_variable_c::status_t conditional_variable_c::wait (mutex_c& mutex, uint32_t timeout_ms)
        {
            return impl_wait (m_pimpl, mutex.m_pimpl, timeout_ms);
        }
        // --------------------------------------------------------------
        conditional_variable_c::status_t conditional_variable_c::wait (recursive_mutex_c& mutex, uint32_t timeout_ms)
        {
            return impl_wait (m_pimpl, mutex.m_pimpl, timeout_ms);
        }
        // ==============================================================
        // fast mutex
        // ==============================================================
#if defined (BSW_FAST_MUTEX_SYS)
        struct fast_mutex_impl_s
        {
#if !defined(BSW_USE_PTHREAD)
            CRITICAL_SECTION m_handle;
            bool             m_locked;
            fast_mutex_c*         m_owner;
#else
            pthread_mutex_t m_handle;
#endif
        };
        // ================================================================
        fast_mutex_c::fast_mutex_c()
        {
            m_pimpl = new fast_mutex_impl_s;
#if !defined(BSW_USE_PTHREAD)
            m_pimpl->m_locked = false;
            InitializeCriticalSection(&m_pimpl->m_handle);
            m_pimpl->m_owner = this;
#else
            pthread_mutex_init(&m_pimpl->m_handle, NULL);
#endif
        }
        // ---------------------------------------------------------------
        fast_mutex_c::~fast_mutex_c ()
        {
#if !defined(BSW_USE_PTHREAD)
            DeleteCriticalSection(&m_pimpl->m_handle);
#else
            pthread_mutex_destroy(&m_pimpl->m_handle);
#endif
            delete m_pimpl;
        }
        // ---------------------------------------------------------------
        void fast_mutex_c::lock()
        {
#if !defined(BSW_USE_PTHREAD)
            EnterCriticalSection(&m_pimpl->m_handle);
            while (m_pimpl->m_locked) Sleep(1000); // Simulate deadlock...
            m_pimpl->m_locked = true;
#else
            pthread_mutex_lock(&m_pimpl->m_handle);
#endif
        }
        // ---------------------------------------------------------------
        bool fast_mutex_c::try_lock()
        {
#if !defined(BSW_USE_PTHREAD)

            bool ret = (TryEnterCriticalSection(&m_pimpl->m_handle) ? true : false);
            if(ret && m_pimpl->m_locked)
            {
                LeaveCriticalSection(&m_pimpl->m_handle);
                ret = false;
            }
            return ret;
#else
            return (pthread_mutex_trylock(&m_pimpl->m_handle) == 0) ? true : false;
#endif
        }
        // ---------------------------------------------------------------
        void fast_mutex_c::unlock()
        {
#if !defined(BSW_USE_PTHREAD)
            m_pimpl->m_locked = false;
            LeaveCriticalSection (&m_pimpl->m_handle);
#else
            pthread_mutex_unlock(&m_pimpl->m_handle);
#endif
        }
#else

        struct fast_mutex_impl_s
        {
            fast_mutex_impl_s() 
                : mLock(0) 
            {
            }

            inline void lock()
            {
                bool gotLock;
                do 
                {
                    gotLock = try_lock();
                    if(!gotLock)
                    {
#if !defined(BSW_USE_PTHREAD)
                        Sleep(0);
#else
                        sched_yield();
#endif
                    }
                } while(!gotLock);
            }

            inline bool try_lock()
            {
                int oldLock;
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
                asm volatile (
                    "movl $1,%%eax\n\t"
                    "xchg %%eax,%0\n\t"
                    "movl %%eax,%1\n\t"
                    : "=m" (mLock), "=m" (oldLock)
                    :
                : "%eax", "memory"
                    );
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
                int *ptrLock = &mLock;
                __asm {
                    mov eax,1
                        mov ecx,ptrLock
                        xchg eax,[ecx]
                    mov oldLock,eax
                }
#elif defined(__GNUC__) && (defined(__ppc__))
                int newLock = 1;
                asm volatile (
                    "\n1:\n\t"
                    "lwarx  %0,0,%1\n\t"
                    "cmpwi  0,%0,0\n\t"
                    "bne-   2f\n\t"
                    "stwcx. %2,0,%1\n\t"
                    "bne-   1b\n\t"
                    "isync\n"
                    "2:\n\t"
                    : "=&r" (oldLock)
                    : "r" (&mLock), "r" (newLock)
                    : "cr0", "memory"
                    );
#endif
                return (oldLock == 0);
            }

            inline void unlock()
            {
#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
                asm volatile (
                    "movl $0,%%eax\n\t"
                    "xchg %%eax,%0\n\t"
                    : "=m" (mLock)
                    :
                : "%eax", "memory"
                    );
#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
                int *ptrLock = &mLock;
                __asm {
                    mov eax,0
                        mov ecx,ptrLock
                        xchg eax,[ecx]
                }
#elif defined(__GNUC__) && (defined(__ppc__))
                asm volatile (
                    "sync\n\t"  // Replace with lwsync where possible?
                    : : : "memory"
                    );
                mLock = 0;
#endif
            }

        private:

            int mLock;
        }; 


        // ===================================================================
        fast_mutex_c::fast_mutex_c ()
        {
            m_pimpl = new fast_mutex_impl_s;
        }
        // -------------------------------------------------------------------
        fast_mutex_c::~fast_mutex_c ()
        {
            delete m_pimpl;
        }
        // -------------------------------------------------------------------   
        void fast_mutex_c::lock ()
        {
            m_pimpl->lock ();
        }
        // -------------------------------------------------------------------
        bool fast_mutex_c::try_lock ()
        {
            return m_pimpl->try_lock ();
        }
        // -------------------------------------------------------------------
        void fast_mutex_c::unlock ()
        {
            return m_pimpl->unlock ();
        }


#endif

    } // ns mt
} // ns bsw

