#include "miscellaneous/common.hxx"
#include "compiler_objects/method.hxx"
#include <utility>
using namespace cobjs;


Method::Method(Scope* parent_scope, const std::string& name, std::vector<Class*>&& argument_classes, Class* return_type)
: Object(parent_scope),
  name(name),
  argument_classes(std::move(argument_classes)),
  return_type(return_type)
{}
