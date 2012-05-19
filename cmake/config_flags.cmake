# ##################################################
# Compiler flags
# ##################################################


include (TestBigEndian)
include (FindOpenMP)

test_big_endian (ONYX_HAS_BIGENDIAN)

if (${ONYX_HAS_BIGENDIAN})
  message (STATUS "BigEndian platform detected")
else ()
  message (STATUS "LittleEndian platform detected")
endif ()



# --------------------------------------------------
# GCC flags
# --------------------------------------------------

if (UNIX)
  set (LFS_FLAGS "-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE")
else ()
endif ()

if (${ONYX_HAS_BIGENDIAN})
  set (ONYX_ENDIANITY_FLAG "-DONYX_BIG_ENDIAN")
endif ()

# --------------------------------------------------
# GCC flags
# --------------------------------------------------

# warning
set (GCC_COMMON_WARNINGS "-Wall -Wunknown-pragmas -Wformat-security -Wpacked  -Wparentheses  -Wpointer-arith -Wwrite-strings -Wundef -Wextra -Wconversion -Wno-long-long")

set (GCC_WARNINGS "${GCC_COMMON_WARNINGS} -Wmissing-prototypes")
set (GPP_WARNINGS "${GCC_COMMON_WARNINGS} -Wnon-virtual-dtor") 

set (GCC_NO_WARNINGS "-w")
set (GPP_NO_WARNINGS "${GCC_NO_WARNINGS}") 

#compiler flags
set (GCC_COMMON_FLAGS "${GCC_64_FLAG} -pipe -fstrict-aliasing -pedantic -fvisibility=hidden ${LFS_FLAGS} ${ONYX_ENDIANITY_FLAG}")
set (GCC_FLAGS "${GCC_COMMON_FLAGS}")
set (GPP_FLAGS "${GCC_COMMON_FLAGS} -fvisibility-inlines-hidden")

set (GCC_FLAGS_DEBUG "${GCC_FLAGS} -g")
set (GCC_FLAGS_RELEASE "${GCC_FLAGS} -O2")
set (GPP_FLAGS_DEBUG "${GPP_FLAGS} -g")
set (GPP_FLAGS_RELEASE "${GPP_FLAGS} -O2")

set (GCC_DEFINES_DEBUG "-D_DEBUG=1 -DONYX_DEBUG")
set (GCC_DEFINES_RELEASE "-DNDEBUG")


# --------------------------------------------------
# INTEL flags
# --------------------------------------------------

# warning
set (ICC_COMMON_WARNINGS "-Wall -Wpointer-arith -Wwrite-strings -Wcheck -Wp64  -Wconversion")

set (ICC_WARNINGS "${ICC_COMMON_WARNINGS} -Wmissing-prototypes")
set (IPP_WARNINGS "${ICC_COMMON_WARNINGS} -Wnon-virtual-dtor ") 

#compiler flags
set (ICC_COMMON_FLAGS "${ICC_64_FLAG} -pipe -fno-strict-aliasing -fvisibility=hidden ${LFS_FLAGS} ${ONYX_ENDIANITY_FLAG}")
set (ICC_FLAGS "${ICC_COMMON_FLAGS}")
set (IPP_FLAGS "${ICC_COMMON_FLAGS} -fvisibility-inlines-hidden")

set (ICC_FLAGS_DEBUG "${ICC_FLAGS} -g")
set (ICC_FLAGS_RELEASE "${ICC_FLAGS} -O2")
set (IPP_FLAGS_DEBUG "${IPP_FLAGS} -g")
set (IPP_FLAGS_RELEASE "${IPP_FLAGS} -O2")

set (ICC_DEFINES_DEBUG "-D_DEBUG=1 -DONYX_DEBUG")
set (ICC_DEFINES_RELEASE "-DNDEBUG")

# --------------------------------------------------
# MSVC flags
# --------------------------------------------------

# warning
set (MSVC_CC_COMMON_WARNINGS "/W3")

set (MSVC_CC_WARNINGS "${MSVC_CC_COMMON_WARNINGS}")
set (MSVC_CXX_WARNINGS "${MSVC_CC_COMMON_WARNINGS} ") 

set (MSVC_CC_NO_WARNINGS "/W0")
set (MSVC_CXX_NO_WARNINGS "${MSVC_CC_NO_WARNINGS} ") 

#compiler flags
set (MSVC_CC_COMMON_FLAGS "${MSVC_CC_64_FLAG} /nologo /EHsc /GR /Zc:wchar_t")
set (MSVC_CC_FLAGS "${MSVC_CC_COMMON_FLAGS}")
set (MSVC_CXX_FLAGS "${MSVC_CC_COMMON_FLAGS}")

set (MSVC_CC_FLAGS_DEBUG "${MSVC_CC_FLAGS} /Zi  /GR /Ob0 /Od /MDd")
set (MSVC_CC_FLAGS_RELEASE "${MSVC_CC_FLAGS} /O2 /MD")

set (MSVC_CXX_FLAGS_DEBUG "${MSVC_CXX_FLAGS} /Zi  /GR /Ob0 /Od /MDd")
set (MSVC_CXX_FLAGS_RELEASE "${MSVC_CXX_FLAGS} /O2 /MD")

set (MSVC_COMMON_DEFINES 
  "/DNOMINMAX ${MSVC_64_FLAG} /D_WINDOWS /D_CONSOLE /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECAT ${ONYX_ENDIANITY_FLAG}")

set (MSVC_CC_DEFINES_DEBUG "${MSVC_COMMON_DEFINES} /D_DEBUG=1 /DONYX_DEBUG")
set (MSVC_CC_DEFINES_RELEASE "${MSVC_COMMON_DEFINES} /DNDEBUG")

set (MSVC_CC_LINKER_FLAGS_DEBUG "/DEBUG")

# --------------------------------------------------
# SunCC flags
# --------------------------------------------------

set (SUN_CC_COMMON_WARNINGS "-errtags=yes ")

set (SUN_CC_WARNINGS "${SUN_CC_COMMON_WARNINGS} ")
set (SUN_CXX_WARNINGS "${SUN_CC_COMMON_WARNINGS} +w2 ")

set (SUN_CC_COMMON_FLAGS "-features=extensions -mt ${SUNCC_64_FLAG}")

set (SUN_CXX_FLAGS_DEBUG "${SUN_CC_COMMON_FLAGS} -g  -library=stlport4")
set (SUN_CXX_FLAGS_RELEASE "${SUN_CC_COMMON_FLAGS} -xtarget=native -fast -xarch=amd64  -library=stlport4")

set (SUN_CC_FLAGS_DEBUG "${SUN_CC_COMMON_FLAGS} -g ")
set (SUN_CC_FLAGS_RELEASE "${SUN_CC_COMMON_FLAGS} -xtarget=native -fast -xarch=amd64 ")

set (SUN_CC_DEFINES_DEBUG "-D_DEBUG=1 -DONYX_DEBUG -D_GNU_SOURCE")
set (SUN_CC_DEFINES_RELEASE "-DNDEBUG  -D_GNU_SOURCE")

set (SUN_CC_LINKER_FLAGS_DEBUG "-g -library=stlport4 -lpthread")
set (SUN_CC_LINKER_FLAGS_RELEASE "-g -library=stlport4 -lpthread")

# ======================================================================
macro (config_compile_and_linker_flags_name CXX_NAME CC_NAME)
  if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set (cxx_flags "${${CXX_NAME}_WARNINGS} ${${CXX_NAME}_FLAGS_DEBUG} ${${CC_NAME}_DEFINES_DEBUG}")
    set (cc_flags  "${${CC_NAME}_WARNINGS} ${${CC_NAME}_FLAGS_DEBUG} ${${CC_NAME}_DEFINES_DEBUG}")
	
    set (cxx_no_warn_flags "${${CXX_NAME}_NO_WARNINGS} ${${CXX_NAME}_FLAGS_DEBUG} ${${CC_NAME}_DEFINES_DEBUG}")
    set (cc_no_warn_flags  "${${CC_NAME}_NO_WARNINGS} ${${CC_NAME}_FLAGS_DEBUG} ${${CC_NAME}_DEFINES_DEBUG}")

    set (linker_flags  "${${CC_NAME}_LINKER_FLAGS_DEBUG}")
  else ()
    set (cxx_flags "${${CXX_NAME}_WARNINGS} ${${CXX_NAME}_FLAGS_RELEASE} ${${CC_NAME}_DEFINES_RELEASE}")
    set (cc_flags  "${${CC_NAME}_WARNINGS} ${${CC_NAME}_FLAGS_RELEASE} ${${CC_NAME}_DEFINES_RELEASE}")

    set (cxx_no_warn_flags "${${CXX_NAME}_NO_WARNINGS} ${${CXX_NAME}_FLAGS_RELEASE} ${${CC_NAME}_DEFINES_RELEASE}")
    set (cc_no_warn_flags  "${${CC_NAME}_NO_WARNINGS} ${${CC_NAME}_FLAGS_RELEASE} ${${CC_NAME}_DEFINES_RELEASE}")

    set (linker_flags  "${${CC_NAME}_LINKER_FLAGS_RELEASE}")
  endif ()
  set (cc_flags "${cc_flags} ${MULTI_THREADED_COMPILE_FLAGS} ${ONYX_COMMON_CC_FLAGS}")
  set (cxx_flags "${cxx_flags} ${MULTI_THREADED_COMPILE_FLAGS} ${ONYX_COMMON_CXX_FLAGS}")

endmacro ()
# ======================================================================
if (ONYX_TOOLSET STREQUAL "intel")
  config_compile_and_linker_flags_name (IPP ICC)
elseif (ONYX_TOOLSET STREQUAL "gcc44" OR ONYX_TOOLSET STREQUAL "gcc45" OR ONYX_TOOLSET STREQUAL "gcc46" OR ONYX_TOOLSET STREQUAL "gcc47")
  config_compile_and_linker_flags_name (GPP GCC)
elseif (ONYX_TOOLSET STREQUAL "vc80" OR ONYX_TOOLSET STREQUAL "vc90"  OR ONYX_TOOLSET STREQUAL "vc10" )
  config_compile_and_linker_flags_name (MSVC_CXX MSVC_CC)
elseif (ONYX_TOOLSET STREQUAL "SunCC")
  config_compile_and_linker_flags_name (SUN_CXX SUN_CC)
else ()
  message (FATAL_ERROR "Unknown compiler ${ONYX_TOOLSET}")
endif ()


if (OPENMP_FOUND)
set (cc_flags "${cc_flags} ${OpenMP_C_FLAGS}")
set (cxx_flags "${cxx_flags} ${OpenMP_CXX_FLAGS}")
endif ()

message ("------------------------------------------")
message ("CC  flags: ${cc_flags}")
message ("C++ flags: ${cxx_flags}")
message ("------------------------------------------")

