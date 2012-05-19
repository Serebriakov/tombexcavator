#ifndef __BSW_MT_MUTEX_HPP__
#define __BSW_MT_MUTEX_HPP__

#include "bsw/bsw_api.hpp"
#include "thirdparty/my_stdint.h"

namespace bsw
{
    namespace mt
    {
        class conditional_variable_c;

        struct mutex_impl_s;
        class BSW_API mutex_c 
        {
            friend class conditional_variable_c;
        public:
            mutex_c ();
            ~mutex_c ();

            void lock ();
            bool try_lock ();
            void unlock ();
        private:
            mutex_c (const mutex_c&);
            mutex_c& operator = (const mutex_c&);
        private:
            mutex_impl_s* m_pimpl;
        };
        // ===================================================
        class BSW_API null_mutex_c 
        {
        public:
            null_mutex_c ();
            ~null_mutex_c ();

            void lock ();
            bool try_lock ();
            void unlock ();
        private:
            null_mutex_c (const null_mutex_c&);
            null_mutex_c& operator = (const null_mutex_c&);
        };
        // ===================================================
        struct recursive_mutex_impl_s;
        class BSW_API recursive_mutex_c 
        {
            friend class conditional_variable_c;
        public:

            recursive_mutex_c ();
            ~recursive_mutex_c ();

            void lock ();
            bool try_lock ();
            void unlock ();
        private:
            recursive_mutex_c (const recursive_mutex_c&);
            recursive_mutex_c& operator = (const recursive_mutex_c&);
        private:
            recursive_mutex_impl_s* m_pimpl;
        };
        // ==================================================
        template <class T>
        class lock_guard_c 
        {
        public:
            typedef T mutex_t;

            lock_guard_c ();
            explicit lock_guard_c (mutex_t &mtx);
            ~lock_guard_c ();
        private:
            mutex_t* m_mutex;
        };
        // ===========================================================================
        struct conditional_variable_impl_s;

        class BSW_API conditional_variable_c 
        {
        public:
            enum status_t
            {
                eOK,
                eEXPIRED,
                eFAILED
            };
        public:
            conditional_variable_c ();
            ~conditional_variable_c ();

            void wait (mutex_c& mutex);
            void wait (recursive_mutex_c& mutex);

            status_t wait (mutex_c& mutex, uint32_t timeout_ms);
            status_t wait (recursive_mutex_c& mutex, uint32_t timeout_ms);

            void signal ();
            void broadcast ();
        private:
            conditional_variable_c (const conditional_variable_c&);
            conditional_variable_c& operator = (const conditional_variable_c&);
        private:
            conditional_variable_impl_s* m_pimpl;
        };
        // =============================================================================
        struct fast_mutex_impl_s;
        class BSW_API fast_mutex_c 
        {
        public:
            fast_mutex_c ();
            ~fast_mutex_c ();

            void lock ();
            bool try_lock ();
            void unlock ();
        private:
            fast_mutex_c (const fast_mutex_c&);
            fast_mutex_c& operator = (const fast_mutex_c&);
        private:
            fast_mutex_impl_s* m_pimpl;
        };
    } // ns mt
} // ns bsw

// ===================================================================
// Implementation
// ===================================================================
namespace bsw
{
    namespace mt
    {
        // ==================================================================
        inline null_mutex_c::null_mutex_c () {}
        inline null_mutex_c::~null_mutex_c () {}
        inline void null_mutex_c::lock () {}
        inline bool null_mutex_c::try_lock () {return true;}
        inline void null_mutex_c::unlock () {}
        // =================================================================
        template <class T>
        lock_guard_c <T>::lock_guard_c () 
            : m_mutex(0) 
        {
        }
        // ------------------------------------------------------

        template <class T>
        lock_guard_c <T>::lock_guard_c (mutex_t &mtx)
        {
            m_mutex = &mtx;
            m_mutex->lock();
        }
        // ------------------------------------------------------
        template <class T>
        lock_guard_c <T>::~lock_guard_c()
        {
            if(m_mutex)
            {
                m_mutex->unlock();
            }
        }
    } // ns mt
} // ns bsw

#endif
