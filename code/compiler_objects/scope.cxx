#include "miscellaneous/common.hxx"
#include "compiler_objects/scope.hxx"
#include "abstract_syntax_tree/abstract_syntax_tree.hxx"
#include "abstract_syntax_tree/caster.hxx"
#include <stdexcept>
  #include <iostream>
using namespace cobjs;


Scope::Scope(Scope* parent_scope) noexcept
: Object(parent_scope)
{}


Object* Scope::lookup(const std::string& name)
{
  auto iter = map.find(name);
  if (iter != map.end())
    return iter->second;
  return parent_scope->lookup(name);
}


Class* Scope::lookupClass(ast::Expression* expression)
{
  if (auto identifier = ast::ast_cast<ast::Identifier*>(expression)) {
    auto object = lookup(identifier->value);
    if (!object)
      throw std::runtime_error("undefined identifier");
    auto result = object->castToClass();
    if (!result)
      throw std::runtime_error("not a class");
    return result;
  }
  throw std::runtime_error("TODO");
}
