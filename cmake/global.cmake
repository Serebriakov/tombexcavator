include(CheckCXXSourceCompiles)


# Toolset detection.
if (NOT ONYX_TOOLSET)
  set(ONYX_TOOLSET "unknown")
  if (MSVC60)
    set(ONYX_TOOLSET "vc6")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "6.0")
  elseif(MSVC70)
    set(ONYX_TOOLSET "vc7")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "7.0")
  elseif(MSVC71)
    set(ONYX_TOOLSET "vc71")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "7.1")
  elseif(MSVC80)
    set(ONYX_TOOLSET "vc80")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "8.0")
  elseif(MSVC90)
    set(ONYX_TOOLSET "vc90")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "9.0")
elseif(MSVC10)
    set(ONYX_TOOLSET "vc10")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "10.0")
  elseif(MSVC)
    set(ONYX_TOOLSET "vc")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "unknown")
  elseif(BORLAND)
    set(ONYX_TOOLSET "bcb")
    set(ONYX_COMPILER "msvc")
    set(ONYX_COMPILER_VERSION "unknown")
  elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(ONYX_COMPILER "gcc")

    # Execute GCC with the -dumpversion option, to give us a version string
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} "-dumpversion" 
      OUTPUT_VARIABLE GCC_VERSION_STRING)
    
    # Match only the major and minor versions of the version string
    string(REGEX MATCH "[0-9]+.[0-9]+" GCC_MAJOR_MINOR_VERSION_STRING
      "${GCC_VERSION_STRING}")

    # Match the full compiler version for the build name
    string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+" ONYX_COMPILER_VERSION
      "${GCC_VERSION_STRING}")
    
    # Strip out the period between the major and minor versions
    string(REGEX REPLACE "\\." "" ONYX_VERSIONING_GCC_VERSION
      "${GCC_MAJOR_MINOR_VERSION_STRING}")
    
    # Set the GCC versioning toolset
    set(ONYX_TOOLSET "gcc${ONYX_VERSIONING_GCC_VERSION}")
  elseif(CMAKE_CXX_COMPILER MATCHES "/icpc$" OR CMAKE_CXX_COMPILER MATCHES "/icpc.exe$")
    set(ONYX_TOOLSET "intel")
    set(ONYX_COMPILER "intel")
    set(CMAKE_COMPILER_IS_INTEL ON)
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} "-dumpversion"
      OUTPUT_VARIABLE INTEL_VERSION_STRING
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(ONYX_COMPILER_VERSION ${INTEL_VERSION_STRING})
  elseif (CMAKE_CXX_COMPILER MATCHES "/sun")
    set(ONYX_TOOLSET "SunCC")
    set(ONYX_COMPILER "SunCC")
    set(CMAKE_COMPILER_IS_SUNCC ON)
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} "-V"
      ERROR_FILE "${PROJECT_SOURCE_DIR}/ccver.tmp"
      ERROR_STRIP_TRAILING_WHITESPACE)
    FILE (STRINGS "${PROJECT_SOURCE_DIR}/ccver.tmp" SUNCC_VERSION_RAW LIMIT_COUNT 1)
    STRING (REGEX MATCH "[0-9]+(\\.[0-9]+)*" SUNCC_VERSION_STRING ${SUNCC_VERSION_RAW})
    FILE (REMOVE "${PROJECT_SOURCE_DIR}/ccver.tmp")
    set(ONYX_COMPILER_VERSION ${SUNCC_VERSION_STRING})
  endif(MSVC60)
endif (NOT ONYX_TOOLSET)


message(STATUS "Onyx compiler: ${ONYX_COMPILER}")
message(STATUS "Onyx toolset:  ${ONYX_TOOLSET}")

# create cache entry
set(ONYX_PLATFORM "unknown")

# check for 64 bit build

if( (MSVC AND CMAKE_CL_64) OR CMAKE_GENERATOR MATCHES Win64)
  set(BUILD_64BIT 1)
elseif(CMAKE_SIZEOF_VOID_P MATCHES 8)
  set(BUILD_64BIT 1)
endif()

if (BUILD_64BIT)
  message (STATUS "64 bit build enabled")
endif ()

# Reflect 64-bits options
if (BUILD_64BIT)
  set (GCC_64_FLAG -m64)
  set (ICC_64_FLAG -m64)
  set (SUNCC_64_FLAG -m64)
  set (MSVC_64_FLAG " /DWIN32 /D_WIN32 ")
else ()
    set (GCC_64_FLAG -m32)
  set (ICC_64_FLAG -m32)
  set (SUNCC_64_FLAG -m32)
    set (MSVC_64_FLAG " /DWIN32 /D_WIN32 ")
endif ()

# Multi-threading support
if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  set(MULTI_THREADED_COMPILE_FLAGS "-pthreads")
  set(MULTI_THREADED_LINK_LIBS rt)
  set(ONYX_PLATFORM "sunos")
elseif(CMAKE_SYSTEM_NAME STREQUAL "BeOS")
  # No threading options necessary for BeOS
  set(ONYX_PLATFORM "beos")
elseif(CMAKE_SYSTEM_NAME MATCHES ".*BSD")
  set(MULTI_THREADED_COMPILE_FLAGS "-pthread")
  set(MULTI_THREADED_LINK_LIBS pthread)
  set(ONYX_PLATFORM "bsd")
elseif(CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
  # DragonFly is a FreeBSD bariant
  set(MULTI_THREADED_COMPILE_FLAGS "-pthread")
  set(ONYX_PLATFORM "dragonfly")
elseif(CMAKE_SYSTEM_NAME STREQUAL "IRIX")
  # TODO: GCC on Irix doesn't support multi-threading?
  set(ONYX_PLATFORM "irix")
elseif(CMAKE_SYSTEM_NAME STREQUAL "HP-UX")
  # TODO: gcc on HP-UX does not support multi-threading?
  set(ONYX_PLATFORM "hpux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # No threading options necessary for Mac OS X
  set(ONYX_PLATFORM "macos")
elseif(UNIX)
  # Assume -pthread and -lrt on all other variants
  if (ONYX_TOOLSET STREQUAL "SunCC")
    set(MULTI_THREADED_COMPILE_FLAGS "-D_REENTRANT")
    set(MULTI_THREADED_LINK_FLAGS " ${SUNCC_64_FLAG} ")  
    set(MULTI_THREADED_LINK_LIBS pthread rt)
  else ()  
    set(MULTI_THREADED_COMPILE_FLAGS "-pthread -D_REENTRANT")
    set(MULTI_THREADED_LINK_FLAGS "")  
    set(MULTI_THREADED_LINK_LIBS pthread rt)
  endif ()

  if (MINGW)
    set(ONYX_PLATFORM "mingw")
  elseif(CYGWIN)
    set(ONYX_PLATFORM "cygwin")
  elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(ONYX_PLATFORM "linux")
  else()
    set(ONYX_PLATFORM "unix")
  endif()
elseif(WIN32)
  set(ONYX_PLATFORM "windows")
else()
  set(ONYX_PLATFORM "unknown")
endif()

# create cache entry
set(ONYX_PLATFORM ${ONYX_PLATFORM} CACHE STRING "Onyx platform name")

message(STATUS "Onyx platform: ${ONYX_PLATFORM}")

# Setup DEBUG_COMPILE_FLAGS, RELEASE_COMPILE_FLAGS, DEBUG_LINK_FLAGS and
# and RELEASE_LINK_FLAGS based on the CMake equivalents
if(CMAKE_CXX_FLAGS_DEBUG)
  if(MSVC)
    # Eliminate the /MDd flag; we'll add it back when we need it
    string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG 
           "${CMAKE_CXX_FLAGS_DEBUG}") 
  endif(MSVC)
  set(DEBUG_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "Compilation flags for debug libraries")
endif(CMAKE_CXX_FLAGS_DEBUG)
if(CMAKE_CXX_FLAGS_RELEASE)
  if(MSVC)
    # Eliminate the /MD flag; we'll add it back when we need it
    string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE
           "${CMAKE_CXX_FLAGS_RELEASE}") 
  endif(MSVC)
  set(RELEASE_COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Compilation flags for release libraries")
endif(CMAKE_CXX_FLAGS_RELEASE)
if(CMAKE_SHARED_LINKER_FLAGS_DEBUG)
  set(DEBUG_LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" CACHE STRING "Linker flags for debug libraries")
endif(CMAKE_SHARED_LINKER_FLAGS_DEBUG)
if(CMAKE_SHARED_LINKER_FLAGS_RELEASE)
  set(RELEASE_LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING "Link flags for release libraries")
endif(CMAKE_SHARED_LINKER_FLAGS_RELEASE)

# Set DEBUG_EXE_LINK_FLAGS, RELEASE_EXE_LINK_FLAGS
if (CMAKE_EXE_LINKER_FLAGS_DEBUG)
  set(DEBUG_EXE_LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
endif (CMAKE_EXE_LINKER_FLAGS_DEBUG)
if (CMAKE_EXE_LINKER_FLAGS_RELEASE)
  set(RELEASE_EXE_LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
endif (CMAKE_EXE_LINKER_FLAGS_RELEASE)

# Tweak the configuration and build types appropriately.
if(CMAKE_CONFIGURATION_TYPES)
  # Limit CMAKE_CONFIGURATION_TYPES to Debug and Release
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Semicolon-separate list of supported configuration types" FORCE)
else(CMAKE_CONFIGURATION_TYPES)
  # Build in release mode by default
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are Release or Debug" FORCE)
  endif (NOT CMAKE_BUILD_TYPE)
endif(CMAKE_CONFIGURATION_TYPES)

# Clear out the built-in C++ compiler and link flags for each of the 
# configurations.
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_CXX_FLAGS_MINSIZEREL "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "" CACHE INTERNAL "Unused by Onyx")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "" CACHE INTERNAL "Unused by Onyx")

# Set the build name 
set(BUILDNAME "${ONYX_COMPILER}-${ONYX_COMPILER_VERSION}-${ONYX_PLATFORM}")
