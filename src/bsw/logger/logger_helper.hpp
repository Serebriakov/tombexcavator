#ifndef __BSW_LOGGER_HELPER_HPP__
#define __BSW_LOGGER_HELPER_HPP__

#include "bsw/logger/record.hpp"
#include "bsw/logger/multi_setter.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw
{
  namespace logger
  {
    class record_c;

    BSW_API void post_record (record_c* record);
  } // ns logger
} // ns bsw

#define EV_LOGGER_TRACE(SEVERITY, MODULE, FUNCTION, FILE, LINE, XARGS)	\
  do {									\
    using namespace bsw::logger;					\
    locus_c locus (MODULE, FILE, FUNCTION, LINE);			\
    record_c* rec = new record_c (SEVERITY, locus);			\
    priv::multi_setter_c ms (*rec);					\
    ms.set XARGS ;							\
    post_record (rec);							\
  } while (0)

#if !defined(BSW_TRACE_LEVEL)
#   if !(defined NDEBUG)
#      define BSW_TRACE_LEVEL 6
#   else
#      define BSW_TRACE_LEVEL 5
#   endif
#endif // BSW_TRACE_LEVEL

#if BSW_TRACE_LEVEL > 1
#define EV_LOGGER_TRACE_EVLOG_FATAL(MODULE, FUNCTION, FILE, LINE, XARGS)	\
  EV_LOGGER_TRACE (bsw::logger::eFATAL, MODULE, FUNCTION, FILE, LINE, XARGS)
#else
#define EV_LOGGER_TRACE_EVLOG_FATAL(MODULE, FUNCTION, FILE, LINE, XARGS)            ((void)0)
#endif

#if BSW_TRACE_LEVEL > 2
#define EV_LOGGER_TRACE_EVLOG_ERROR(MODULE, FUNCTION, FILE, LINE, XARGS)	\
  EV_LOGGER_TRACE (bsw::logger::eERROR, MODULE, FUNCTION, FILE, LINE,       XARGS)
#else
#define EV_LOGGER_TRACE_EVLOG_ERROR(MODULE, FUNCTION, FILE, LINE, XARGS)            ((void)0)
#endif

#if BSW_TRACE_LEVEL > 3
#define EV_LOGGER_TRACE_EVLOG_WARNING(MODULE, FUNCTION, FILE, LINE, XARGS) \
  EV_LOGGER_TRACE (bsw::logger::eWARNING, MODULE, FUNCTION, FILE, LINE, XARGS)
#else
#define EV_LOGGER_TRACE_EVLOG_WARNING(MODULE, FUNCTION, FILE, LINE, XARGS)            ((void)0)
#endif

#if BSW_TRACE_LEVEL > 4
#define EV_LOGGER_TRACE_EVLOG_INFO(MODULE, FUNCTION, FILE, LINE, XARGS)	\
  EV_LOGGER_TRACE (bsw::logger::eINFO, MODULE, FUNCTION, FILE, LINE,       XARGS)
#else
#define EV_LOGGER_TRACE_EVLOG_INFO(MODULE, FUNCTION, FILE, LINE, XARGS)                 ((void)0)
#endif

#if BSW_TRACE_LEVEL > 5
#define EV_LOGGER_TRACE_EVLOG_DEBUG(MODULE, FUNCTION, FILE, LINE, XARGS)	\
  EV_LOGGER_TRACE (bsw::logger::eDEBUG, MODULE, FUNCTION, FILE, LINE, XARGS)
#else
#define EV_LOGGER_TRACE_EVLOG_DEBUG(MODULE, FUNCTION, FILE, LINE, XARGS)            ((void)0)
#endif

#endif
