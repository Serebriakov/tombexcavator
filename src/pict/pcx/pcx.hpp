#ifndef __PICT_PCX_HPP__
#define __PICT_PCX_HPP__

#include <iosfwd>

#include "bsw/io/ios.hpp"

#include "pict/pict_api.hpp"
#include "pict/bpp.hpp"

namespace bsw
{
  class input_stream_c;
} // ns bsw

namespace pict
{
  class abstract_picture_c;
  class allocator_c;
    
  enum pcx_vendor_t
  {
      eVENDOR_2_5,
      eVENDOR_2_8_PAL,
      eVENDOR_2_8_WO_PAL,
      eVENDOR_4,
      eVENDOR_5
  };

  struct PICT_API pcx_info_s
  {
     pcx_vendor_t      vendor;
     unsigned int      width;
     unsigned int      height;
     bits_per_pixel_t  bpp;
     bool              greyscale;
     std::size_t       bytes_per_line;
     unsigned          num_planes;
  };

  PICT_API void load_pcx_info (bsw::input_stream_c& inp, pcx_info_s& pcx_info);

  PICT_API abstract_picture_c* load_pcx (bsw::input_stream_c& inp, allocator_c* allocator, pcx_info_s& pcx_info);
    
  PICT_API std::ostream& operator << (std::ostream& os, pcx_vendor_t vendor);
  PICT_API std::ostream& operator << (std::ostream& os, const pcx_info_s& pcx_info);
  
} // ns pict


#endif
