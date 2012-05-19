#include "thirdparty/predef.h"
#if !defined(PREDEF_OS_WINDOWS)
#include <string.h>
#include <errno.h>
#include <stdlib.h>

static char* str_error_r_safe (int err_code)
{
  char *s;
  size_t size;

  size = 1024;
  s = (char*)malloc(size);
  if (s == NULL)
    {
      return NULL;
    }
  strerror_r (err_code, s, size);
  return s;
}
#endif

#include "bsw/err/err.hpp"
#if defined(PREDEF_OS_WINDOWS)
#include <windows.h>
#else

#endif

#include <sstream>

namespace bsw
{
#if defined(PREDEF_OS_WINDOWS)
  std::string error_to_string (err_code_t err_code)
  {
    LPVOID lpMsgBuf;
    FormatMessage(
		  FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		  FORMAT_MESSAGE_FROM_SYSTEM |
		  FORMAT_MESSAGE_IGNORE_INSERTS,
		  NULL,
		  err_code,
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lpMsgBuf,
		  0, NULL );
    std::string res;
    res.assign ((LPCTSTR)lpMsgBuf);
    return res;
  }
  // =============================================================================
  void exit_with_msg_box (const char* bad_func, err_code_t err_cod)
  {
      std::ostringstream os;
      os << bad_func << " exited with: " << err_cod << ", " << error_to_string (err_cod);
      MessageBox(NULL, (LPCTSTR)os.str ().c_str(), TEXT("Error"), MB_OK); 
      ExitProcess (err_cod); 
  }
#else
  // =============================================================================
  std::string error_to_string (err_code_t err_code)
  {
    char* s = str_error_r_safe (err_code);
    std::string res;
    if (s)
      {
	res.assign (s);
	free (s);
      }
    return res;
  }
#endif

} // ns bsw
