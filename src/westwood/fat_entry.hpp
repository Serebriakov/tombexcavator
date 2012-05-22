#ifndef __WESTWOOD_FAT_ENTRY_HPP__
#define __WESTWOOD_FAT_ENTRY_HPP__

#include <string>
#include <iosfwd>
#include "westwood/westwood_api.hpp"
#include "thirdparty/my_stdint.h"

namespace westwood
{
  class fat_c;

  class WESTWOOD_API fat_entry_c
  {
    friend class fat_c;
  public:
    enum type_t
      {
	eIMAGE,
	eANIMATION,
	eTEXT
      };
  public:
    const std::string& name () const;
    uint32_t offset () const;
    uint32_t size () const;
    type_t   type () const;
  private:
    fat_entry_c (uint32_t offset, uint32_t size, const std::string& name, type_t type);
  private:
    uint32_t    m_offset;
    uint32_t    m_size;
    std::string m_name;
    type_t      m_type;
  };

  WESTWOOD_API std::ostream& operator << (std::ostream& os, const fat_entry_c::type_t& type);
} // ns westwood

#endif
