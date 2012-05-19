#ifndef __BSW_MACROS_HPP__
#define __BSW_MACROS_HPP__

#ifdef CONCATENATE_DIRECT
#undef CONCATENATE_DIRECT
#endif

#ifdef CONCATENATE
#undef CONCATENATE
#endif

#ifdef ANONYMOUS_VAR
#undef ANONYMOUS_VAR
#endif

#ifdef STRINGIZE
#undef STRINGIZE
#endif

#ifdef STRINGIZE_HELPER
#undef STRINGIZE_HELPER
#endif

#ifdef ANONYMOUS_VAR
#undef ANONYMOUS_VAR
#endif

#define CONCATENATE_DIRECT(s1,s2) s1##s2
#define CONCATENATE(s1,s2) CONCATENATE_DIRECT(s1,s2)

#define STRINGIZE_HELPER(exp) #exp
#define STRINGIZE(exp) STRINGIZE_HELPER(exp)

#define ANONYMOUS_VAR(str) CONCATENATE(str, __LINE__)

#endif
