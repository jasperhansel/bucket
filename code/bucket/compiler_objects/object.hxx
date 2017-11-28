#pragma once
#include "common.hxx"
#include <memory>


namespace cobjs {


class Scope;
class Class;


class Object {

public:

  explicit Object(Scope* parent_scope) noexcept;

  virtual ~Object() = default;

  virtual Class* castToClass();

protected:

  Scope* const parent_scope;

  virtual Object* addToModuleFreeList(std::unique_ptr<Object>&& object);

};


}
