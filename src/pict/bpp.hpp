#ifndef __PICT_BPP_HPP__
#define __PICT_BPP_HPP__

#include <iosfwd>
#include "pict/pict_api.hpp"

namespace pict
{
  enum bits_per_pixel_t
    {
      eBPP1,
      eBPP4,
      eBPP8,
      eBPP16,
      eBPP24,
      eBPP32
    };
  PICT_API int color_depth (const bits_per_pixel_t& bpp);

  PICT_API std::ostream& operator << (std::ostream& os, const bits_per_pixel_t& bpp);
} // ns pict

#endif
