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

  void visit(Class* class_ptr) override {cast(class_ptr);}

  void visit(Method* method_ptr) override {cast(method_ptr);}

  void visit(Field* field_ptr) override {cast(field_ptr);}

  void visit(If* if_ptr) override {cast(if_ptr);}

  void visit(Loop* loop_ptr) override {cast(loop_ptr);}

  void visit(Break* break_ptr) override {cast(break_ptr);}

  void visit(Cycle* cycle_ptr) override {cast(cycle_ptr);}

  void visit(Ret* ret_ptr) override {cast(ret_ptr);}

  void visit(ExpressionStatement* expression_statement_ptr) override {cast(expression_statement_ptr);}

  void visit(Assignment* assignment_ptr) override {cast(assignment_ptr);}

  void visit(Call* call_ptr) override {cast(call_ptr);}

  void visit(Identifier* identifier_ptr) override {cast(identifier_ptr);}

  void visit(Integer* integer_ptr) override {cast(integer_ptr);}

  void visit(Real* real_ptr) override {cast(real_ptr);}

  void visit(String* string_ptr) override {cast(string_ptr);}

  void visit(Character* character_ptr) override {cast(character_ptr);}

  void visit(Bool* bool_ptr) override {cast(bool_ptr);}

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
  assert(caster.result == dynamic_cast<T1>(ptr));
  return caster.result;
}


}
