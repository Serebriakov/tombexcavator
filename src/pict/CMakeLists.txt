set (allocator_src
  allocator/sdl_allocator.cpp
)

set (allocator_hdr
  allocator/sdl_allocator.hpp
)

set (bmp_src
  bmp/bmp.cpp
)

set (bmp_hdr
  bmp/bmp.hpp
)

set (generic_src
  abstract_picture.cpp
  bpp.cpp
  allocator.cpp
)

set (generic_hdr
  pict_api.hpp
  abstract_picture.hpp
  allocator.hpp
  bpp.hpp
)
# ========================
# housekeeping
# ========================

set (modules generic allocator bmp)

set (src )
set (hdr )

foreach (m ${modules})
  set (mdl_src ${${m}_src})
  set (mdl_hdr ${${m}_hdr})
  
  foreach (s ${mdl_src})
    list (APPEND src ${s})
  endforeach ()

  foreach (h ${mdl_hdr})
    list (APPEND hdr ${h})
  endforeach ()
  if (m MATCHES "generic")
    source_group ("sources" FILES ${mdl_src})
    source_group ("headers" FILES ${mdl_hdr})
  else ()
    source_group ("${m}\\sources" FILES ${mdl_src})
    source_group ("${m}\\headers" FILES ${mdl_hdr})
  endif ()
endforeach ()

onyx_shared_lib (pict src hdr bsw ${SDL_LIBRARY} ${QT_LIBRARIES})

add_subdirectory (demos)
