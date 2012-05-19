#ifndef __PLATFORM_PROPER_EXPORT_H__
#define __PLATFORM_PROPER_EXPORT_H__

# if defined(__BEOS__) || defined(__HAIKU__)
#  if defined(__GNUC__)
#   define PROPER_EXPORT __declspec(dllexport)
#  else
#   define PROPER_EXPORT __declspec(export)
#  endif
# elif defined(__WIN32__) || defined(_WIN32) || defined (WIN32) || defined (WIN64) || defined (_WIN64)
#  ifdef __BORLANDC__
#    define PROPER_EXPORT
#    define PROPER_IMPORT     __declspec(dllimport)
#  else
#   define PROPER_EXPORT      __declspec(dllexport)
#   define PROPER_IMPORT      __declspec(dllimport)
#  endif
# elif defined(__OS2__)
#  ifdef __WATCOMC__
#    define PROPER_EXPORT     __declspec(dllexport)
#    define PROPER_IMPORT
#  elif defined (__GNUC__) && __GNUC__ < 4
#    define PROPER_EXPORT    __declspec(dllexport)
#    define PROPER_IMPORT
#  else
#   define PROPER_EXPORT
#   define PROPER_IMPORT
#  endif
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define PROPER_EXPORT      __attribute__ ((visibility("default")))
#   define PROPER_IMPORT
#  else
#   define PROPER_EXPORT
#   define PROPER_IMPORT
#  endif
# endif

# if defined(__WIN32__) && !defined(__GNUC__)
#  define CALLSPEC __cdecl
# elif defined(__OS2__)
#  if defined (__GNUC__) && __GNUC__ < 4
#   define CALLSPEC _cdecl
#  else
#   define CALLSPEC _System
#  endif
# else
#  define CALLSPEC
# endif

#ifdef __SYMBIAN32__
#ifndef EKA2
#undef PROPER_IMPORT
#define PROPER_IMPORT
#undef PROPER_EXPORT
#define PROPER_EXPORT
#elif !defined(__WINS__)
#undef PROPER_EXPORT
#define PROPER_EXPORT __declspec(dllexport)
#endif /* !EKA2 */
#endif /* __SYMBIAN32__ */



#if !defined(PROPER_EXPORT)
#error "PROPER_EXPORT is undefined"
#endif

#if !defined(PROPER_IMPORT)
#error "PROPER_EXPORT is undefined"
#endif

#if !defined(CALLSPEC)
#error "CALLSPEC is undefined"
#endif


#endif

