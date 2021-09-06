#ifndef Meta_map_template_function_to_type_list_hpp_
#define Meta_map_template_function_to_type_list_hpp_

#include "type_list.hpp"
#include "catenate_type_list.hpp"

namespace Meta
{

  template< template< class > class Function, class TypeList >
  struct map_template_function_to_type_list;

  template< template< class > class Function, class TypeList >
  using map_template_function_to_type_list_t = typename map_template_function_to_type_list< Function, TypeList >::type;

  template< template< class > class Function >
  struct map_template_function_to_type_list< Function, type_list< > >
  {
    typedef type_list< > type;
  };

  template< template< class > class Function, class Type, class... TypePack >
  struct map_template_function_to_type_list< Function, type_list< Type, TypePack... > >
  {
    typedef catenate_type_list<
      type_list< typename Function< Type >::type >,
      map_template_function_to_type_list_t< Function, type_list< TypePack... > > > type;
  };

}

#endif
