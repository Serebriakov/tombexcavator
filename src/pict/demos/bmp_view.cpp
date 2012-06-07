#include <iostream>
#include <stdexcept>
#include <sstream>

#include <SDL.h>

#include "pict/bmp/bmp.hpp"
#include "pict/pcx/pcx.hpp"
#include "pict/allocator/sdl_allocator.hpp"
#include "bsw/fs/file.hpp"

void view_sdl (SDL_Surface* image, unsigned int width, unsigned int height)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
      std::ostringstream os;
      os << "Unable to initialize SDL: " << SDL_GetError();
      throw std::runtime_error (os.str ());
    }
  
  SDL_Surface* screen = SDL_SetVideoMode(width, 
					 height, 
					 image->format->BitsPerPixel, 
					 SDL_DOUBLEBUF);
  if (screen == NULL) 
    {
      std::ostringstream os;
      os << "Unable to set video mode: " << SDL_GetError();
      throw std::runtime_error (os.str ());
    }
  bool key_hit = false;
  SDL_Rect src, dst;
  src.x = 0;
  src.y = 0;
  src.w = width;
  src.h = height;

  dst.x = 0;
  dst.y = 0;
  dst.w = width;
  dst.h = height;

  while (!key_hit)
    {
      if(SDL_MUSTLOCK(screen)) 
	{
	  if(SDL_LockSurface (screen) < 0) 
	    {
	      return;
	    }
	}
      
      if (image->format->BitsPerPixel <= 8)
	{
	  // set palette if needed
	  SDL_SetPalette (screen, 
			  SDL_LOGPAL|SDL_PHYSPAL, 
			  image->format->palette->colors, 
			  0, 
			  image->format->palette->ncolors);
	}
      SDL_BlitSurface (image, &src, screen, &dst);

      if(SDL_MUSTLOCK (screen)) 
	{
	  SDL_UnlockSurface (screen);
	}
  
      SDL_Flip (screen); 
      
      SDL_Event event;
      while(SDL_PollEvent(&event)) 
	{      
	  switch (event.type) 
	    {
	    case SDL_QUIT:
	      key_hit = true;
	      break;
	    case SDL_KEYDOWN:
	      key_hit = true;
	      break;
	    }
	}
    }
  SDL_Quit ();
}

int main (int argc, char* argv [])
{
  if (argc != 2)
    {
      std::cerr << "USAGE " << argv [0] << " <file.bmp> " << std::endl;
      return 1;
    }
  try
    {
      bsw::input_file_c ifs (argv [1]);
      
      pict::sdl_allocator_c allocator;
      /*
      pict::bmp_info_s bi;
      pict::abstract_picture_c* bmp = load_bmp (ifs, &allocator, bi);
      std::cout << bi << std::endl;
      */
      pict::pcx_info_s bi;
     
      pict::abstract_picture_c* bmp = load_pcx (ifs, &allocator, bi);
      std::cout << bi << std::endl;

      view_sdl ((SDL_Surface*)bmp->opaque (), bi.width, bi.height);
      
      allocator.dispose (bmp);
    }
  catch (std::exception& e)
    {
      std::cerr << "Error: " << e.what () << std::endl;
      return 1;
    }
  return 0;
}
