
  // === has_static_size ===
  template< class C >
  struct has_static_size : std::integral_constant< bool, C::has_static_size > { };

  // === filter_by_has_dynamic_size ===
  // === filter_by_static_size ===
  template< size_t LowerSize, size_t UpperSize >
  struct filter_by_static_size
  {
    template< class Type >
    struct parameterized
    {
      typedef std::conditional_t<
        Field< Type >::static_size > LowerSize && Field< Type >::static_size <= UpperSize,
        type_list< Type >,
        type_list< > > type;
    };
  };

  template< size_t LowerSize, size_t UpperSize, class... Types >
  struct filter_by_static_size
  {
    typedef apply_template_function_to_type_list_t<
      catenate_type_list,
      map_template_function_to_types_t< retain_by_static_size< LowerSize, UpperSize >::template parameterized, Types... > > type;
  };
