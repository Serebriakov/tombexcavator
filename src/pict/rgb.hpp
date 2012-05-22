#ifndef __PICT_RGB_HPP__
#define __PICT_RGB_HPP__

#include "thirdparty/my_stdint.h"

namespace pict
{

  typedef uint32_t rgb_t;                        // RGB triplet

  static const rgb_t RGB_MASK = 0x00ffffff;  // masks RGB values
  // ---------------------------------------------------------------------
  inline 
  uint32_t red (rgb_t rgb)                // get red part of RGB
  { 
    return  ( (rgb >> 16) & 0xff); 
  }
  // ---------------------------------------------------------------------
  inline 
  uint32_t green (rgb_t rgb)                // get green part of RGB
  { 
    return  ( (rgb >> 8) & 0xff); 
  }
  // ---------------------------------------------------------------------
  inline 
  uint32_t blue (rgb_t rgb)                // get blue part of RGB
  { 
    return  (rgb & 0xff); 
  }
  // ---------------------------------------------------------------------
  inline 
  uint32_t alpha (rgb_t rgb)                // get alpha part of RGBA
  { 
    return rgb >> 24; 
  }
  // ---------------------------------------------------------------------
  inline 
  rgb_t rgb  (uint32_t r, uint32_t g, uint32_t b)// set RGB value
  { 
    return  (0xffu << 24) |  ( (r & 0xff) << 16) |  ( (g & 0xff) << 8) |  (b & 0xff); 
  }
  // ---------------------------------------------------------------------
  inline 
  rgb_t rgba (uint32_t r, uint32_t g, uint32_t b, uint32_t a)// set RGBA value
  { 
    return  ( (a & 0xff) << 24) |  ( (r & 0xff) << 16) |  ( (g & 0xff) << 8) |  (b & 0xff); 
  }
  // ---------------------------------------------------------------------
  inline 
  int gray (uint32_t r, uint32_t g, uint32_t b)// convert R,G,B to gray 0..255
  { 
    return  (r*11+g*16+b*5)/32; 
  }
  // ---------------------------------------------------------------------
  inline 
  int gray (rgb_t rgb)                // convert RGB to gray 0..255
  { 
    return gray (red (rgb), green (rgb), blue (rgb)); 
  }
  // ---------------------------------------------------------------------
  inline 
  bool is_gray (rgb_t rgb)
  { 
    
    return red (rgb) == green (rgb) && red (rgb) == blue (rgb); 
  }
} // ns pict

#endif
