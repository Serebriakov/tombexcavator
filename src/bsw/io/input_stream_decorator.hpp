#ifndef __BSW_IO_INPUT_STREAM_DECORATOR_HPP__
#define __BSW_IO_INPUT_STREAM_DECORATOR_HPP__

#include <cstddef>
#include <string>

#include "thirdparty/my_stdint.h"
#include "bsw/bsw_api.hpp"
#include "bsw/io/ios.hpp"

namespace bsw
{
  class input_stream_c;
  
  class BSW_API input_stream_decorator_c 
  {
  public:
    input_stream_decorator_c (input_stream_c& is);
    input_stream_decorator_c (input_stream_c& is, bool is_little_endian);

    bool little_endian () const;
    void little_endian (bool v);

    std::string name () const;

    file_size_t file_size ();

    file_size_t tell ();
    
    void seek (file_size_t pos, whence_t whence);
    void seek (file_size_t pos);
    
    std::size_t read (char* buffer, std::size_t length);

    bool read (char& ch);
    bool read (uint8_t& x);
    bool read (int8_t& x);
    
    bool read (uint16_t& x);
    bool read (int16_t& x);

    bool read (uint32_t& x);
    bool read (int32_t& x);

    bool read (uint64_t& x);
    bool read (int64_t& x);
  private:
    input_stream_c& m_is;
    bool            m_is_little_endian;
  };

  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, char&     x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, uint8_t&  x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, int8_t&   x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, uint16_t& x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, int16_t&  x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, uint32_t& x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, int32_t&  x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, uint64_t& x);
  BSW_API input_stream_decorator_c& operator >> (input_stream_decorator_c& isd, int64_t&  x);
} // ns bsw

#endif
