#ifndef __BSW_LOGGER_FUNCTION_NAME_HPP__
#define __BSW_LOGGER_FUNCTION_NAME_HPP__

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC) 
// works only with -features=extensions compiler flag
#define BSW_FUNCTION_NAME __func__
#define BSW_PRETTY_FUNCTION_NAME BSW_FUNCTION_NAME
#elif defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600))
# define BSW_FUNCTION_NAME __FUNCTION__
# define BSW_PRETTY_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER) && (_MSC_VER >= 1020)
# define BSW_FUNCTION_NAME __FUNCTION__
# define BSW_PRETTY_FUNCTION_NAME __FUNCDNAME__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
# define BSW_FUNCTION_NAME __FUNCTION__
# define BSW_PRETTY_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
# define BSW_FUNCTION_NAME __FUNCSIG__
# define BSW_PRETTY_FUNCTION_NAME BSW_FUNCTION_NAME
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
# define BSW_FUNCTION_NAME __FUNCTION__
# define BSW_PRETTY_FUNCTION_NAME BSW_FUNCTION_NAME
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
# define BSW_FUNCTION_NAME __FUNC__
# define BSW_PRETTY_FUNCTION_NAME BSW_FUNCTION_NAME
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
# define BSW_FUNCTION_NAME __func__
# define BSW_PRETTY_FUNCTION_NAME BSW_FUNCTION_NAME
#else
# define BSW_FUNCTION_NAME "(unknown function)"
# define BSW_PRETTY_FUNCTION_NAME BSW_FUNCTION_NAME
#endif

#endif
