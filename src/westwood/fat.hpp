#ifndef __WESTWOOD_FAT_HPP__
#define __WESTWOOD_FAT_HPP__

#include <vector>

#include "westwood/westwood_api.hpp"
#include "westwood/fat_entry.hpp"

namespace bsw
{
  class input_file_c;
} // ns bsw

namespace westwood
{
  class WESTWOOD_API fat_c
  {
  public:
    fat_c (bsw::input_file_c& file);
    ~fat_c ();
    
    bool empty () const;
    std::size_t size () const;
    const fat_entry_c& operator [] (std::size_t idx) const;
  private:
    std::vector <fat_entry_c> m_fat;
  };
} // ns westwood

#endif
