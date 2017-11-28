#pragma once
#include "common.hxx"
#include "compiler_objects/object.hxx"
#include <string>
#include <unordered_map>


namespace ast {


struct Expression;


}


namespace cobjs {


class Class;


class Scope : public Object {

public:

  explicit Scope(Scope* parent_scope) noexcept;

  virtual Object* lookup(const std::string& name);

  Class* lookupClass(ast::Expression* expression);

protected:

  std::unordered_map<std::string, Object*> map;

};


}
