#ifndef __BSW_MT_ATOMIC_HPP__
#define __BSW_MT_ATOMIC_HPP__

#include "bsw/mt/os_config.hpp"

#if defined(BSW_HAS_ATOMIC)

#include "bsw/bsw_api.hpp"
#include "thirdparty/my_stdint.h"
namespace bsw
{
    namespace mt
    {
        BSW_API long atomic_inc (volatile long& x);
        BSW_API long atomic_dec (volatile long& x);
    } // ns mt
} // ns bsw
#endif


#endif
