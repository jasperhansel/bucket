#include "common.hxx"
#include "compiler_objects/module.hxx"
using namespace cobjs;


Module::Module(ast::Class* cls)
: Class(nullptr, cls)
{}


Object* Module::addToModuleFreeList(std::unique_ptr<Object>&& object)
{
  auto ptr = object.get();
  free_list.push_back(std::move(object));
  return ptr;
}


Object* Module::lookup(const std::string& name)
{
  auto iter = map.find(name);
  if (iter != map.end())
    return iter->second;
  return nullptr;
}
