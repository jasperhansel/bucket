#include "miscellaneous/common.hxx"
#include "compiler_objects/field.hxx"
using namespace cobjs;


Field::Field(Scope* parent, const std::string& name, Class* cls)
: Object(parent),
  name(name),
  cls(cls)
{}
