#ifndef Meta_filter_hpp_
#define Meta_filter_hpp_

#include <type_traits>
#include "catenate_type_list.hpp"
#include "map_template_function_to_type_list.hpp"
#include "apply_template_function_to_type_list.hpp"

namespace Meta
{
  template< template < class... > class Condition, class TypeList, class... ConditionArgs >
  struct filter
  {
    template< class Type >
    struct check
    {
      typedef std::conditional_t<
        Condition< Type, ConditionArgs... >::value,
        type_list< Type >,
        type_list< > > type;
    };

    typedef apply_template_function_to_type_list_t<
      catenate_type_list,
      map_template_function_to_type_list_t< check, TypeList > > type;
  };

}

#endif
