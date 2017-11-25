#pragma once
#include "miscellaneous/common.hxx"
#include "compiler_objects/scope.hxx"


namespace ast {

struct Class;

}


namespace cobjs {


class Class : public Scope {

public:

  Class(Scope* parent_scope, ast::Class* cls);

  void init(ast::Class* cls);

  Class* castToClass() override;

protected:

  std::string name;

};


}
