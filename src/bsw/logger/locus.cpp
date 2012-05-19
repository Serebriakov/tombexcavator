#include "bsw/logger/locus.hpp"

namespace bsw
{
  namespace logger
  {
    locus_c::locus_c (const char* module_name,
		      const char* source,
		      const char* function,
		      unsigned int line)
      : m_module_name (module_name),
	m_src_file    (source),
	m_function    (function),
	m_line        (line)
    {
    }
    // ----------------------------------------------------------
    const char* locus_c::module () const
    {
      return m_module_name;
    }
    // ----------------------------------------------------------
    const char* locus_c::source () const
    {
      return m_src_file;
    }
    // ----------------------------------------------------------
    const char* locus_c::function () const
    {
      return m_function;
    }
    // ----------------------------------------------------------
    unsigned int locus_c::line () const
    {
      return m_line;
    }
  } // ns logger
} // ns bsw

