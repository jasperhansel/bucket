#pragma once
#include "miscellaneous/common.hxx"
#include "compiler_objects/object.hxx"
#include <string>
#include <vector>


namespace cobjs {


class Class;


class Method : public Object {

public:

  Method(Scope* parent_scope, const std::string& name, std::vector<Class*>&& argument_classes, Class* return_type);

private:

  const std::string name;

  std::vector<Class*> argument_classes;

  Class* return_type;

};


}
