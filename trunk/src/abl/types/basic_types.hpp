/**
 * @file   basic_types.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Thu Dec 24 19:31:22 2009
 * 
 * @brief  Definitions of fixed-size integer types for various platforms
 * 
 */

#ifndef __ABL_BASIC_TYPES_HPP__
#define __ABL_BASIC_TYPES_HPP__

#if !defined(__INCLUDE_BASIC_TYPES__)
#error "BugBug"
#endif

#if defined(_MSC_VER)
	/*
	* Windows/Visual C++
	*/
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed __int64         Int64;
	typedef unsigned __int64       UInt64;
	#if defined(_WIN64)
		#define ABL_PTR_IS_64_BIT 1
		typedef signed __int64     IntPtr;
		typedef unsigned __int64   UIntPtr;
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
	#endif
	#define ABL_HAVE_INT64 1
#elif defined(__GNUC__)
	/*
	* Unix/GCC
	*/
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__LP64__)
		#define ABL_PTR_IS_64_BIT 1
		#define ABL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define ABL_HAVE_INT64 1
#elif defined(__DECCXX)
	/*
	* Compaq C++
	*/
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed __int64         Int64;
	typedef unsigned __int64       UInt64;
	#if defined(__VMS)
		#if defined(__32BITS)
			typedef signed long    IntPtr;
			typedef unsigned long  UIntPtr;
		#else
			typedef Int64          IntPtr;
			typedef UInt64         UIntPtr;
			#define ABL_PTR_IS_64_BIT 1
		#endif
	#else
		typedef signed long        IntPtr;
		typedef unsigned long      UIntPtr;
		#define ABL_PTR_IS_64_BIT 1
		#define ABL_LONG_IS_64_BIT 1
	#endif
	#define ABL_HAVE_INT64 1
#elif defined(__HP_aCC)
	/*
	* HP Ansi C++
	*/
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__LP64__)
		#define ABL_PTR_IS_64_BIT 1
		#define ABL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define ABL_HAVE_INT64 1
#elif defined(__SUNPRO_CC)
	/*
	* SUN Forte C++
	*/
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__sparcv9)
		#define ABL_PTR_IS_64_BIT 1
		#define ABL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define ABL_HAVE_INT64 1
#elif defined(__IBMCPP__) 
	/*
	* IBM XL C++
	*/
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if defined(__64BIT__)
		#define ABL_PTR_IS_64_BIT 1
		#define ABL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define ABL_HAVE_INT64 1
#elif defined(__sgi) 
	/*
	* MIPSpro C++
	*/
	typedef signed char            Int8;
	typedef unsigned char          UInt8;
	typedef signed short           Int16;
	typedef unsigned short         UInt16;
	typedef signed int             Int32;
	typedef unsigned int           UInt32;
	typedef signed long            IntPtr;
	typedef unsigned long          UIntPtr;
	#if _MIPS_SZLONG == 64
		#define ABL_PTR_IS_64_BIT 1
		#define ABL_LONG_IS_64_BIT 1
		typedef signed long        Int64;
		typedef unsigned long      UInt64;
	#else
		typedef signed long long   Int64;
		typedef unsigned long long UInt64;
	#endif
	#define ABL_HAVE_INT64 1
#endif




#endif 
