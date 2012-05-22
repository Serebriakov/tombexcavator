#include <iostream>
#include "westwood/fat.hpp"
#include "bsw/fs/file.hpp"

int main (int argc, char* argv [])
{
  if (argc != 2)
    {
      std::cerr << "USAGE: " << argv [0] << " <westwood pak file>" << std::endl;
      return 1;
    }
  try
    {
      bsw::input_file_c inp (argv [1]);
      westwood::fat_c fat (inp);
      for (std::size_t i=0; i<fat.size (); i++)
	{
	  std::cout << fat [i].size () << "   " << fat [i].name () << std::endl;
	}
    }
  catch (std::exception& e)
    {
      std::cerr << "ERROR: " << e.what () << std::endl;
      return 1;
    }
  return 0;
}
