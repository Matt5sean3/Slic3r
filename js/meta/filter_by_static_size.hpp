#ifndef Meta_filter_by_static_size_hpp_
#define Meta_filter_by_static_size_hpp_

#include <type_traits>
#include "filter.hpp"

namespace Meta
{
  template< size_t LowerSize, size_t UpperSize, class TypeList >
  struct filter_by_static_size;

  template< size_t LowerSize, size_t UpperSize, class TypeList >
  using filter_by_static_size_t = typename filter_by_has_static_size< TypeList >::type;

  template< class Type, class LowerSize, class UpperSize >
  struct condition_static_size
  {
    static constexpr bool value =
      LowerSize::value < Type::static_size &&
      UpperSize::value >= Type::static_size;
  };

  template< size_t LowerSize, size_t UpperSize, class TypeList >
  struct filter_by_static_size : filter<
    condition_static_size,
    TypeList,
    std::integral_constant< size_t, LowerSize >,
    std::integral_constant< size_t, UpperSize > > { };
}

#endif
