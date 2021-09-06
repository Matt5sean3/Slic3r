#ifndef Meta_type_list_reduce_hpp_
#define Meta_type_list_reduce_hpp_

namespace Meta
{
  template< template < class, class > class Op, class TypeList >
  struct type_list_reduce;

  template< template< class, class > class Op, class TypeA, class TypePack... >
  struct type_list_reduce< Op, type_list< Type, TypePack... > >
  {
    static constexpr auto value = Op<
      TypeA::value,
      type_list_reduce< Op, type_list< TypePack... > >::value
    >::value;
  };
}

#endif
