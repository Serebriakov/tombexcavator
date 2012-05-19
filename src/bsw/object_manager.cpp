#include <stdlib.h>
#include <list>

#include "bsw/object_manager.hpp"

#include "bsw/mt/mutex.hpp"

namespace bsw
{
  namespace
  {
    typedef std::list <object_manager_callback_c*> cbk_list_t;
    
    cbk_list_t*  cbk_list = 0;
    mt::mutex_c* cbk_mtx = 0;
  }

  namespace priv
  {
    void on_exit ()
    {
      {
	mt::lock_guard_c <mt::mutex_c> guard (*cbk_mtx);
	for (cbk_list_t::iterator i = cbk_list->begin (); 
	     i != cbk_list->end (); i++)
	  {
	    object_manager_callback_c* victim = *i;
	    try
	      {
		victim->call ();
	      }
	    catch (...)
	      {
	      }
	    delete victim;
	  }
	delete cbk_list;
	cbk_list = 0;
      }
      delete cbk_mtx;
      cbk_mtx = 0;
    }
  } // ns priv
  // ---------------------------------------------------------------------------
  void register_object_manager_callback (object_manager_callback_c* cbk)
  {
    if (cbk_list == 0)
      {
	cbk_list = new cbk_list_t;
	cbk_mtx  = new mt::mutex_c;

	atexit (priv::on_exit);
      }
    cbk_list->push_back (cbk);
  }
  // ==========================================================================
  object_manager_callback_c::object_manager_callback_c ()
  {
  }
  // --------------------------------------------------------------------------
  object_manager_callback_c::~object_manager_callback_c ()
  {
  }
} // ns bsw
