#pragma once
#include "miscellaneous/common.hxx"


namespace ast {


struct Node;
struct Program;
struct GlobalStatement;
struct Class;
struct Method;
struct Field;
struct Statement;
struct If;
struct Loop;
struct Break;
struct Cycle;
struct Ret;
struct ExpressionStatement;
struct Expression;
struct Assignment;
struct Call;
struct Identifier;
struct Integer;
struct Real;
struct String;
struct Character;
struct Bool;


class Visitor {

public:

  virtual ~Visitor() = default;

  virtual void visit(Program* program_ptr) = 0;

  virtual void visit(Class* class_ptr) = 0;

  virtual void visit(Method* method_ptr) = 0;

  virtual void visit(Field* field_ptr) = 0;

  virtual void visit(If* if_ptr) = 0;

  virtual void visit(Loop* loop_ptr) = 0;

  virtual void visit(Break* break_ptr) = 0;

  virtual void visit(Cycle* cycle_ptr) = 0;

  virtual void visit(Ret* ret_ptr) = 0;

  virtual void visit(ExpressionStatement* expression_statement_ptr) = 0;

  virtual void visit(Assignment* assignment_ptr) = 0;

  virtual void visit(Call* call_ptr) = 0;

  virtual void visit(Identifier* identifier_ptr) = 0;

  virtual void visit(Integer* integer_ptr) = 0;

  virtual void visit(Real* real_ptr) = 0;

  virtual void visit(String* string_ptr) = 0;

  virtual void visit(Character* character_ptr) = 0;

  virtual void visit(Bool* bool_ptr) = 0;

protected:

  Visitor() = default;

};


}
