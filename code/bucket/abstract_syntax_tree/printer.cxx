#include "common.hxx"
#include "abstract_syntax_tree/abstract_syntax_tree.hxx"
#include "abstract_syntax_tree/visitor.hxx"
using namespace ast;


namespace {


class Printer final : public Visitor {

public:

  explicit Printer(std::ostream& stream) noexcept;

  void visit(Class* class_ptr) override;

  void visit(Method* method_ptr) override;

  void visit(Field* field_ptr) override;

  void visit(If* if_ptr) override;

  void visit(Loop* loop_ptr) override;

  void visit(Break* break_ptr) override;

  void visit(Cycle* cycle_ptr) override;

  void visit(Ret* ret_ptr) override;

  void visit(ExpressionStatement* expression_statement_ptr) override;

  void visit(Assignment* assignment_ptr) override;

  void visit(Call* call_ptr) override;

  void visit(Identifier* identifier_ptr) override;

  void visit(Integer* integer_ptr) override;

  void visit(Real* real_ptr) override;

  void visit(String* string_ptr) override;

  void visit(Character* character_ptr) override;

  void visit(Bool* bool_ptr) override;

private:

  std::ostream& stream;

};


Printer::Printer(std::ostream& stream) noexcept
: stream(stream)
{}


void Printer::visit(Class* class_ptr)
{
  stream << "class " << class_ptr->name << '\n';
  for (auto& global_statement : class_ptr->body)
    global_statement->receive(*this);
  stream << "end" << '\n';
}


void Printer::visit(Method* method_ptr)
{
  stream << "method " << method_ptr->name << '(';
  auto iter = method_ptr->args.begin();
  if (iter != method_ptr->args.end()) {
    stream << iter->first << " : ";
    iter->second->receive(*this);
    ++iter;
    while (iter != method_ptr->args.end()) {
      stream << ", ";
      stream << iter->first << " : ";
      iter->second->receive(*this);
      ++iter;
    }
  }
  stream << ')';
  if (method_ptr->return_class) {
    stream << " : ";
    method_ptr->return_class->receive(*this);
  }
  stream << '\n';
  for (auto& statement : method_ptr->body)
    statement->receive(*this);
  stream << "end\n";
}


void Printer::visit(Field* field_ptr)
{
  stream << field_ptr->name << " : ";
  field_ptr->cls->receive(*this);
  stream << '\n';
}


void Printer::visit(If* if_ptr)
{
  stream << "if ";
  if_ptr->condition->receive(*this);
  stream << '\n';
  for (auto& statement : if_ptr->if_body)
    statement->receive(*this);
  for (auto& elif_body : if_ptr->elif_bodies) {
    stream << "elif ";
    elif_body.first->receive(*this);
    stream << '\n';
    for (auto& statement : elif_body.second)
      statement->receive(*this);
  }
  for (auto& statement : if_ptr->else_body)
    statement->receive(*this);
  stream << "end\n";
}


void Printer::visit(Loop* loop_ptr)
{
  stream << "do\n";
  for (auto& statement : loop_ptr->body)
    statement->receive(*this);
  stream << "end\n";
}


void Printer::visit(Break*)
{
  stream << "break\n";
}


void Printer::visit(Cycle*)
{
  stream << "cycle\n";
}


void Printer::visit(Ret* ret_ptr)
{
  stream << "ret";
  if (ret_ptr->value) {
    stream << ' ';
    ret_ptr->value->receive(*this);
  }
  stream << '\n';
}


void Printer::visit(ExpressionStatement* expression_statement_ptr)
{
  expression_statement_ptr->value->receive(*this);
  stream << '\n';
}


void Printer::visit(Assignment* assignment_ptr)
{
  assignment_ptr->left->receive(*this);
  stream << " = ";
  assignment_ptr->right->receive(*this);
  stream << '\n';
}


void Printer::visit(Call* call_ptr)
{
  call_ptr->object->receive(*this);
  stream << '.' << call_ptr->name << '(';
  auto iter = call_ptr->args.begin();
  if (iter != call_ptr->args.end()) {
    (*iter)->receive(*this);
    ++iter;
    while (iter != call_ptr->args.end()) {
      stream << ", ";
      (*iter)->receive(*this);
      ++iter;
    }
  }
  stream << ")\n";
}


void Printer::visit(Identifier* identifier_ptr)
{
  stream << identifier_ptr->value;
}


void Printer::visit(Integer* integer_ptr)
{
  stream << integer_ptr->value;
}


void Printer::visit(Real* real_ptr)
{
  stream << real_ptr->value;
}


void Printer::visit(String* string_ptr)
{
  stream << string_ptr->value;
}


void Printer::visit(Character* character_ptr)
{
  stream << character_ptr->value;
}


void Printer::visit(Bool* bool_ptr)
{
  stream << (bool_ptr->value ? "true" : "false");
}


}


std::ostream& ast::operator<<(std::ostream& stream, Node& node)
{
  Printer visitor{stream};
  node.receive(visitor);
  return stream;
}
