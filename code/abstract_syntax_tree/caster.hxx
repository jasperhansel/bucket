#pragma once
#include "miscellaneous/common.hxx"
#include <cassert>
#include <type_traits>


namespace ast {


namespace {


template <typename To>
class Caster final : public Visitor {

public:

  To* result = nullptr;

  void visit(Class* class_node) override {cast(class_node);}

  void visit(Method* method_node) override {cast(method_node);}

  void visit(Field* field_node) override {cast(field_node);}

  void visit(If* if_node) override {cast(if_node);}

  void visit(Loop* loop_node) override {cast(loop_node);}

  void visit(Break* break_node) override {cast(break_node);}

  void visit(Cycle* cycle_node) override {cast(cycle_node);}

  void visit(Ret* ret_node) override {cast(ret_node);}

  void visit(ExpressionStatement* expression_statement_node) override {cast(expression_statement_node);}

  void visit(Assignment* assignment_node) override {cast(assignment_node);}

  void visit(Call* call_node) override {cast(call_node);}

  void visit(Identifier* identifier_node) override {cast(identifier_node);}

  void visit(Integer* integer_node) override {cast(integer_node);}

  void visit(Real* real_node) override {cast(real_node);}

  void visit(String* string_node) override {cast(string_node);}

  void visit(Character* character_node) override {cast(character_node);}

  void visit(Bool* bool_node) override {cast(bool_node);}

private:

  template <typename From>
  void cast(From* ptr)
  {
    if constexpr (std::is_base_of<To, From>::value)
      result = static_cast<To*>(ptr);
    else
      result = nullptr;
  }

};


}


template <typename T1, typename T2>
T1 ast_cast(T2 ptr)
{
  static_assert(std::is_pointer<T1>::value);
  static_assert(std::is_pointer<T2>::value);
  using To = typename std::remove_pointer<T1>::type;
  using From = typename std::remove_pointer<T2>::type;
  static_assert(std::is_base_of<Node, To>::value);
  static_assert(std::is_base_of<Node, From>::value);
  Caster<To> caster;
  ptr->receive(caster);
  assert(caster.result == dynamic_cast<From*>(ptr));
  return caster.result;
}


}
