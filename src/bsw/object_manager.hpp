#ifndef __BSW_OBJECT_MANAGER_HPP__
#define __BSW_OBJECT_MANAGER_HPP__

#include "bsw/guard/destructor.hpp"
#include "bsw/bsw_api.hpp"

namespace bsw
{
  class BSW_API object_manager_callback_c;
  BSW_API void register_object_manager_callback (object_manager_callback_c* cbk);

  namespace priv
  {
    void on_exit ();
  }

  class object_manager_callback_c
  {
    friend void priv::on_exit ();
  public:
    virtual void call () = 0;
  protected:
    object_manager_callback_c ();
    virtual ~object_manager_callback_c (); // prevent creation of instances on stack
  private:
    object_manager_callback_c (const object_manager_callback_c&);
    object_manager_callback_c& operator = (const object_manager_callback_c&);
  };

  template <class T>
  class object_deleter_c : public object_manager_callback_c
  {
  public:
    object_deleter_c (T* subject);
  private:
    virtual void call ();
  private:
    bsw::utils::destructor_c <T> m_destructor;
  };
} // ns bsw
// =====================================================================
// Implementation
// =====================================================================
namespace bsw
{
  template <class T>
  object_deleter_c <T>::object_deleter_c (T* subject)
    : m_destructor (subject)
  {
  }
  // -------------------------------------------------------------------
  template <class T>
  void object_deleter_c <T>::call ()
  {
    m_destructor ();
  }
} // ns bsw

#endif
