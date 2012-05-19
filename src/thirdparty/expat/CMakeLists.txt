set (hdr
amigaconfig.h
ascii.h
asciitab.h
expat_external.h
expat.h
iasciitab.h
internal.h
latin1tab.h
macconfig.h
nametab.h
utf8tab.h
winconfig.h
xmlrole.h
xmltok.h
xmltok_impl.h
)


set (src
xmlparse.c
xmlrole.c
xmltok.c
xmltok_impl.c
xmltok_ns.c
)


if (${ONYX_HAS_BIGENDIAN})
set (EXPAT_DLL_DEFS "-DBIGENDIAN -DXML_BUILDING_EXPAT -DXML_USE_MSC_EXTENSIONS")
else ()
set (EXPAT_DLL_DEFS "-DXML_BUILDING_EXPAT -DXML_USE_MSC_EXTENSIONS")
endif ()

add_library (atl_expat SHARED ${src} ${hdr})
if (UNIX OR MINGW)
  set_target_properties (atl_expat PROPERTIES COMPILE_FLAGS "-D_LARGEFILE64_SOURCE=1 -DZ_HAVE_UNISTD_H ${EXPAT_DLL_DEFS} -w")
endif ()

if (WIN32 AND NOT MINGW)
  set_target_properties (atl_expat PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS ${EXPAT_DLL_DEFS} /W0")
endif ()

