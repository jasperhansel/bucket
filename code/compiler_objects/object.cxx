#include "miscellaneous/common.hxx"
#include "compiler_objects/object.hxx"
#include "compiler_objects/scope.hxx"
using namespace cobjs;


Object::Object(Scope* parent_scope) noexcept
: parent_scope(parent_scope)
{}


Object* Object::addToModuleFreeList(std::unique_ptr<Object>&& object)
{
  return parent_scope->addToModuleFreeList(std::move(object));
}


Class* Object::castToClass()
{
  return nullptr;
}
