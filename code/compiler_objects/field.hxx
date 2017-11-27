#pragma once
#include "common.hxx"
#include "compiler_objects/object.hxx"
#include <string>


namespace cobjs {


class Class;


class Field : public Object {

public:

  Field(Scope* parent, const std::string& name, Class* cls);

protected:

  const std::string name;

  Class* const cls;

};


}
