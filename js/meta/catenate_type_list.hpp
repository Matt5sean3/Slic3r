#ifndef Meta_catenate_type_list_hpp_
#define Meta_catenate_type_list_hpp_

#include "type_list.hpp"

namespace Meta
{
  template< class... TypeLists >
  struct catenate_type_list;

  template< class... TypeLists >
  using catenate_type_list_t = typename catenate_type_list< TypeLists... >::type;

  template< >
  struct catenate_type_list< >
  {
    typedef type_list< > type;
  };

  template< class... A >
  struct catenate_type_list< type_list< A... > >
  {
    typedef type_list< A... > type;
  };


  template< class... A, class... B, class... Ts >
  struct catenate_type_list< type_list< A... >, type_list< B... >, Ts... >
  {
    typedef catenate_type_list_t< type_list< A..., B... >, Ts... > type;
  };

}

#endif
