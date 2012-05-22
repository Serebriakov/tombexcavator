#ifndef __BSW_IO_INPUT_STREAM_HPP__
#define __BSW_IO_INPUT_STREAM_HPP__

#include <cstddef>
#include <string>

#include "bsw/bsw_api.hpp"
#include "bsw/io/ios.hpp"

#include "thirdparty/my_stdint.h"


namespace bsw
{
  
  class BSW_API input_stream_c
  {
  public:
    virtual ~input_stream_c ();
    
    virtual const std::string& name () const = 0;

    virtual file_size_t file_size () = 0;

    virtual file_size_t tell () = 0;
    
    virtual bool seek (file_size_t pos, whence_t whence) = 0;
    
    virtual std::size_t read (char* buffer, std::size_t length) = 0;

    virtual bool read (char& ch) = 0;

    virtual bool read (uint8_t& x) = 0;
    virtual bool read (int8_t& x) = 0;
    
    virtual bool read (uint16_t& x, bool is_little_endian) = 0;
    virtual bool read (int16_t& x , bool is_little_endian) = 0;

    virtual bool read (uint32_t& x, bool is_little_endian) = 0;
    virtual bool read (int32_t& x , bool is_little_endian) = 0;

    virtual bool read (uint64_t& x, bool is_little_endian) = 0;
    virtual bool read (int64_t& x , bool is_little_endian) = 0;
  };
} // ns bsw

#endif
