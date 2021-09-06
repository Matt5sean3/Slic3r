#ifndef Meta_type_list_size_hpp_
#define Meta_type_list_size_hpp_

namespace Meta
{
  template< class TypeList >
  struct type_list_size;

  template< >
  struct type_list_size< type_list< > >
  {
    static constexpr size_t value = 0;
  };

  template< class Type, class... TypePack >
  struct type_list_size< type_list< Type, TypePack... > >
  {
    static constexpr size_t value = 1 + type_list_size< type_list< TypePack... > >::value;
  };

  template< class TypeList >
  inline constexpr size_t type_list_size_v = type_list_size< TypeList >::value;
}

#endif
