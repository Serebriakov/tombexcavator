set (src adler32.c
compress.c
crc32.c
deflate.c
gzclose.c
gzlib.c
gzread.c
gzwrite.c
infback.c
inflate.c
inffast.c
inftrees.c
trees.c
uncompr.c
zutil.c
)

set (hdr crc32.h
deflate.h
gzguts.h
inffast.h
inffixed.h
inflate.h
inftrees.h
trees.h
zconf.h
zlib.h
zutil.h
)


set (ZLIB_DLL_DEFS "-DZLIB_DLL -DZLIB_INTERNAL")

add_library (atl_z SHARED ${src} ${hdr})
if (UNIX OR MINGW)
  set_target_properties (atl_z PROPERTIES COMPILE_FLAGS "-D_LARGEFILE64_SOURCE=1 -DZ_HAVE_UNISTD_H ${ZLIB_DLL_DEFS} -w")
endif ()

if (WIN32 AND NOT MINGW)
  set_target_properties (atl_z PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS ${ZLIB_DLL_DEFS} /W0")
endif ()



