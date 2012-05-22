#include <stdexcept>
#include <sstream>

#include "westwood/fat.hpp"
#include "bsw/fs/file.hpp"

namespace westwood
{
  fat_c::fat_c (bsw::input_file_c& file)
  {
    bsw::file_size_t curr = file.tell ();
    file.seek (0, bsw::eFROM_START);
    
    const bsw::file_size_t file_size = file.file_size ();
    int k = 0;
    uint32_t prev_offs = 0;
    while (true)
      {
	static const bool is_little_endian = true;
	uint32_t offs;
	if (!file.read (offs, is_little_endian))
	  {
	    std::ostringstream os;
	    os << "failed to read offset of " << k << "-th fat_entry in WESTWOOD PAK file " << file.name ();
	    throw std::runtime_error (os.str ());
	  }
	if (offs == 0 || offs >= file_size || offs <= prev_offs)
	  {
	    break;
	  } 
	uint32_t size = offs - prev_offs;
	prev_offs = offs;
	std::ostringstream name_os;	
	char ch;
	do 
	  {
	    if (!file.read (ch))
	      {
		std::ostringstream os;
		os << "failed to read name of " << k << "-th fat_entry in WESTWOOD PAK file " << file.name ();
	      }
	    name_os << ch;
	  }
	while (ch);
	const std::string& name = name_os.str ();
	
	m_fat.push_back (fat_entry_c (offs, size, name, fat_entry_c::eIMAGE));
      }

    file.seek (curr, bsw::eFROM_START);
  }
  // -------------------------------------------------------------
  fat_c::~fat_c ()
  {
  }
  // -------------------------------------------------------------
  bool fat_c::empty () const
  {
    return m_fat.empty ();
  }
  // -------------------------------------------------------------
  std::size_t fat_c::size () const
  {
    return m_fat.size ();
  }
  // -------------------------------------------------------------
  const fat_entry_c& fat_c::operator [] (std::size_t idx) const
  {
    return m_fat.at (idx);
  }
} // ns westwood
