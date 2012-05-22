#include <iostream>
#include <stdexcept>

#include "pict/bmp/bmp.hpp"
#include "bsw/fs/file.hpp"

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
      pict::bmp_info_s bi;
      pict::load_bmp_info (ifs, bi);
      std::cout << bi << std::endl;
    }
  catch (std::exception& e)
    {
      std::cerr << "Error: " << e.what () << std::endl;
      return 1;
    }
  return 0;
}
