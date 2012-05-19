#include "bsw/mt/atomic.hpp"
#if defined(BSW_HAS_ATOMIC)

#if defined(BSW_HAS_ATOMIC_WINDOWS)
#include <windows.h>
#elif defined (BSW_HAS_ATOMIC_MACOSX)
#include <libkern/OSAtomic.h>
#elif defined (BSW_HAS_ATOMIC_GCC)
// use gcc intrisics
#else
#error "Unsupported platform"
#endif


namespace bsw
{
    namespace mt
    {
        long atomic_inc (volatile long& x)
        {
            #if defined(BSW_HAS_ATOMIC_WINDOWS)
                return InterlockedIncrement (&x);
            #elif defined (BSW_HAS_ATOMIC_MACOSX)
                return OSAtomicIncrement32 (&x);
            #elif defined (BSW_HAS_ATOMIC_GCC)
                return __sync_add_and_fetch (&x, 1);
            #else
                #error "Unsupported platform"
            #endif
        }
        // -----------------------------------------------------------
        long atomic_dec (volatile long& x)
        {
            #if defined(BSW_HAS_ATOMIC_WINDOWS)
                return InterlockedDecrement (&x);
            #elif defined (BSW_HAS_ATOMIC_MACOSX)
                return OSAtomicDecrement32 (&x);
            #elif defined (BSW_HAS_ATOMIC_GCC)
                return __sync_sub_and_fetch (&x, 1);
            #else
                #error "Unsupported platform"
            #endif
        }
    } // ns mt
} // ns bsw


#else
static int do_not_cry = 0;
#endif
