#include <iostream>
#include "pict/bpp.hpp"

namespace pict
{

  int color_depth (const bits_per_pixel_t& bpp)
  {
    switch (bpp)
      {
      case eBPP1:
	return 1;
      case eBPP2:
	return 1;
      case eBPP4:
	return 4;
      case eBPP8:
	return 8;
      case eBPP16:
	return 16;
      case eBPP24:
	return 24;
      case eBPP32:
	return 32;
      }
    return 0;
  }
  // ------------------------------------------------------------------------
  std::ostream& operator << (std::ostream& os, const bits_per_pixel_t& bpp)
  {
    os << color_depth (bpp);
    return os;
  }
} // ns pict
