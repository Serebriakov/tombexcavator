#ifndef __BSW_LOGGER_HPP__
#define __BSW_LOGGER_HPP__

#include "bsw/macros.hpp"
#include "bsw/logger/logger_helper.hpp"
#include "bsw/logger/function_name.hpp"

/*! 
 * Allows emitting a message of a specific severity.
 * - \c SEVERITY is one of a list of six severity levels, as follows:
 *   - EVLOG_FATAL (most severe)
 *   - EVLOG_CRITICAL
 *   - EVLOG_ERROR
 *   - EVLOG_WARNING
 *   - EVLOG_INFO
 *   - EVLOG_DEBUG (least severe)
 * - args is a list containing at least 1 and a maximum of 32 arguments.
 *
 * If the logger cannot convert the passed arguments into strings, a compilation error is issued.
 */

#if defined(ONYX_MODULE_NAME)
#define EVLOG_MODULE_NAME STRINGIZE(ONYX_MODULE_NAME)
#else
#define EVLOG_MODULE_NAME ""
#endif

#define EVLOG_TRACE(SEVERITY, XARGS)  CONCATENATE (EV_LOGGER_TRACE_,SEVERITY)  (EVLOG_MODULE_NAME, BSW_FUNCTION_NAME, __FILE__, __LINE__, XARGS)

#endif
