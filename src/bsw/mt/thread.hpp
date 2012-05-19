#ifndef __BSW_MT_THREAD_HPP__
#define __BSW_MT_THREAD_HPP__

#include "bsw/bsw_api.hpp"
#include "thirdparty/predef.h"
#include "bsw/mt/thread_id.hpp"

#if defined(PREDEF_OS_WINDOWS)
#define BSW_THREAD_RETURN_TYPE unsigned int
#else
#define BSW_THREAD_RETURN_TYPE void*
#endif

#if defined(PREDEF_COMPILER_MINGW32) || (defined(_MSC_VER) && (_MSC_VER == 1600))
#define BSW_THREAD_CALL __stdcall
#else
#define BSW_THREAD_CALL
#endif


namespace bsw
{
    namespace mt
    {
        struct thread_impl_s;

        class BSW_API thread_c 
        {
        public:
            thread_c ();
            thread_c (void (*function)(void *), void* arg);

            ~thread_c ();

            void join();
            bool joinable() const;

            thread_id_t get_id() const;
            
            static unsigned hardware_concurrency ();
        private:
            typedef BSW_THREAD_RETURN_TYPE thread_ret_t;

            static thread_ret_t BSW_THREAD_CALL wrapper_function (void * arg);
        private:
            thread_c (const thread_c&);
            thread_c& operator = (const thread_c&);
        private:
            thread_impl_s* m_pimpl;
        };
    } // ns mt
} // ns bsw


#endif
