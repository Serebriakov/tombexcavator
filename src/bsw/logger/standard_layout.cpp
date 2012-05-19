#include "bsw/logger/standard_layout.hpp"
#include "bsw/logger/log_record_layout.hpp"

namespace bsw
{
  namespace logger
  {
    log_record_layout_c* standard_layout ()
    {
      log_record_layout_c* layout = new log_record_layout_c;
      layout->append_time_value ()->append_text (" ")->append_level ()->
          append_text (" ")->append_text ("@")->append_file_name ()->append_text (":")->append_line_number ()->
          append_space ()->append_func_name ()->append_text ("> ")->append_text ();
      return layout;
    }
  } // ns logger
} // ns bsw
