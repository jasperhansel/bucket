#include "common.hxx"
#include "abstract_syntax_tree/visitor.hxx"
#include "compiler_objects/module.hxx"


namespace codegen {


class CodeGenerator final : public ast::Visitor {

public:

  explicit CodeGenerator(ast::Class* cls);

  void visit(ast::Class* class_ptr) override;

  void visit(ast::Method* method_ptr) override;

  void visit(ast::Field* field_ptr) override;

  void visit(ast::If* if_ptr) override;

  void visit(ast::Loop* loop_ptr) override;

  void visit(ast::Break* break_ptr) override;

  void visit(ast::Cycle* cycle_ptr) override;

  void visit(ast::Ret* ret_ptr) override;

  void visit(ast::ExpressionStatement* expression_statement_ptr) override;

  void visit(ast::Assignment* assignment_ptr) override;

  void visit(ast::Call* call_ptr) override;

  void visit(ast::Identifier* identifier_ptr) override;

  void visit(ast::Integer* integer_ptr) override;

  void visit(ast::Real* real_ptr) override;

  void visit(ast::String* string_ptr) override;

  void visit(ast::Character* character_ptr) override;

  void visit(ast::Bool* bool_ptr) override;

private:

  cobjs::Module module;

};


}
