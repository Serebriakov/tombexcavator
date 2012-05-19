#ifndef __BSW_ERR_HPP__
#define __BSW_ERR_HPP__

#include <string>
#include "thirdparty/predef.h"
#include "bsw/bsw_api.hpp"

namespace bsw
{
  typedef int err_code_t;

  BSW_API std::string error_to_string (err_code_t err_code);

#if defined(PREDEF_OS_WINDOWS)
  BSW_API void exit_with_msg_box (const char* bad_func, err_code_t err_cod);
#endif

} // ns bsw

#endif
