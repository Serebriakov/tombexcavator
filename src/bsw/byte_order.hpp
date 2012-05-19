#ifndef __BSW_BYTE_ORDER_HPP__
#define __BSW_BYTE_ORDER_HPP__

#include "thirdparty/my_stdint.h"

namespace bsw
{
  /**
     This class contains a number of static methods
     to convert between big-endian and little-endian
     integers of various sizes.
  */
  class byte_order_c
  {
  public:
    static int16_t  flipBytes (int16_t  value);
    static uint16_t flipBytes (uint16_t value);
    static int32_t  flipBytes (int32_t  value);
    static uint32_t flipBytes (uint32_t value);
    static int64_t  flipBytes (int64_t  value);
    static uint64_t flipBytes (uint64_t value);


    static int16_t  toBigEndian (int16_t  value);
    static uint16_t toBigEndian (uint16_t value);
    static int32_t  toBigEndian (int32_t  value);
    static uint32_t toBigEndian (uint32_t value);
    static int64_t  toBigEndian (int64_t  value);
    static uint64_t toBigEndian (uint64_t value);

    static int16_t  fromBigEndian (int16_t  value);
    static uint16_t fromBigEndian (uint16_t value);
    static int32_t  fromBigEndian (int32_t  value);
    static uint32_t fromBigEndian (uint32_t value);
    static int64_t  fromBigEndian (int64_t  value);
    static uint64_t fromBigEndian (uint64_t value);

    static int16_t  toLittleEndian (int16_t  value);
    static uint16_t toLittleEndian (uint16_t value);
    static int32_t  toLittleEndian (int32_t  value);
    static uint32_t toLittleEndian (uint32_t value);
    static int64_t  toLittleEndian (int64_t  value);
    static uint64_t toLittleEndian (uint64_t value);

    static int16_t  fromLittleEndian (int16_t  value);
    static uint16_t fromLittleEndian (uint16_t value);
    static int32_t  fromLittleEndian (int32_t  value);
    static uint32_t fromLittleEndian (uint32_t value);
    static int64_t  fromLittleEndian (int64_t  value);
    static uint64_t fromLittleEndian (uint64_t value);

    static int16_t  toNetwork (int16_t  value);
    static uint16_t toNetwork (uint16_t value);
    static int32_t  toNetwork (int32_t  value);
    static uint32_t toNetwork (uint32_t value);
    static int64_t  toNetwork (int64_t  value);
    static uint64_t toNetwork (uint64_t value);

    static int16_t  fromNetwork (int16_t  value);
    static uint16_t fromNetwork (uint16_t value);
    static int32_t  fromNetwork (int32_t  value);
    static uint32_t fromNetwork (uint32_t value);
    static int64_t  fromNetwork (int64_t  value);
    static uint64_t fromNetwork (uint64_t value);
  };


  //
  // inlines
  //
  inline uint16_t byte_order_c::flipBytes(uint16_t value)
  {
    return (uint16_t) (((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00));
  }


  inline int16_t byte_order_c::flipBytes(int16_t value)
  {
    return int16_t(flipBytes(uint16_t(value)));
  }


  inline uint32_t byte_order_c::flipBytes(uint32_t value)
  {
    return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
      | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
  }


  inline int32_t byte_order_c::flipBytes(int32_t value)
  {
    return int32_t(flipBytes(uint32_t(value)));
  }


  inline uint64_t byte_order_c::flipBytes(uint64_t value)
  {
    uint32_t hi = uint32_t(value >> 32);
    uint32_t lo = uint32_t(value & 0xFFFFFFFF);
    return uint64_t(flipBytes(hi)) | (uint64_t(flipBytes(lo)) << 32);
  }


  inline int64_t byte_order_c::flipBytes(int64_t value)
  {
    return int64_t(flipBytes(uint64_t(value)));
  }


  //
  // some macro trickery to automate the method implementation
  //
#define BSW_IMPLEMENT_BYTEORDER_NOOP_(op, type)	\
  inline type byte_order_c::op(type value)	\
  {						\
    return value;				\
  }
#define BSW_IMPLEMENT_BYTEORDER_FLIP_(op, type)	\
  inline type byte_order_c::op(type value)	\
  {						\
    return flipBytes(value);			\
  }


#define BSW_IMPLEMENT_BYTEORDER_NOOP(op)		\
  BSW_IMPLEMENT_BYTEORDER_NOOP_(op, int16_t)	\
    BSW_IMPLEMENT_BYTEORDER_NOOP_(op, uint16_t)	\
    BSW_IMPLEMENT_BYTEORDER_NOOP_(op, int32_t)	\
    BSW_IMPLEMENT_BYTEORDER_NOOP_(op, uint32_t)	\
    BSW_IMPLEMENT_BYTEORDER_NOOP_(op, int64_t)	\
    BSW_IMPLEMENT_BYTEORDER_NOOP_(op, uint64_t)

#define BSW_IMPLEMENT_BYTEORDER_FLIP(op)		\
  BSW_IMPLEMENT_BYTEORDER_FLIP_(op, int16_t)	\
    BSW_IMPLEMENT_BYTEORDER_FLIP_(op, uint16_t)	\
    BSW_IMPLEMENT_BYTEORDER_FLIP_(op, int32_t)	\
    BSW_IMPLEMENT_BYTEORDER_FLIP_(op, uint32_t)	\
    BSW_IMPLEMENT_BYTEORDER_FLIP_(op, int64_t)	\
    BSW_IMPLEMENT_BYTEORDER_FLIP_(op, uint64_t)



#if defined(ATLAS_PLATFORM_BIG_ENDIAN) 
#define BSW_IMPLEMENT_BYTEORDER_BIG BSW_IMPLEMENT_BYTEORDER_NOOP
#define BSW_IMPLEMENT_BYTEORDER_LIT BSW_IMPLEMENT_BYTEORDER_FLIP
#else
#define BSW_IMPLEMENT_BYTEORDER_BIG BSW_IMPLEMENT_BYTEORDER_FLIP
#define BSW_IMPLEMENT_BYTEORDER_LIT BSW_IMPLEMENT_BYTEORDER_NOOP
#endif


  BSW_IMPLEMENT_BYTEORDER_BIG(toBigEndian)
    BSW_IMPLEMENT_BYTEORDER_BIG(fromBigEndian)
    BSW_IMPLEMENT_BYTEORDER_BIG(toNetwork)
    BSW_IMPLEMENT_BYTEORDER_BIG(fromNetwork)
    BSW_IMPLEMENT_BYTEORDER_LIT(toLittleEndian)
    BSW_IMPLEMENT_BYTEORDER_LIT(fromLittleEndian)


    } 


#endif 
