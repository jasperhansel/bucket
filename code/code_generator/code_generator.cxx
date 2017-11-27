#include "common.hxx"
#include "code_generator/code_generator.hxx"
using namespace codegen;


CodeGenerator::CodeGenerator(ast::Class* cls)
: module(cls)
{
  module.init(cls);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void CodeGenerator::visit(ast::Class* class_ptr) {}

void CodeGenerator::visit(ast::Method* method_ptr) {}

void CodeGenerator::visit(ast::Field* field_ptr) {}

void CodeGenerator::visit(ast::If* if_ptr) {}

void CodeGenerator::visit(ast::Loop* loop_ptr) {}

void CodeGenerator::visit(ast::Break* break_ptr) {}

void CodeGenerator::visit(ast::Cycle* cycle_ptr) {}

void CodeGenerator::visit(ast::Ret* ret_ptr) {}

void CodeGenerator::visit(ast::ExpressionStatement* expression_statement_ptr) {}

void CodeGenerator::visit(ast::Assignment* assignment_ptr) {}

void CodeGenerator::visit(ast::Call* call_ptr) {}

void CodeGenerator::visit(ast::Identifier* identifier_ptr) {}

void CodeGenerator::visit(ast::Integer* integer_ptr) {}

void CodeGenerator::visit(ast::Real* real_ptr) {}

void CodeGenerator::visit(ast::String* string_ptr) {}

void CodeGenerator::visit(ast::Character* character_ptr) {}

void CodeGenerator::visit(ast::Bool* bool_ptr) {}

#pragma GCC diagnostic pop
