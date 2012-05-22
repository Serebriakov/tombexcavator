#include <iostream>

#include "westwood/fat_entry.hpp"

namespace westwood
{
  fat_entry_c::fat_entry_c (uint32_t offset        , uint32_t size, 
			    const std::string& name, type_t type)
    : m_offset (offset),
      m_size   (size),
      m_name   (name),
      m_type   (type)
  {
  }
  // --------------------------------------------------------------------------------
  const std::string& fat_entry_c::name () const
  {
    return m_name;
  }
  // --------------------------------------------------------------------------------
  uint32_t fat_entry_c::offset () const
  {
    return m_offset;
  }
  // --------------------------------------------------------------------------------
  fat_entry_c::type_t fat_entry_c::type () const
  {
    return m_type;
  }
  // --------------------------------------------------------------------------------
  uint32_t fat_entry_c::size () const
  {
    return m_size;
  }
  // --------------------------------------------------------------------------------
  std::ostream& operator << (std::ostream& os, const fat_entry_c::type_t& type)
  {
    static const char* s_names [] = 
      {
	"IMAGE",
	"ANIMATION",
	"TEXT"
      };
    os << s_names [type];
    return os;
  }
} // ns westwood
