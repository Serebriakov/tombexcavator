set (time_src
  time/time.cpp
)

set (time_hdr
  time/time.hpp
)
# ======================================
set (logger_src
  logger/locus.cpp
  logger/record.cpp
  logger/level.cpp
  logger/predicate.cpp
  logger/abstract_printer.cpp
  logger/layout_commands.cpp
  logger/layout.cpp
  logger/log_record_layout.cpp
  logger/formatter.cpp
  logger/device.cpp
  logger/system.cpp
  logger/logger_helper.cpp
  logger/simple_printer.cpp
  logger/console_printer.cpp
  logger/standard_layout.cpp
  logger/console.cpp
  logger/file_device.cpp
)

set (logger_hdr
  logger/locus.hpp
  logger/level.hpp
  logger/record.hpp
  logger/multi_setter.hpp
  logger/predicate.hpp
  logger/abstract_printer.hpp
  logger/layout_commands.hpp
  logger/layout.hpp
  logger/log_record_layout.hpp
  logger/formatter.hpp
  logger/device.hpp
  logger/function_name.hpp
  logger/system.hpp
  logger/logger_helper.hpp
  logger/simple_printer.hpp
  logger/console_printer.hpp
  logger/standard_layout.hpp
  logger/console.hpp
  logger/logger.hpp
  logger/file_device.hpp
)

# ======================================
set (ptr_hdr
  ptr/ptr_counter.hpp
  ptr/weak_ptr.hpp
  ptr/shared_ptr.hpp
  ptr/ptr.hpp
)

set (guard_hdr
  guard/destructor.hpp
)

set (ipc_hdr
  ipc/network/network.hpp
)

set (ipc_src
  ipc/network/network.cpp
)
# ======================================
set (mt_hdr
  mt/os_config.hpp
  mt/queue.hpp
  mt/processor.hpp
  mt/thread_id.hpp
  mt/mutex.hpp
  mt/thread.hpp
  mt/event.hpp
  mt/atomic.hpp
)

set (mt_src
  mt/thread_id.cpp
  mt/mutex.cpp
  mt/thread.cpp
  mt/event.cpp
  mt/atomic.cpp
)
# ======================================
set (err_hdr
  err/err.hpp
  err/throw.hpp
)

set (err_src
  err/err.cpp
)
# ======================================
set (generic_src
  object_manager.cpp
)

set (generic_hdr
  byte_order.hpp
  safe_cast.hpp
  macros.hpp
  object_manager.hpp
  bsw_api.hpp
  proper_export.h
)
# ========================
# housekeeping
# ========================

set (modules  mt logger time err ipc generic)

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

if (UNIX)
  set (PTHREAD pthread)
  set (WINLIBS )
else ()
  set (PTHREAD )
  set (WINLIBS Ws2_32.lib Iphlpapi.lib)
endif ()

onyx_shared_lib (bsw src hdr atl_expat ${PTHREAD} ${WINLIBS})

