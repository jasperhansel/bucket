#pragma once
#include "miscellaneous/common.hxx"
#include "compiler_objects/object.hxx"
#include <string>


namespace cobjs {


class Class;


class Field : public Object {

public:

  Field(Scope* parent, const std::string& name, Class* cls);

private:

  const std::string name;

  Class* const cls;

};


}
