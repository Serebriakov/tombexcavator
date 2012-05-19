#ifndef __BSW_SAFE_CAST_HPP__
#define __BSW_SAFE_CAST_HPP__

namespace bsw
{
    // safely cast between types without breaking strict aliasing rules
    template <typename ReturnType, typename OriginalType>
    ReturnType SafeCast( OriginalType Variable )
    {
        union
        {
            OriginalType    In;
            ReturnType      Out;
        };
        Out = 0;
        In = Variable;
        return Out;
    }
}

#endif
