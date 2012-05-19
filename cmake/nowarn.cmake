macro (CC_FILE_NO_WARNINGS target)
  set_source_files_properties (${target} PROPERTIES COMPILE_FLAGS ${cc_no_warn_flags})
endmacro ()

macro (CXX_FILE_NO_WARNINGS target)
  message (${cxx_no_warn_flags})
  set_source_files_properties (${target} PROPERTIES COMPILE_FLAGS ${cxx_no_warn_flags})
endmacro ()
