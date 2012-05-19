#include "thirdparty/predef.h"

#if defined(_MSC_VER)
#include <windows.h>
#define WINDOWS_H_LOADED
#else
#include <sys/time.h>
#endif

#if !defined(HAVE_LOCALTIME_R)

#if defined(PREDEF_COMPILER_MINGW32) && !defined(WINDOWS_H_LOADED)
#include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>

#endif

#include "bsw/time/time.hpp"
#include "bsw/safe_cast.hpp"

namespace bsw
{
    time_s::time_s ()
        : seconds  (0),
        useconds (0)
    {
    }
    // -----------------------------------------------
    time_s get_time ()
    {
        time_s result;
#if defined(_MSC_VER)
        FILETIME CurrentTime;
        __int64 cur_time;

        GetSystemTimeAsFileTime(&CurrentTime);

        cur_time = ((__int64)CurrentTime.dwHighDateTime << 32) + (__int64)CurrentTime.dwLowDateTime;
        cur_time -= 116444736000000000LL;
        cur_time /= 10LL;

        result.seconds = SafeCast <long>(cur_time / 1000000LL);
        result.useconds = SafeCast <long>(cur_time % 1000000LL);
#else
        timeval tv;
        gettimeofday (&tv, 0);
        result.seconds = tv.tv_sec;
        result.useconds = tv.tv_usec;
#endif
        return result;
    }
} // ns bsw

#if !defined(HAVE_LOCALTIME_R)
#if defined (PREDEF_PLATFORM_WIN32)
static CRITICAL_SECTION localtime_r_cs;

static void localtime_r_atexit(void)
{
    DeleteCriticalSection(&localtime_r_cs);
}
struct tm * localtime_r(const time_t *timep, struct tm *result)
{
    static struct tm *local_tm;
    static int initialized = 0;
    if (!initialized) {
        ++initialized;
        InitializeCriticalSection(&localtime_r_cs);
        atexit(localtime_r_atexit);
    }

    EnterCriticalSection(&localtime_r_cs);
    local_tm = localtime(timep);
    memcpy(result, local_tm, sizeof(struct tm));
    LeaveCriticalSection(&localtime_r_cs);
    return result;
} 
#else
#error "locatime_r unimplemented on this platform"
#endif

#endif
