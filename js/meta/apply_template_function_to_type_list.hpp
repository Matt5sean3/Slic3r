#ifndef Meta_apply_template_function_to_type_list_hpp_
#define Meta_apply_template_function_to_type_list_hpp_

#include "type_list.hpp"

namespace Meta
{

  template< template< class... > class Template, class TypeList >
  struct apply_template_function_to_type_list;

  template< template< class... > class Template, class TypeList >
  using apply_template_function_to_type_list_t = typename apply_template_function_to_type_list< Template, TypeList >::type;

  template< template< class... > class Template, class... TypePack >
  struct apply_template_function_to_type_list< Template, type_list< TypePack... > >
  {
    typedef typename Template< TypePack... >::type type;
  };

}

#endif
