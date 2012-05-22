#ifndef __PICT_BMP_HPP__
#define __PICT_BMP_HPP__

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

  enum bmp_compression_method_t
    {
      eBMP_RGB = 0,
      eBMP_RLE8,
      eBMP_RLE4,
      eBMP_BITFIELDS, 
      eBMP_JPEG,      
      eBMP_PNG
    }; 

  enum bmp_kind_t
    {
      eBMP_BM,
      eBMP_BA,
      eBMP_CI,
      eBMP_CP,
      eBMP_IC,
      eBMP_PT
    };
  
  struct PICT_API bmp_info_s
  {
    bmp_kind_t               kind;
    unsigned int             width;
    unsigned int             height;
    bits_per_pixel_t         bpp;
    bsw::file_size_t         offset_to_data;
    bmp_compression_method_t compression_method;
    unsigned int             colors_in_palette;
    unsigned int             rgb_length;
  };

  PICT_API void load_bmp_info (bsw::input_stream_c& inp, bmp_info_s& bi);

  PICT_API abstract_picture_c* load_bmp (bsw::input_stream_c& inp, allocator_c* allocator);

  PICT_API std::ostream& operator << (std::ostream& os, 
				      const bmp_kind_t& kind);

  PICT_API std::ostream& operator << (std::ostream& os, 
				      const bmp_compression_method_t& bmp_compression_method);

  PICT_API std::ostream& operator << (std::ostream& os, 
				      const bmp_info_s& bmp_info);
  
} // ns pict

#endif
