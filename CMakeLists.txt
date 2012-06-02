cmake_minimum_required(VERSION 2.8)

#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their
# definition
# and dump it in the cache along with proper documentation, otherwise set
# CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
if (DEFINED CMAKE_BUILD_TYPE)
  message ("Appling CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} from command line")
  set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
Release RelWithDebInfo MinSizeRel.")
else ()
   set (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
RelWithDebInfo MinSizeRel.")
endif ()


project (tomb_excavator)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include (global)

message (STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message (STATUS "Build name: ${BUILDNAME}")

include (config_flags)
include (nowarn)
include (build)

enable_testing()

# --------------------------------------------------------------
# configure
# --------------------------------------------------------------
find_package (SDL)
find_package (Qt4)

if (SDL_FOUND)
  OPTION (TOMB_EXCAVATOR_WITH_SDL "Use SDL" 1)
  message (STATUS "SDL found. ${SDL_INCLUDE_DIR} ${SDL_LIBRARY}")
else ()
  message (STATUS "SDL support disabled")
endif ()

if (QT4_FOUND)
  OPTION (TOMB_EXCAVATOR_WITH_QT "Use QT" 1)
  include(${QT_USE_FILE})
  message (STATUS "QT found. ${QT_INCLUDE_DIR} ${QT_LIBRARIES}")
else ()
  message (STATUS "QT support disabled")
endif ()

configure_file (${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)

# -------------------------------------------------------------
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (ONYX_ROOT ${CMAKE_SOURCE_DIR})
set (ONYX_SRC  ${CMAKE_SOURCE_DIR}/src)
set (ONYX_THIRDPARTY  ${ONYX_SRC}/thirdparty)

set           (ONYX_INCLUDE           ${ONYX_SRC}           ${ONYX_THIRDPARTY}
${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_BINARY_DIR} ${SDL_INCLUDE_DIR} ${QT_INCLUDE_DIR})

set (modules
  westwood

  bsw
  pict
  thirdparty/expat
  thirdparty/libz
)

foreach (module ${modules})
  add_subdirectory(${ONYX_SRC}/${module} obj/${module})
endforeach ()

