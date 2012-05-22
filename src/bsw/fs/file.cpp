#include <ios>
#include <sstream>
#include <stdexcept>

#include "bsw/fs/file.hpp"
#include "bsw/byte_order.hpp"

namespace bsw
{
  namespace priv
  {
    template <typename T>
    inline
    bool read (std::ifstream& ifs, T& x, bool is_little_endian)
    {
      ifs.read ((char*)&x, sizeof (T));
      std::size_t count = ifs.gcount ();
      if (count != sizeof (T))
	{
	  return false;
	}
      
      if (is_little_endian)
	{
	  x = byte_order_c::fromLittleEndian (x);
	}
      else
	{
	  x = byte_order_c::fromBigEndian (x);
	}
      return true;
    }
  } // ns priv
  // ================================================================
  input_file_c::input_file_c (const char* path)
    : m_ifs (path, std::ifstream::in | std::ifstream::binary),
      m_name (path)
  {
    if (!m_ifs)
      {
	std::ostringstream os;
	os << "Cant open file [" << path << "] for reading" << std::endl;
	throw std::runtime_error (os.str ());
      }
  }
  // -----------------------------------------------------------------
  input_file_c::~input_file_c ()
  {
  }
  // -----------------------------------------------------------------
  const std::string& input_file_c::name () const
  {
    return m_name;
  }
  // -----------------------------------------------------------------
  file_size_t input_file_c::file_size ()
  {
    const file_size_t curr = m_ifs.tellg ();
    m_ifs.seekg (0, std::ios::end);
    const file_size_t res = m_ifs.tellg ();
    m_ifs.seekg (curr, std::ios::beg);
    return res;
  }
  // -----------------------------------------------------------------
  file_size_t input_file_c::tell ()
  {
    return m_ifs.tellg ();
  }
  // -----------------------------------------------------------------
  bool input_file_c::seek (file_size_t pos, whence_t whence)
  {
    switch (whence)
      {
      case eFROM_START:
	m_ifs.seekg (pos, std::ios::beg);
	break;
      case eFROM_END:
	m_ifs.seekg (pos, std::ios::end);
	break;
      case eFROM_CURRENT:
	m_ifs.seekg (pos, std::ios::cur);
	break;
      }
    if (!m_ifs)
      {
	return false;
      }
    return true;
  }
  // -----------------------------------------------------------------
  std::size_t input_file_c::read (char* buffer, std::size_t length)
  {
    m_ifs.read (buffer, length);
    return m_ifs.gcount ();
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (char& x)
  {
    m_ifs.read ((char*)&x, sizeof (x));
    return (m_ifs.gcount () == sizeof (x));
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (uint8_t& x)
  {
    m_ifs.read ((char*)&x, sizeof (x));
    return (m_ifs.gcount () == sizeof (x));
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (int8_t& x)
  {
    m_ifs.read ((char*)&x, sizeof (x));
    return (m_ifs.gcount () == sizeof (x));
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (uint16_t& x, bool is_little_endian)
  {
    return priv::read (m_ifs, x, is_little_endian);
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (int16_t& x , bool is_little_endian)
  {
    return priv::read (m_ifs, x, is_little_endian);
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (uint32_t& x, bool is_little_endian)
  {
    return priv::read (m_ifs, x, is_little_endian);
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (int32_t& x , bool is_little_endian)
  {
    return priv::read (m_ifs, x, is_little_endian);
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (uint64_t& x, bool is_little_endian)
  {
    return priv::read (m_ifs, x, is_little_endian);
  }
  // -----------------------------------------------------------------
  bool input_file_c::read (int64_t& x , bool is_little_endian)
  {
    return priv::read (m_ifs, x, is_little_endian);
  }
} // ns bsw

