#include <stdexcept>
#include <sstream>

#include "bsw/io/input_stream_decorator.hpp"
#include "bsw/io/input_stream.hpp"

namespace bsw 
{
  input_stream_decorator_c::input_stream_decorator_c (input_stream_c& is)
    : m_is (is),
      m_is_little_endian (true)
  {
  }
  // --------------------------------------------------------------------
  input_stream_decorator_c::input_stream_decorator_c (input_stream_c& is, 
						      bool is_little_endian)
   : m_is (is),
     m_is_little_endian (is_little_endian)
  {
  } 
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::little_endian () const
  {
    return m_is_little_endian;
  }
  // --------------------------------------------------------------------
  void input_stream_decorator_c::little_endian (bool v)
  {
    m_is_little_endian = v;
  }
  // --------------------------------------------------------------------
  file_size_t input_stream_decorator_c::file_size ()
  {
    return m_is.file_size ();
  }
  // --------------------------------------------------------------------
  file_size_t input_stream_decorator_c::tell ()
  {
    return m_is.tell ();
  }
  // --------------------------------------------------------------------
  std::string input_stream_decorator_c::name () const
  {
    return m_is.name ();
  }
  // --------------------------------------------------------------------
  void input_stream_decorator_c::seek (file_size_t pos, whence_t whence)
  {
    if (!m_is.seek (pos, whence))
      {
	std::ostringstream os;
	os << "failed to seek " << pos << " bytes " << whence << " in input stream " << m_is.name ();
	throw std::runtime_error (os.str ());
      }
  }
  // --------------------------------------------------------------------
  void input_stream_decorator_c::seek (file_size_t pos)
  {
    this->seek (pos, eFROM_START);
  }
  // --------------------------------------------------------------------
  std::size_t input_stream_decorator_c::read (char* buffer, std::size_t length)
  {
    return m_is.read (buffer, length);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (char& ch)
  {
    return m_is.read (ch);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (uint8_t& x)
  {
    return m_is.read (x);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (int8_t& x)
  {
    return m_is.read (x);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (uint16_t& x)
  {
    return m_is.read (x, m_is_little_endian);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (int16_t& x)
  {
    return m_is.read (x, m_is_little_endian);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (uint32_t& x)
  {
    return m_is.read (x, m_is_little_endian);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (int32_t& x)
  {
    return m_is.read (x, m_is_little_endian);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (uint64_t& x)
  {
    return m_is.read (x, m_is_little_endian);
  }
  // --------------------------------------------------------------------
  bool input_stream_decorator_c::read (int64_t& x)
  {
    return m_is.read (x, m_is_little_endian);
  }
  
#define DECLARE_READ_OP(TYPE)						\
  input_stream_decorator_c&						\
  operator >> (input_stream_decorator_c& isd,				\
	       TYPE&     ch){						\
    if (!isd.read (ch)) {						\
      std::ostringstream os;						\
      os << "failed to read " << #TYPE					\
	 << " from input stream " << isd.name ();			\
      throw std::runtime_error (os.str ());				\
    }									\
    return isd;								\
  }

  DECLARE_READ_OP (char)
  DECLARE_READ_OP (uint8_t)  
  DECLARE_READ_OP (int8_t)    
  DECLARE_READ_OP (uint16_t)  
  DECLARE_READ_OP (int16_t)   
  DECLARE_READ_OP (uint32_t)  
  DECLARE_READ_OP (int32_t) 
  DECLARE_READ_OP (uint64_t)  
  DECLARE_READ_OP (int64_t)     
} // ns bsw
