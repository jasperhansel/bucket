#pragma once
#include "abstract_syntax_tree/visitor.hxx"
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>


namespace ast {


struct Node {
  virtual ~Node() = default;
  virtual void receive(Visitor& visitor) = 0;
};


struct Program final : Node {
  std::vector<std::unique_ptr<GlobalStatement>> global_statements;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct GlobalStatement : Node {};


struct Class final : GlobalStatement {
  std::string name;
  std::vector<std::unique_ptr<GlobalStatement>> body;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Method final : GlobalStatement {
  std::string name;
  std::vector<std::pair<std::string, std::unique_ptr<Expression>>> args;
  std::unique_ptr<Expression> return_class;
  std::vector<std::unique_ptr<Statement>> body;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Field final : GlobalStatement {
  std::string name;
  std::unique_ptr<Expression> cls;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Statement : Node {};


struct If final : Statement {
  std::unique_ptr<Expression> condition;
  std::vector<std::unique_ptr<Statement>> if_body;
  std::vector<std::pair<std::unique_ptr<Expression>, std::vector<std::unique_ptr<Statement>>>> elif_bodies;
  std::vector<std::unique_ptr<Statement>> else_body;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Loop final : Statement {
  std::vector<std::unique_ptr<Statement>> body;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Break final : Statement {
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Cycle final : Statement {
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Ret final : Statement {
  std::unique_ptr<Expression> value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct ExpressionStatement final : Statement {
  std::unique_ptr<Expression> value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Expression : Node {};


struct Assignment final : Expression {
  std::unique_ptr<Expression> left, right;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Call final : Expression {
  std::unique_ptr<Expression> object;
  std::string name;
  std::vector<std::unique_ptr<Expression>> args;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Identifier final : Expression {
  std::string value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Integer final : Expression {
  unsigned long value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Real final : Expression {
  double value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct String final : Expression {
  std::string value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Character final : Expression {
  char value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


struct Bool final : Expression {
  bool value;
  inline void receive(Visitor& visitor) override {visitor.visit(this);}
};


std::ostream& operator<<(std::ostream& stream, Node& node);


}
