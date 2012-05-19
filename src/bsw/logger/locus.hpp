#ifndef __BSW_LOGGER_LOCUS_HPP__
#define __BSW_LOGGER_LOCUS_HPP__

#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    class BSW_API locus_c
    {
    public:
      locus_c (const char* module_name,
	       const char* source,
	       const char* function,
	       unsigned int line);

      const char* module () const;
      const char* source () const;
      const char* function () const;
      unsigned int line () const;

    private:
      const char* m_module_name;
      const char* m_src_file;
      const char* m_function;
      const unsigned int m_line;
    };
  } // ns logger
} // ns bsw


#endif
