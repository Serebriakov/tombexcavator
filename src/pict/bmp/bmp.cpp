#include <assert.h>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cstring>

#include "pict/bmp/bmp.hpp"
#include "pict/allocator.hpp"
#include "pict/abstract_picture.hpp"

#include "bsw/io/input_stream_decorator.hpp"

#include "thirdparty/my_stdint.h"

namespace pict
{

  static
  inline
  unsigned int pixels_row_size (unsigned int bits, unsigned int width)
  {
    const unsigned int wb = width*bits;
    unsigned int r = wb / 32;
    if (wb % 32)
      {
	r++;
      }
    return r*4;
  }
  template <int CHUNKS>
  class loader_c;

  template <>
  class loader_c <8>
  {
  public:
    static uint8_t load (uint8_t val, int pos)
    {
      assert (pos == 0);
      return val;
    } 
  };
  // =========================================================
  template <>
  class loader_c <4>
  {
  public:
    static uint8_t load (uint8_t val, int pos)
    {
      static uint8_t shft [] = {
	 4,
	 4,
	 4,
	 4,
	 0,
	 0,
	 0,
	 0
      };
      return ((val >> shft [pos]) & 0x0F);
    } 
  };
  // =========================================================
  template <>
  class loader_c <2>
  {
  public:
    static uint8_t load (uint8_t val, int pos)
    {
      static uint8_t shft [] = {
	 6,
	 6,
	 4,
	 4,
	 2,
	 2,
	 0,
	 0
      };
      return ((val >> shft [pos]) & 0x3);
    } 
  };
  // =========================================================
  template <>
  class loader_c <1>
  {
  public:
    static uint8_t load (uint8_t val, int pos)
    {
      static uint8_t shft [] = {
	 7,
	 6,
	 5,
	 4,
	 3,
	 2,
	 1,
	 0
      };
      return (val >> shft [pos]) & 0x1;
    } 
  };
  // =========================================================
  template <unsigned int BITS>
  class simple_pixels_reader_c
  {
  public:
    typedef uint8_t pixel_t;
  public:
    simple_pixels_reader_c (bsw::input_stream_decorator_c& isd, unsigned int width)
      : m_isd      (isd),
	m_row_size (pixels_row_size (BITS, width)),
	m_row      (m_row_size, 0),
	m_current  (0),
	m_chunk    (0)
    {
    }
    // ---------------------------------------------------------------------------
    void next_pixel (pixel_t& pixel)
    {
      if (m_chunk >= 8)
	{
	  m_current++;
	  m_chunk = 0;
	}
      if (m_current >= m_row_size)
	{
	  throw std::runtime_error ("next_pixel aout of bounds");
	}
      pixel = loader_c <BITS>::load (m_row [m_current], m_chunk);
      m_chunk += BITS;
    }

    void load_row ()
    {
      m_current = 0;
      m_chunk   = 0;
      m_isd.read ( (char*)&m_row [0], m_row_size);
    }
  private:
    bsw::input_stream_decorator_c& m_isd;
  protected:
    unsigned int                   m_row_size;
    std::vector <uint8_t>          m_row;
    unsigned int                   m_current;
    unsigned int                   m_chunk;
    
  };
  // ====================================================================================
  struct bmpfile_header_s 
  {
    unsigned char magic[2];
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;

    bmpfile_header_s (bsw::input_stream_decorator_c& isd)
    {
      isd >> magic [0] >> magic [1] 
	  >> filesz >> creator1 >> creator2 >> bmp_offset;
    }
  };

  struct os2_info_header_s 
  {
    uint32_t  biSize;
    int32_t   biWidth;
    int32_t   biHeight; 
    uint16_t  biPlanes; 
    uint16_t  biBitCount;
    os2_info_header_s  (bsw::input_stream_decorator_c& icd, uint32_t size)
      : biSize (size)
    {
      icd >> biWidth
	  >> biHeight
	  >> biPlanes
	  >> biBitCount;
    }

    void operator () (bmp_info_s& bi)
    {
      bi.width  = biWidth;
      if (biHeight < 0)
	{
	  bi.height = -biHeight;
	  bi.top_down = false;
	}
      else
	{
	  bi.height   = biHeight;
	  bi.top_down = true;
	}
      switch (biBitCount)
	{
	case 1:
	  bi.bpp = eBPP1;
	  break;
	case 2:
	  bi.bpp = eBPP2;
	  break;
	case 4:
	  bi.bpp = eBPP4;
	  break;
	case 8:
	  bi.bpp = eBPP8;
	  break;
	case 16:
	  bi.bpp = eBPP16;
	  break;
	case 24:
	  bi.bpp = eBPP24;
	  break;
	case 32:
	  bi.bpp = eBPP32;
	  break;
	default:
	  {
	    std::ostringstream os;
	    os << "Unsupported color depth " << biBitCount;
	    throw std::runtime_error (os.str ());
	  }
	}
      bi.compression_method = eBMP_RGB;
    }
  };

  struct core_info_header_s 
  {
    os2_info_header_s biOS2Header;

    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
    
    core_info_header_s (bsw::input_stream_decorator_c& icd, uint32_t size)
      : biOS2Header (icd, size)
    {
      icd >> biCompression 
	  >> biSizeImage 
	  >> biXPelsPerMeter 
	  >> biYPelsPerMeter 
	  >> biClrUsed 
	  >> biClrImportant;
    }
    
    void operator () (bmp_info_s& bi)
    {
      biOS2Header (bi);
      switch (biCompression)
	{
	case 0:
	  bi.compression_method = eBMP_RGB;
	  break;
	case 1:
	  bi.compression_method = eBMP_RLE8;
	  break;
	case 2:
	  bi.compression_method = eBMP_RLE4;
	  break;
	case 3:
	  bi.compression_method = eBMP_BITFIELDS;
	  break;
	case 4:
	  bi.compression_method = eBMP_JPEG;
	  break;
	case 5:
	  bi.compression_method = eBMP_PNG;
	  break;
	default:
	  {
	    std::ostringstream os;
	    os << "Unsupported compression method " << biCompression;
	    throw std::runtime_error (os.str ());
	  }
	}
      if (biClrUsed == 0)
	{
	  if (biClrImportant == 0)
	    {
	      switch (biOS2Header.biBitCount)
		{
		case 1:
		  bi.colors_in_palette = 2;
		  break;
		case 2:
		  bi.colors_in_palette = 4;
		  break;
		case 4:
		  bi.colors_in_palette = 16;
		  break;
		case 8:
		  bi.colors_in_palette = 256;
		  break;
		default:
		  bi.colors_in_palette = 0;
		}
	    }
	  else
	    {
	      bi.colors_in_palette = biClrImportant;
	    }
	}
      else
	{
	  bi.colors_in_palette = biClrUsed;
	}
      bi.rgb_length        = 4;
    }
  };
  
  

  struct v2_info_header_s 
  {
    core_info_header_s biCoreHeader;
    uint32_t bmiRedMask;
    uint32_t bmiGreenMask;
    uint32_t bmiBlueMask;

    v2_info_header_s (bsw::input_stream_decorator_c& icd, uint32_t size)
      : biCoreHeader (icd, size)
    {
      icd >> bmiRedMask
	  >> bmiGreenMask
	  >> bmiBlueMask;
    }
    
    void operator () (bmp_info_s& bi)
    {
      biCoreHeader (bi);
    }
  };

  struct v3_info_header_s 
  {
    v2_info_header_s bmiV2Header;
    uint32_t bmiAlphaMask;
    
    v3_info_header_s (bsw::input_stream_decorator_c& icd, uint32_t size)
      : bmiV2Header (icd, size)
    {
      icd >> bmiAlphaMask;
    }

    void operator () (bmp_info_s& bi)
    {
      bmiV2Header (bi);
    }
  };

  struct ciexyz_s 
  {
    int32_t ciexyzX;
    int32_t ciexyzY;
    int32_t ciexyzZ;
  };

  static
  bsw::input_stream_decorator_c& operator >> (bsw::input_stream_decorator_c& isd, ciexyz_s& x)
  {
    isd >> x.ciexyzX >> x.ciexyzY >> x.ciexyzZ;
    return isd;
  }

  struct ciexyz_tripple_s
  {
    ciexyz_s ciexyzRed;
    ciexyz_s ciexyzGreen;
    ciexyz_s ciexyzBlue;
  };

  static
  bsw::input_stream_decorator_c& operator >> (bsw::input_stream_decorator_c& isd, ciexyz_tripple_s& x)
  {
    isd >> x.ciexyzRed >> x.ciexyzGreen >> x.ciexyzBlue;
    return isd;
  }

  struct v4_info_header_s 
  {
    v3_info_header_s bV3InfoHeader;
    uint32_t        bV4CSType;
    ciexyz_tripple_s bV4Endpoints;
    uint32_t        bV4GammaRed;
    uint32_t        bV4GammaGreen;
    uint32_t        bV4GammaBlue;
    v4_info_header_s (bsw::input_stream_decorator_c& icd, uint32_t size)
      : bV3InfoHeader (icd, size)
    {
      icd >> bV4CSType >> bV4Endpoints 
	  >> bV4GammaRed >> bV4GammaGreen
	  >> bV4GammaBlue;
    }

    void operator () (bmp_info_s& bi)
    {
      bV3InfoHeader (bi);
    }
  };

  struct v5_info_header_s  
  {
    v4_info_header_s  bV4InfoHeader;
    uint32_t        bV5Intent;
    uint32_t        bV5ProfileData;
    uint32_t        bV5ProfileSize;
    uint32_t        bV5Reserved;

    v5_info_header_s (bsw::input_stream_decorator_c& icd, uint32_t size)
      : bV4InfoHeader (icd, size)
    {
      icd >> bV5Intent >> bV5ProfileData >> bV5ProfileSize >> bV5Reserved;
    }

    void operator () (bmp_info_s& bi)
    {
      bV4InfoHeader (bi);
    }
  };
  // --------------------------------------------------------------------------
  std::ostream& operator << (std::ostream& os, 
			     const bmp_kind_t& kind)
  {
    switch (kind)
      {
      case eBMP_BM:
	os << "Windows";
	break;
      case eBMP_BA:
	os << "OS/2 Bitmap Array";
	break;
      case eBMP_CI:
	os << "OS/2 Color Icon";
	break;
      case eBMP_CP:
	os << "OS/2 Color Pointer";
	break;
      case eBMP_IC:
	os << "OS/2 Icon";
	break;
      case eBMP_PT:
	os << "OS/2 Pointer";
	break;
      }
    return os;
  }
  // --------------------------------------------------------------------------
  std::ostream& operator << (std::ostream& os, 
			     const bmp_compression_method_t& bmp_compression_method)
  {
    switch (bmp_compression_method)
      {
      case eBMP_RGB:
	os << "RGB";
	break;
      case eBMP_RLE8:
	os << "RLE-8";
	break;
      case eBMP_RLE4:
	os << "RLE-4";
	break;
      case eBMP_BITFIELDS:
	os << "BITFIELDS";
	break;
      case eBMP_JPEG:
	os << "JPG";
	break;
      case eBMP_PNG:
	os << "PNG";
	break;
      }
    return os;
  }
  // --------------------------------------------------------------------------
  std::ostream& operator << (std::ostream& os, 
			     const bmp_info_s& bmp_info)
  {
    os << "kind:        " << bmp_info.kind           << std::endl
       << "width:       " << bmp_info.width          << std::endl
       << "height:      " << bmp_info.height         << std::endl
       << "depth:       " << bmp_info.bpp            << std::endl
       << "offset:      " << bmp_info.offset_to_data << std::endl
       << "colors:      " << bmp_info.colors_in_palette << std::endl
       << "compression: " << bmp_info.compression_method;
    return os;
  }
  // --------------------------------------------------------------------------
  static
  inline
  bool test_magic (const bmpfile_header_s& hdr, const char* dt)
  {
    return (hdr.magic [0] == dt [0] && hdr.magic [1] == dt [1]);
  } 
  // --------------------------------------------------------------------------
  void load_bmp_info (bsw::input_stream_c& inp, bmp_info_s& bi)
  {
    bsw::input_stream_decorator_c isd (inp, true);
    bmpfile_header_s fh (isd);
    if (test_magic (fh, "BM"))
      {
	bi.kind = eBMP_BM;
      }
    else
      {
	if (test_magic (fh, "BA"))
	  {
	    bi.kind = eBMP_BA;
	  }
	else
	  {
	    if (test_magic (fh, "CI"))
	      {
		bi.kind = eBMP_CI;
	      }
	    else
	      {
		if (test_magic (fh, "CP"))
		  {
		    bi.kind = eBMP_CP;
		  }
		else
		  {
		    if (test_magic (fh, "IC"))
		      {
			bi.kind = eBMP_IC;
		      }
		    else
		      {
			if (test_magic (fh, "PT"))
			  {
			    bi.kind = eBMP_PT;
			  }
			else
			  {
			    std::ostringstream os;
			    os << "Unknown BMP header [ $" << std::hex 
			       << ((int)fh.magic [0] & 0xFF)
			       << "$"
			       << std::hex
			       << ((int)fh.magic [1] & 0xFF)
			       << std::dec
			       << "] of input stream " << isd.name ();
			    throw std::runtime_error (os.str ());
			  }
		      }
		  }
	      }
	  }
      }
    bi.offset_to_data = fh.bmp_offset;
    uint32_t sz;
    isd >> sz;
    switch (sz)
      {
      case 12:
	{
	  os2_info_header_s ih (isd, sz);
	  ih (bi);
	}
	break;
      case 64:
	{
	  core_info_header_s ih (isd, sz);
	  ih (bi);
	}
	break;
      case 40:
	{
	  core_info_header_s ih (isd, sz);
	  ih (bi);
	}
	break;
      case 52:
	{
	  v2_info_header_s ih (isd, sz);
	  ih (bi);
	}
	break;
      case 56:
	{
	  v3_info_header_s ih (isd, sz);
	  ih (bi);
	}
	break;
      case 108:
	{
	  v4_info_header_s ih (isd, sz);
	  ih (bi);
	}
      case 124:
	{
	  v5_info_header_s ih (isd, sz);
	  ih (bi);
	}
      }
  }
  // --------------------------------------------------------------------------
  template <int BITS>
  void uncompressed_loader (bsw::input_stream_decorator_c& isd, abstract_picture_c* img, const bmp_info_s& bi)
  {
    simple_pixels_reader_c <BITS> rdr (isd, bi.width);
    for (unsigned int y=0; y<bi.height; y++)
      {
	rdr.load_row ();
	unsigned int inv_y = bi.top_down ? bi.height - 1 - y : y;

	for (unsigned int x=0; x<bi.width; x++)
	  {
	    typename simple_pixels_reader_c <BITS>::pixel_t pixel;
	    rdr.next_pixel (pixel);
	    img->put_pixel (x, inv_y, pixel);
	  }
      }
  }
  // --------------------------------------------------------------------------
  abstract_picture_c* load_bmp (bsw::input_stream_c& inp, allocator_c* allocator, bmp_info_s& bi)
  {
    bsw::input_stream_decorator_c isd (inp, true);

    const bsw::file_size_t stream_start = isd.tell ();
    load_bmp_info (inp, bi);

    bsw::file_size_t curr_pos = isd.tell ();
    if (bi.offset_to_data < curr_pos)
      {
	std::ostringstream os;
	os << "BMP file " << isd.name () << " is corrupted" << std::endl;
	throw std::runtime_error (os.str ());
      }
    
    unsigned int num_colors = (unsigned int)(bi.offset_to_data - curr_pos) / bi.rgb_length;
    if (num_colors > bi.colors_in_palette)
      {
	std::ostringstream os;
	os << "BMP file " << isd.name () << " is corrupted. Number of colors mismatch." 
	   << "estimated number of colors " << num_colors << ", desired number of colors " << bi.colors_in_palette;
	throw std::runtime_error (os.str ());
      }
    
    bits_per_pixel_t bpp;
    
    switch (bi.bpp)
      {
      case eBPP1:
      case eBPP2:
      case eBPP4:
      case eBPP8:
	bpp = eBPP8;
	break;

      case eBPP16:
      case eBPP24:
      case eBPP32:
	bpp = eBPP32;
	break;
      }

    abstract_picture_c* img = allocator->create (bi.width, bi.height, bpp);
    
    rgba_vector_t pal;

    for (unsigned int i = 0; i<bi.colors_in_palette; i++)
      {
	uint8_t rgba [4];
	isd.read ( (char*) rgba, bi.rgb_length);
	rgba_s color;
	color.b = rgba [0] & 0xFF;
	color.g = rgba [1] & 0xFF;
	color.r = rgba [2] & 0xFF;
	pal.push_back (color);
      }
    img->set_palette (pal);

    isd.seek (stream_start + bi.offset_to_data);

    switch (bi.bpp)
      {
      case eBPP1:
	uncompressed_loader <1> (isd, img, bi);
	break;
      case eBPP2:
	uncompressed_loader <2> (isd, img, bi);
	break;
      case eBPP4:
	uncompressed_loader <4> (isd, img, bi);
	break;
      case eBPP8:
	uncompressed_loader <8> (isd, img, bi);
	break;
      }
    return img;
  }
} // ns bmp
