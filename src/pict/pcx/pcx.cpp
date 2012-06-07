#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "pict/pcx/pcx.hpp"
#include "pict/bpp.hpp"
#include "bsw/io/input_stream_decorator.hpp"
#include "pict/abstract_picture.hpp"
#include "pict/allocator.hpp"

namespace pict
{
    std::ostream& operator << (std::ostream& os, pcx_vendor_t vendor)
    {
        switch (vendor)
        {
        case eVENDOR_2_5:
            os << "Version 2.5 of PC Paintbrush";
            break;
        case eVENDOR_2_8_PAL:
            os << "Version 2.8 w/palette information";
            break;
        case eVENDOR_2_8_WO_PAL:
            os << "Version 2.8 wo/palette information";
            break;
        case eVENDOR_4:
            os << "PC Paintbrush for Windows";
            break;
        case eVENDOR_5:
            os << "Publisher's Paintbrush";
            break;
        default:
            os << "Unknown";
            break;
        }
        return os;
    }
    // ------------------------------------------------------------------------------
    std::ostream& operator << (std::ostream& os, const pcx_info_s& pcx_info)
    {
        os << "Vendor: " << pcx_info.vendor << std::endl
           << "Width:  " << pcx_info.width << std::endl
           << "Height: " << pcx_info.height << std::endl
           << "BPP:    " << pcx_info.bpp << std::endl
           << "Planes: " << pcx_info.num_planes;
        return os;
    }
    // ------------------------------------------------------------------------------
    void load_pcx_info (bsw::input_stream_c& inp, pcx_info_s& pcx_info)
    {
        const uint8_t PCX_MAGIC = 0x0A;

        bsw::input_stream_decorator_c isd (inp, true);
        uint8_t pcx_id;
        isd >> pcx_id;
        if (pcx_id != PCX_MAGIC)
        {
            std::ostringstream os;
            os << "File " << isd.name () << " is not a PCX";
            throw std::runtime_error (os.str ());
        }
        uint8_t pcx_version;
        isd >> pcx_version;
        switch (pcx_version)
        {
        case 0:
            pcx_info.vendor = eVENDOR_2_5;
            break;
        case 2:
            pcx_info.vendor = eVENDOR_2_8_PAL;
            break;
        case 3:
            pcx_info.vendor = eVENDOR_2_8_WO_PAL;
            break;
        case 4:
            pcx_info.vendor = eVENDOR_4;
            break;
        case 5:
            pcx_info.vendor = eVENDOR_5;
            break;
        default:
            {
                const int vnd = (int)pcx_version & 0xFF;
                std::ostringstream os;
                os << "Unknown vendor [" << vnd << "] of PCX file " << isd.name ();
                throw std::runtime_error (os.str ());
            }
        }
        uint8_t pcx_encoding;
        isd >> pcx_encoding;
        if (pcx_encoding != 1)
        {
            std::ostringstream os;
            const int enc = (int)pcx_encoding & 0xFF;
            os << "Unknown encoding [" << enc << "] of PCX file " << isd.name ();
        }
        uint8_t pcx_bpp;
        int16_t	XStart;            /* Left of image */
        int16_t	YStart;            /* Top of Image */
        int16_t	XEnd;              /* Right of Image */
        int16_t	YEnd;              /* Bottom of image */
        int16_t	HorzRes;           /* Horizontal Resolution */
        int16_t	VertRes;           /* Vertical Resolution */
        uint8_t	Palette[48];       /* 16-Color EGA Palette */
        uint8_t	Reserved1;         /* Reserved (Always 0) */
        uint8_t	NumBitPlanes;      /* Number of Bit Planes */
        int16_t	BytesPerLine;      /* Bytes per Scan-line */
        int16_t	PaletteType;       /* Palette Type */
        int16_t	HorzScreenSize;    /* Horizontal Screen Size */
        int16_t	VertScreenSize;    /* Vertical Screen Size */
        
        isd >> pcx_bpp >> XStart >> YStart >> XEnd >> YEnd
            >> HorzRes >> VertRes;
        isd.read ((char*)Palette, sizeof (Palette));
        isd >> Reserved1 >> NumBitPlanes >> BytesPerLine 
            >> PaletteType >> HorzScreenSize >> VertScreenSize;
        pcx_info.width = 1 + XEnd - XStart;
        pcx_info.height = 1 + YEnd - YStart;
        if (PaletteType == 2)
        {
            pcx_info.greyscale = true;
        }
        else
        {
            pcx_info.greyscale = false;
        }
        pcx_info.num_planes = NumBitPlanes;
        pcx_info.bytes_per_line = pcx_info.num_planes*BytesPerLine;
        if (NumBitPlanes == 3)
        {
            pcx_info.bpp = eBPP24;
        }
        else
        {
            if (NumBitPlanes != 1 && NumBitPlanes != 4)
            {
                const int nbp = (int)NumBitPlanes & 0xFF;
                std::ostringstream os;
                os << "Bad number of bit planes [" << nbp << "] for PCX file " << isd.name ();
                throw std::runtime_error (os.str ());
            }
            switch (pcx_bpp)
            {
            case 1:
                switch (NumBitPlanes)
                {
                case 1:
                    pcx_info.bpp = eBPP1;
                    break;
                case 2:
                    pcx_info.bpp = eBPP2;
                    break;
                case 3:
                case 4:
                    pcx_info.bpp = eBPP4;
                    break;
                default:
                {
                    const int ibpp = (int)pcx_bpp & 0xFF;
                    const int inum = (int)NumBitPlanes & 0xFF;
                    std::ostringstream os;
                    os << "Bad BPP [" << ibpp << "/" << inum << "] for PCX file " << isd.name ();
                    throw std::runtime_error (os.str ());
                }
                };
                break;
            case  2:
                pcx_info.bpp = eBPP2;
                break;
            case 4:
                pcx_info.bpp = eBPP4;
                break;
            case 8:
                pcx_info.bpp = eBPP8;
                break;
            default:
                {
                    const int ibpp = (int)pcx_bpp & 0xFF;
                    std::ostringstream os;
                    os << "Bad BPP [" << ibpp << "] for PCX file " << isd.name ();
                    throw std::runtime_error (os.str ());
                }
            }
        }
    }
    // ---------------------------------------------------------------------------
    static void load_palette (bsw::input_stream_decorator_c& isd, 
                              std::vector <rgba_s>& palette,
                              bool is_16_colors)
    {
       const unsigned int to_read = is_16_colors ? 16 : 256;
       palette.reserve (to_read);

       for (unsigned int i=0; i<to_read; i++)
       {
          uint8_t r,g,b;
          isd >> r >> g >> b;

          rgba_s rgb;
          rgb.r = (int)r & 0xFF;
          rgb.g = (int)g & 0xFF;
          rgb.b = (int)b & 0xFF;

          palette.push_back (rgb);
       }
    }
    // ---------------------------------------------------------------------------
    static void load_scan_line (bsw::input_stream_decorator_c& isd,
                                std::vector <uint8_t>& line)
    {
        const std::size_t sz = line.size ();
        bool end_of_line = false;
        std::size_t x = 0;
        while (x < sz)
        {
            uint8_t run_len = 1;
            uint8_t code;
            uint8_t val;
            isd >> code;
            if (0xC0 == (0xC0 & code))
            {
                run_len = 0x3F & code;
                isd >> val;
            }
            else
            {
                val = code;
            }
            for (uint8_t i=0; i < run_len; i++, x++)
            {
                if (x < sz)
                {
                    line [x] = val;
                }
                else
                {
                    end_of_line = true;
                    break;
                }
            }
        }
    }
    // ---------------------------------------------------------------------------
    static
    void put_pixels_24 (abstract_picture_c* img, const std::vector <uint8_t>& scan_line, 
                        unsigned int y, const pcx_info_s& pcx_info)
    {
        for (unsigned int x=0; x<pcx_info.width; x++)
        {
            rgba_s rgb;
            rgb.r = scan_line [0*pcx_info.width + x];
            rgb.g = scan_line [1*pcx_info.width + x];
            rgb.b = scan_line [2*pcx_info.width + x];
            img->put_pixel (x, y, rgb);
        }
    }
    // ---------------------------------------------------------------------------
    static
    void put_pixels_8 (abstract_picture_c* img, const std::vector <uint8_t>& scan_line, 
                        unsigned int y, const pcx_info_s& pcx_info)
    {
        for (unsigned int x=0; x<pcx_info.width; x++)
        {
            img->put_pixel (x, y, scan_line [x]);
        }
    }
    // ---------------------------------------------------------------------------
    static
    void put_pixels_by_bits (abstract_picture_c* img, const std::vector <uint8_t>& scan_line, 
                             unsigned int y, const pcx_info_s& pcx_info,
                             std::vector <uint8_t>& row)
    {
        const std::size_t bytes_in_line = pcx_info.bytes_per_line / pcx_info.num_planes;
        unsigned int k = 0;
        for (unsigned int plane=0; plane < pcx_info.num_planes; plane++)
        {
            unsigned int x = 0;
            for (std::size_t i=0; i<bytes_in_line; i++)
            {
                uint8_t byte = scan_line [k++];
                for(int j = 7; j >= 0; j--) 
                {
						uint8_t bit = (byte >> j) & 1;
						/* skip padding bits */
						if (i * 8 + j >= pcx_info.width)
                        {
							continue;
                        }
                        uint8_t z = (bit << plane);
						row[x++] |= z;
				} 
            }
        }
        for (unsigned int x = 0; x < pcx_info.width; x++)
        {
            img->put_pixel (x, y, row [x]);
            row [x] = 0;
        }
    }
    // ---------------------------------------------------------------------------
    abstract_picture_c* load_pcx (bsw::input_stream_c& inp, allocator_c* allocator, pcx_info_s& pcx_info)
    {
       bsw::input_stream_decorator_c isd (inp, true);
       const bsw::file_size_t stream_start = isd.tell ();
       load_pcx_info (inp, pcx_info);
       std::vector <uint8_t> row;
       enum
       {
           eNO_PALETTE,
           eBUILT_IN_PALETTE,
           eAT_END_PALETTE
       } pal_status;
       bits_per_pixel_t image_bpp;
       switch (pcx_info.bpp)
       {
       case eBPP1:
       case eBPP2:
       case eBPP4:
           pal_status = eBUILT_IN_PALETTE;
           image_bpp = eBPP8;
           row.resize (pcx_info.width, 0);
           break;
       case eBPP8:
           pal_status = eAT_END_PALETTE;
           image_bpp = eBPP8;
           break;
       default:
           pal_status = eNO_PALETTE;
           image_bpp = eBPP24;
       }

       std::vector <rgba_s> palette;
       if (pal_status == eBUILT_IN_PALETTE)
       {
           isd.seek (stream_start + (bsw::file_size_t)16);
           load_palette (isd, palette, true);
       }
       else
       {
           if (pal_status == eAT_END_PALETTE)
           {
               const bsw::file_size_t fsize = isd.file_size ();
               if (fsize < 769 + 128)
               {
                   std::ostringstream os;
                   os << "Bad  size [" <<  fsize << "] of the PCX file " << isd.name ();
                   throw std::runtime_error (os.str ());
               }
               isd.seek (fsize - (bsw::file_size_t)769);
               uint8_t pcx_pal_magic;
               isd >> pcx_pal_magic;
               if (pcx_pal_magic != 12)
               {
                  std::ostringstream os;
                  os << "Unexpected start palette code in the PCX file " << isd.name ();
                  throw std::runtime_error (os.str ());
               }
               load_palette (isd, palette, false);
           }
       }

       isd.seek (128);
       
       abstract_picture_c* img = allocator->create (pcx_info.width, pcx_info.height, image_bpp);
       if (!palette.empty ())
       {
           img->set_palette (palette);
       }
       std::vector <uint8_t> scan_line (pcx_info.bytes_per_line);
       for (unsigned int y=0; y < pcx_info.height; y++)
       {
           load_scan_line (isd, scan_line);
           if (pcx_info.bpp == eBPP24)
            {
                put_pixels_24 (img, scan_line, y, pcx_info);               
            }
           else
           {
               if (pcx_info.bpp == eBPP8)
               {
                   put_pixels_8 (img, scan_line, y, pcx_info); 
               }
               else
               {
                   put_pixels_by_bits (img, scan_line, y, pcx_info, row);
               }
       
           }
       }
       return img;
    }
} // ns pict

