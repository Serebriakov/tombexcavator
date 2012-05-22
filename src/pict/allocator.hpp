#ifndef __PICT_ALLOCATOR_HPP__
#define __PICT_ALLOCATOR_HPP__

#include <string>

#include "pict/pict_api.hpp"
#include "pict/bpp.hpp"

namespace pict
{
  class abstract_picture_c;

  class PICT_API allocator_c
  {
  public:
    virtual ~allocator_c ();

    virtual abstract_picture_c* create (unsigned int width,
					unsigned int height,
					bits_per_pixel_t bpp) = 0;

    virtual void dispose (abstract_picture_c* victim) = 0;

    virtual std::string describe () const = 0;
  private:
    allocator_c (const allocator_c&);
    allocator_c& operator = (const allocator_c&);
  };

} // ns pict

#endif
