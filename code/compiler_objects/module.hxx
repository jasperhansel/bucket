#pragma once
#include "miscellaneous/common.hxx"
#include "compiler_objects/class.hxx"
#include <memory>
#include <vector>


namespace cobjs {


class Module : public Class {

public:

  Module(ast::Class* cls);

  Object* lookup(const std::string& name) override;

protected:

  std::vector<std::unique_ptr<Object>> free_list;

  Object* addToModuleFreeList(std::unique_ptr<Object>&& object) override;

};


}
