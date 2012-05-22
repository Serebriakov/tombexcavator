#ifndef __BSW_FS_FILE_HPP__
#define __BSW_FS_FILE_HPP__

#include <fstream>

#include "bsw/bsw_api.hpp"
#include "bsw/io/input_stream.hpp"

namespace bsw
{
  class BSW_API input_file_c : public input_stream_c
  {
  public:
    input_file_c (const char* path);
    ~input_file_c ();
    
    const std::string& name () const;

    file_size_t file_size ();

    file_size_t tell ();
    bool seek (file_size_t pos, whence_t whence);
    
    std::size_t read (char* buffer, std::size_t length);

    bool read (char& ch);

    bool read (uint8_t& x);
    bool read (int8_t& x);
    
    bool read (uint16_t& x, bool is_little_endian = true);
    bool read (int16_t& x , bool is_little_endian = true);

    bool read (uint32_t& x, bool is_little_endian = true);
    bool read (int32_t& x , bool is_little_endian = true);

    bool read (uint64_t& x, bool is_little_endian = true);
    bool read (int64_t& x , bool is_little_endian = true);
  private:
    std::ifstream m_ifs;
    std::string   m_name;
  };
} // ns fs

#endif
