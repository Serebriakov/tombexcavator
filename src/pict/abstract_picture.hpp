#ifndef __PICT_ABSTRACT_PICTURE_HPP__
#define __PICT_ABSTRACT_PICTURE_HPP__

#include <iosfwd>
#include <vector>

#include "pict/pict_api.hpp"
#include "pict/bpp.hpp"
#include "thirdparty/my_stdint.h"

namespace pict
{
  struct PICT_API rgba_s
  {
    rgba_s ()
      : r (0),
	g (0),
	b (0),
	a (0) 
    {
    }

    int r;
    int g;
    int b;
    int a;
  };

  typedef std::vector <rgba_s> rgba_vector_t;

  class PICT_API abstract_picture_c
  {
    friend class allocator_c;
  public:
    bits_per_pixel_t bpp () const;

    unsigned int width () const;
    unsigned int height () const;

    void set_palette (const rgba_vector_t& pal, std::size_t from, std::size_t count);
    void set_palette (const rgba_vector_t& pal);

    void put_pixel (unsigned int x, unsigned int y, const rgba_s& color);
    void put_pixel (unsigned int x, unsigned int y, uint8_t index);

    bool is_indexed () const;

    virtual void* opaque () = 0;
  protected:
    virtual void _set_palette (const rgba_vector_t& pal, std::size_t from, std::size_t count) = 0;
    virtual void _put_pixel (unsigned int x, unsigned int y, const rgba_s& color) = 0;
    virtual void _put_pixel (unsigned int x, unsigned int y, uint8_t index) = 0;
  protected:
    abstract_picture_c (unsigned int width,
			unsigned int height,
			bits_per_pixel_t bpp);
    virtual ~abstract_picture_c ();
  private:
    abstract_picture_c (const abstract_picture_c&);
    abstract_picture_c& operator = (const abstract_picture_c&);
  private:
    unsigned int     m_width;
    unsigned int     m_height;
    bits_per_pixel_t m_bpp;
  };
} // ns pict


#endif
