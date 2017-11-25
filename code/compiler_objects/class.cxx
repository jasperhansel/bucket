#include "miscellaneous/common.hxx"
#include "compiler_objects/class.hxx"
#include "compiler_objects/field.hxx"
#include "compiler_objects/method.hxx"
#include "abstract_syntax_tree/abstract_syntax_tree.hxx"
#include "abstract_syntax_tree/caster.hxx"
#include "miscellaneous/concatenate.hxx"
#include <stdexcept>
#include <utility>
using namespace cobjs;


Class::Class(Scope* parent_scope, ast::Class* cls)
: Scope(parent_scope),
  name(cls->name)
{}


void Class::init(ast::Class* cls)
{
  for (auto& global_statement : cls->body) {
    if (auto class_ptr = ast::ast_cast<ast::Class*>(global_statement.get())) {
      auto& entry = map[class_ptr->name];
      if (entry)
        throw std::runtime_error(misc::concatenate("ERROR REDEFINING NAME"));
      auto ptr = std::make_unique<Class>(this, class_ptr);
      ptr->init(class_ptr);
      entry = addToModuleFreeList(std::move(ptr));
    }
    else if (auto method_ptr = ast::ast_cast<ast::Method*>(global_statement.get())) {
      auto& entry = map[method_ptr->name];
      if (entry)
        throw std::runtime_error(misc::concatenate("ERROR REDEFINING NAME"));
      std::vector<Class*> argument_classes;
      for (auto& arg : method_ptr->args)
        argument_classes.push_back(lookupClass(arg.second.get()));
      entry = addToModuleFreeList(std::make_unique<Method>(this, method_ptr->name, std::move(argument_classes), lookupClass(method_ptr->return_class.get())));
    }
    else {
      assert(ast::ast_cast<ast::Field*>(global_statement.get()));
      auto field_ptr = static_cast<ast::Field*>(global_statement.get());
      auto& entry = map[field_ptr->name];
      if (entry)
        throw std::runtime_error(misc::concatenate("ERROR REDEFINING NAME"));
      entry = addToModuleFreeList(std::make_unique<Field>(this, field_ptr->name, lookupClass(field_ptr->cls.get())));
    }
  }
}


Class* Class::castToClass()
{
  return this;
}
