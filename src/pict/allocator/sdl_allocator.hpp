#ifndef __PICT_ALLOCATOR_SDL_ALLOCATOR_HPP__
#define __PICT_ALLOCATOR_SDL_ALLOCATOR_HPP__

#include "config.h"

#if defined(TOMB_EXCAVATOR_WITH_SDL)

#include "pict/abstract_picture.hpp"
#include "pict/allocator.hpp"

namespace pict
{
  class PICT_API sdl_allocator_c : public allocator_c
  {
  public:
    abstract_picture_c* create (unsigned int width,
				unsigned int height,
				bits_per_pixel_t bpp);

    void dispose (abstract_picture_c* victim);
    std::string describe () const;
  };
} // ns pict

#endif


#endif
