#ifndef Meta_filter_by_has_static_size_hpp_
#define Meta_filter_by_has_static_size_hpp_

#include "filter.hpp"

namespace Meta
{

  template< class TypeList >
  struct filter_by_has_static_size;

  template< class TypeList >
  using filter_by_has_static_size_t = typename filter_by_has_static_size< TypeList >::type;

  template< class Type >
  struct has_static_size_condition
  {
    static constexpr bool value = Type::has_static_value;
  };

  template< class TypeList >
  struct filter_by_has_static_size : filter< has_static_size_condition, TypeList > { };

}

#endif
