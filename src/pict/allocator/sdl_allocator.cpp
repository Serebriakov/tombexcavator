#include "pict/allocator/sdl_allocator.hpp"

#if defined(TOMB_EXCAVATOR_WITH_SDL)
#include <stdexcept>
#include <SDL/SDL.h>
#include <assert.h>
#include <cstring>

namespace pict
{

  class sdl_picture_c : public abstract_picture_c
  {
    friend class sdl_allocator_c;
    sdl_picture_c (unsigned int width,
		   unsigned int height,
		   bits_per_pixel_t bpp);
    ~sdl_picture_c ();

    void _set_palette (const rgba_vector_t& pal, std::size_t from, std::size_t count);
    void _put_pixel (unsigned int x, unsigned int y, const rgba_s& color);
    void _put_pixel (unsigned int x, unsigned int y, const uint8_t color);
    void* opaque ();
    SDL_Surface* m_surface;
  };

  // ----------------------------------------------------------------------
  sdl_picture_c::sdl_picture_c (unsigned int width,
				unsigned int height,
				bits_per_pixel_t bpp)
    : abstract_picture_c (width, height, bpp),
      m_surface (0)
  {
    
    SDL_Surface* surface = SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, color_depth (bpp), 0, 0, 0, 0);
    if (!surface)
      {
	throw std::runtime_error ("failed to create sdl surface");
      }
    m_surface = surface;
  }
  // ----------------------------------------------------------------------
  sdl_picture_c::~sdl_picture_c ()
  {
    if (m_surface)
      {
	SDL_FreeSurface (m_surface);
      }
  }
  // ----------------------------------------------------------------------
  void sdl_picture_c::_set_palette (const rgba_vector_t& pal, std::size_t from, std::size_t count)
  {
    SDL_Palette* palette = (m_surface->format)->palette;
    for (std::size_t i=0; i<count; i++)
      {
	palette->colors [i].r = (Uint8)pal [from + i].r;
	palette->colors [i].g = (Uint8)pal [from + i].g;
	palette->colors [i].b = (Uint8)pal [from + i].b;
	palette->colors [i].unused = (Uint8)pal [from + i].a;
      }
    palette->ncolors = count;
  }
  // ----------------------------------------------------------------------
  void sdl_picture_c::_put_pixel (unsigned int x, unsigned int y, const rgba_s& color)
  {
    
    const int bpp = m_surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)m_surface->pixels + y*m_surface->pitch + x*bpp;
    Uint32 pixel = 0;
    
    switch (bpp)
      {
	case 2:
	  pixel = (color.r & 32);
	  pixel = (pixel << 5) | (color.g & 32);
	  pixel = (pixel << 5) | (color.b & 32);
	  *(Uint16 *)p = (Uint16)pixel;
	  break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) 
	  {
	    p[0] = (Uint8)(color.r);
	    p[1] = (Uint8)color.g;
	    p[2] = (Uint8)color.b;
	  } 
	else 
	  {
            p[0] = (Uint8)color.r;
            p[1] = (Uint8)color.g;
            p[2] = (Uint8)color.b;
	  }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
  }
  // ----------------------------------------------------------------------
  void sdl_picture_c::_put_pixel (unsigned int x, unsigned int y, const uint8_t color)
  {
    const int bpp = m_surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)m_surface->pixels + y*m_surface->pitch + x*bpp;
    *p = color;
  }
  // ----------------------------------------------------------------------
  void* sdl_picture_c::opaque ()
  {
    return m_surface;
  }
  // ======================================================================
  sdl_allocator_c::sdl_allocator_c ()
  {
  }
  // ----------------------------------------------------------------------
  abstract_picture_c* sdl_allocator_c::create (unsigned int width,
					       unsigned int height,
					       bits_per_pixel_t bpp)
  {
    return new sdl_picture_c (width, height, bpp);
  }
  // ----------------------------------------------------------------------
  void sdl_allocator_c::dispose (abstract_picture_c* victim)
  {
    if (sdl_picture_c* obj = dynamic_cast <sdl_picture_c*> (victim))
      {
	delete obj;
      }
  }
  // ----------------------------------------------------------------------
  std::string sdl_allocator_c::describe () const
  {
    return "SDL surface picture allocator";
  }
} // ns pict

#else
static const int dont_cry = 0;
#endif
