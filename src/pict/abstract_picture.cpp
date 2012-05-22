#include <stdexcept>
#include <sstream>

#include "pict/abstract_picture.hpp"

namespace pict
{
  abstract_picture_c::abstract_picture_c (unsigned int    width,
					  unsigned int    height,
					  bits_per_pixel_t bpp)
    : m_width        (width),
      m_height       (height),
      m_bpp          (bpp)
  {
  }
  // -------------------------------------------------------------------------
  abstract_picture_c::~abstract_picture_c ()
  {
  }
  // -------------------------------------------------------------------------
  bits_per_pixel_t abstract_picture_c::bpp () const
  {
    return m_bpp;
  }
  // -------------------------------------------------------------------------
  unsigned int abstract_picture_c::width () const
  {
    return m_width;
  }
  // -------------------------------------------------------------------------
  unsigned int abstract_picture_c::height () const
  {
    return m_height;
  }
  // -------------------------------------------------------------------------
  void abstract_picture_c::set_palette (const rgba_vector_t& pal, std::size_t from, std::size_t count)
  {
    if (count == 0)
      {
	return;
      }
    if (!is_indexed ())
      {
	std::ostringstream os;
	os << "Attempt to set palette on non indexed image with depth " << m_bpp;
	throw std::runtime_error (os.str ());
      }
    if (from + count > pal.size ())
      {
	std::ostringstream os;
	os << "Palette range (" << from << ", " << count << ") is out of palette size " << pal.size ();
	throw std::runtime_error (os.str ());
      }
    _set_palette (pal, from, count);
  }
  // -------------------------------------------------------------------------
  void abstract_picture_c::set_palette (const rgba_vector_t& pal)
  {
    this->set_palette (pal, 0, pal.size ());
  }
  // -------------------------------------------------------------------------
  void abstract_picture_c::put_pixel (unsigned int x, unsigned int y, const rgba_s& color)
  {
    if (is_indexed ())
      {
	std::ostringstream os;
	os << "Attempt to set RGBA pel on indexed image with depth " << m_bpp;
	throw std::runtime_error (os.str ());
      }
    if (x >= m_width)
      {
	std::ostringstream os;
	os << "X-coordinate " << x << " is out of image width " << m_width;
	throw std::runtime_error (os.str ());
      }
    if (y >= m_height)
      {
	std::ostringstream os;
	os << "Y-coordinate " << y << " is out of image height " << m_height;
	throw std::runtime_error (os.str ());
      }
    _put_pixel (x, y, color);
  }
  // -------------------------------------------------------------------------
  void abstract_picture_c::put_pixel (unsigned int x, unsigned int y, uint8_t index)
  {
    if (!is_indexed ())
      {
	std::ostringstream os;
	os << "Attempt to set indexed pel on non indexed image with depth " << m_bpp;
	throw std::runtime_error (os.str ());
      }
    if (x >= m_width)
      {
	std::ostringstream os;
	os << "X-coordinate " << x << " is out of image width " << m_width;
	throw std::runtime_error (os.str ());
      }
    if (y >= m_height)
      {
	std::ostringstream os;
	os << "Y-coordinate " << y << " is out of image height " << m_height;
	throw std::runtime_error (os.str ());
      }
    _put_pixel (x, y, index);
  }
  // -------------------------------------------------------------------------
  bool abstract_picture_c::is_indexed () const
  {
    switch (m_bpp)
      {
      case eBPP1:
      case eBPP4:
      case eBPP8:
	return true;
      case eBPP16:
      case eBPP24:
      case eBPP32:
	return false;
      }
    return true;
  }
} // ns pict

