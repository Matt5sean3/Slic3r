#ifndef Meta_map_template_to_type_list_hpp_
#define Meta_map_template_to_type_list_hpp_

#include "type_list.hpp"
#include "catenate_type_list.hpp"

namespace Meta
{

  template< template< class > class Template, class TypeList, class... TemplateParameters >
  struct map_template_to_type_list;

  template< template< class... > class Template, class TypeList, class... TemplateParameters >
  using map_template_to_type_list_t = typename map_template_to_type_list< Template, TypeList, TemplateParameters... >::type;

  template< template< class... > class Template, class... TemplateParameters >
  struct map_template_to_type_list< Template, type_list< >, TemplateParameters... >
  {
    typedef type_list< > type;
  };

  template< template< class... > class Template, class Type, class... TypePack, class... TemplateParameters >
  struct map_template_to_type_list< Template, type_list< Type, TypePack... >, TemplateParameters... >
  {
    typedef catenate_type_list_t<
      type_list< Template< Type, TemplateParameters... > >,
      map_template_to_type_list_t< Template, type_list< TypePack... >, TemplateParameters... > > type;
  };

}

#endif
