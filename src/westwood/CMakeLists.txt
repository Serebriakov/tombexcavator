set (generic_src
  fat_entry.cpp
  fat.cpp
)

set (generic_hdr
  westwood_api.hpp
  fat_entry.hpp
  fat.hpp
)
# ========================
# housekeeping
# ========================

set (modules generic)

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

onyx_shared_lib (westwood src hdr bsw)

add_subdirectory (demos)