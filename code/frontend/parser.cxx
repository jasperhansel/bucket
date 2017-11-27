#include "common.hxx"
#include "frontend/parser.hxx"
#include "support/concatenate.hxx"
#include <stdexcept>
#include <iostream>
using namespace frontend;


Parser::Parser(const char* path)
: lexer(path)
{}


std::unique_ptr<ast::Class> Parser::parse()
{
  auto program = std::make_unique<ast::Class>();
  program->name = "__module__";
  while (true) {
    if (auto ptr = parseGlobalStatement()) {
      program->body.push_back(std::move(ptr));
      continue;
    }
    if (accept(Symbol::Newline))
      continue;
    if (accept(Symbol::EndOfFile))
      break;
    throw std::runtime_error("expected global statement, newline, or end of file");
  }
  return program;
}


std::unique_ptr<ast::GlobalStatement> Parser::parseGlobalStatement()
{
  if (std::unique_ptr<ast::GlobalStatement> ptr;
    (ptr = parseClassDefinition()) ||
    (ptr = parseMethodDefinition()) ||
    (ptr = parseMemberVariable())
  ) return ptr;
  return nullptr;
}


std::unique_ptr<ast::Class> Parser::parseClassDefinition()
{
  if (!accept(Keyword::Class))
    return nullptr;
  auto class_definition = std::make_unique<ast::Class>();
  if ((class_definition->name = getIdentifierString()).empty())
    throw std::runtime_error("expected identifier after \'class\'");
  //if (accept(Symbol::Lesser)) {
  //  if (!(class_definition->base = parsePostfixExpression()))
  //    throw std::runtime_error("expected postfix expression after \'is\'");
  //}
  expect(Symbol::Newline);
  while (auto ptr = parseGlobalStatement())
    class_definition->body.push_back(std::move(ptr));
  expect(Keyword::End);
  expect(Symbol::Newline);
  return class_definition;
}


std::unique_ptr<ast::Method> Parser::parseMethodDefinition()
{
  if (!accept(Keyword::Method))
    return nullptr;
  auto method_definition = std::make_unique<ast::Method>();
  if ((method_definition->name = getIdentifierString()).empty())
    throw std::runtime_error("expected identifier after \'method\'");
  if (accept(Symbol::OpenParenthesis)) {
    if (!accept(Symbol::CloseParenthesis)) {
      do {
        auto arg_name = getIdentifierString();
        if (arg_name.empty())
          throw std::runtime_error("expected argument name");
        expect(Symbol::Colon);
        auto arg_class = parsePostfixExpression();
        if (!arg_class)
          throw std::runtime_error("expected class");
        method_definition->args.emplace_back(std::move(arg_name), std::move(arg_class));
      } while (accept(Symbol::Comma));
      expect(Symbol::CloseParenthesis);
    }
  }
  if (accept(Symbol::Colon)) {
    if (!(method_definition->return_class = parsePostfixExpression()))
      throw std::runtime_error("expected return type after arrow");
  }
  expect(Symbol::Newline);
  while (auto ptr = parseStatement())
    method_definition->body.push_back(std::move(ptr));
  expect(Keyword::End);
  if (!accept(Symbol::Newline) && !accept(Symbol::EndOfFile))
    throw std::runtime_error("expected newline or eof");
  return method_definition;
}


std::unique_ptr<ast::Field> Parser::parseMemberVariable()
{
  auto member_variable = std::make_unique<ast::Field>();
  if ((member_variable->name = getIdentifierString()).empty())
    return nullptr;
  expect(Symbol::Colon);
  if (!(member_variable->cls = parseExpression()))
    throw std::runtime_error("expected expression in member variable");
  expect(Symbol::Newline);
  return member_variable;
}


std::unique_ptr<ast::Statement> Parser::parseStatement()
{
  if (auto ptr = parseIf())
    return ptr;

  if (auto ptr = parseLoop())
    return ptr;

  if (accept(Keyword::Break)) {
    expect(Symbol::Newline);
    return std::make_unique<ast::Break>();
  }

  if (accept(Keyword::Cycle)) {
    expect(Symbol::Newline);
    return std::make_unique<ast::Cycle>();
  }

  if (accept(Keyword::Ret)) {
    auto ret = std::make_unique<ast::Ret>();
    ret->value = parseExpression();
    expect(Symbol::Newline);
    return ret;
  }

  if (auto ptr = parseExpression()) {
    expect(Symbol::Newline);
    auto expression_statement = std::make_unique<ast::ExpressionStatement>();
    expression_statement->value = std::move(ptr);
    return expression_statement;
  }

  return nullptr;

}


std::unique_ptr<ast::If> Parser::parseIf()
{
  if (!accept(Keyword::If))
    return nullptr;
  auto if_ = std::make_unique<ast::If>();
  if (!(if_->condition = parseExpression()))
    throw std::runtime_error("expected expression after \'if\'");
  expect(Symbol::Newline);
  while (auto statement = parseStatement())
    if_->if_body.push_back(std::move(statement));
  while (accept(Keyword::Elif)) {
    auto expression = parseExpression();
    if (!expression)
      throw std::runtime_error("expected expression after \'elif\'");
    expect(Symbol::Newline);
    std::vector<std::unique_ptr<ast::Statement>> statements;
    while (auto statement = parseStatement())
      statements.push_back(std::move(statement));
    if_->elif_bodies.emplace_back(std::move(expression), std::move(statements));
  }
  if (accept(Keyword::Else)) {
    expect(Symbol::Newline);
    while (auto statement = parseStatement())
      if_->else_body.push_back(std::move(statement));
  }
  expect(Keyword::End);
  return if_;
}


std::unique_ptr<ast::Loop> Parser::parseLoop()
{
  if (!accept(Keyword::Do))
    return nullptr;
  expect(Symbol::Newline);
  auto loop = std::make_unique<ast::Loop>();
  while (auto statement = parseStatement())
    loop->body.push_back(std::move(statement));
  expect(Keyword::End);
  expect(Symbol::Newline);
  return loop;
}


std::unique_ptr<ast::Expression> Parser::parseExpression()
{
  auto ptr = parseOrExpression();
  if (!ptr)
    return nullptr;
  if (accept(Symbol::SingleEquals)) {
    auto assignment = std::make_unique<ast::Assignment>();
    assignment->left = std::move(ptr);
    if (!(assignment->right = parseExpression()))
      throw std::runtime_error("expected expression on rhs");
    return assignment;
  }
  return ptr;
}


std::unique_ptr<ast::Expression> Parser::parseOrExpression()
{
  auto expression = parseAndExpression();
  if (!expression)
    return nullptr;
  if (!accept(Keyword::Or))
    return expression;
  auto call = std::make_unique<ast::Call>();
  call->object = std::move(expression);
  call->name = "__or__";
  expression = parseOrExpression();
  if (!expression)
    throw std::runtime_error("expected expression after 'or'");
  call->args.push_back(std::move(expression));
  return call;
}


std::unique_ptr<ast::Expression> Parser::parseAndExpression()
{
  auto expression = parseEqualityExpression();
  if (!expression)
    return nullptr;
  if (!accept(Keyword::Or))
    return expression;
  auto call = std::make_unique<ast::Call>();
  call->object = std::move(expression);
  call->name = "__and__";
  expression = parseAndExpression();
  if (!expression)
    throw std::runtime_error("expected expression after 'and'");
  call->args.push_back(std::move(expression));
  return call;
}


std::unique_ptr<ast::Expression> Parser::parseEqualityExpression()
{
  auto expression = parseComparisonExpression();
  if (!expression)
    return nullptr;
  std::string name;
  if (accept(Symbol::DoubleEquals))
    name = "__eq__";
  else if (accept(Symbol::BangEquals))
    name = "__ne__";
  else
    return expression;
  auto call = std::make_unique<ast::Call>();
  call->object = std::move(expression);
  call->name = std::move(name);
  expression = parseComparisonExpression();
  if (!expression)
    throw std::runtime_error("expected expression after equality");
  call->args.push_back(std::move(expression));
  return call;
}


std::unique_ptr<ast::Expression> Parser::parseComparisonExpression()
{
  auto expression = parseArithmeticExpression();
  if (!expression)
    return nullptr;
  std::string name;
  if (accept(Symbol::Greater))
    name = "__gt__";
  else if (accept(Symbol::GreaterOrEqual))
    name = "__ge__";
  else if (accept(Symbol::Lesser))
    name = "__lt__";
  else if (accept(Symbol::LesserOrEqual))
    name = "__le__";
  else
    return expression;
  auto call = std::make_unique<ast::Call>();
  call->object = std::move(expression);
  call->name = std::move(name);
  expression = parseArithmeticExpression();
  if (!expression)
    throw std::runtime_error("expected expression after equality");
  call->args.push_back(std::move(expression));
  return call;
}


std::unique_ptr<ast::Expression> Parser::parseArithmeticExpression()
{
  auto expression = parseTerm();
  if (!expression)
    return nullptr;
  std::string name;
  if (accept(Symbol::Plus))
    name = "__add__";
  else if (accept(Symbol::Minus))
    name = "__sub__";
  else
    return expression;
  auto call = std::make_unique<ast::Call>();
  call->object = std::move(expression);
  call->name = std::move(name);
  expression = parseTerm();
  if (!expression)
    throw std::runtime_error("expected expression after plus/minus");
  call->args.push_back(std::move(expression));
  while (true) {
    if (accept(Symbol::Plus))
      name = "__add__";
    else if (accept(Symbol::Minus))
      name = "__sub__";
    else
      return std::move(call);
    auto new_call = std::make_unique<ast::Call>();
    new_call->object = std::move(call);
    new_call->name = std::move(name);
    expression = parseTerm();
    if (!expression)
      throw std::runtime_error("expected expression after plus/minus");
    new_call->args.push_back(std::move(expression));
    call = std::move(new_call);
  }
}


std::unique_ptr<ast::Expression> Parser::parseTerm()
{
  auto expression = parseFactor();
  if (!expression)
    return nullptr;
  std::string name;
  if (accept(Symbol::Asterisk))
    name = "__mul__";
  else if (accept(Symbol::Slash))
    name = "__div__";
  else if (accept(Symbol::PercentSign))
    name = "__mod__";
  else
    return expression;
  auto call = std::make_unique<ast::Call>();
  call->object = std::move(expression);
  call->name = std::move(name);
  expression = parseFactor();
  if (!expression)
    throw std::runtime_error("expected expression after times/divide/modulo");
  call->args.push_back(std::move(expression));
  while (true) {
    if (accept(Symbol::Asterisk))
      name = "__mul__";
    else if (accept(Symbol::Slash))
      name = "__div__";
    else if (accept(Symbol::PercentSign))
      name = "__mod__";
    else
      return call;
    auto new_call = std::make_unique<ast::Call>();
    new_call->object = std::move(call);
    new_call->name = std::move(name);
    expression = parseTerm();
    if (!expression)
      throw std::runtime_error("expected expression after plus/minus");
    new_call->args.push_back(std::move(expression));
    call = std::move(new_call);
  }
}


std::unique_ptr<ast::Expression> Parser::parseFactor()
{
  std::string name;
  if (accept(Symbol::Plus))
    name = "__pos__";
  else if (accept(Symbol::Minus))
    name = "__neg__";
  else if (accept(Keyword::Not))
    name = "__not__";
  else if (accept(Symbol::Asterisk))
    name = "__dereference__";
  else if (accept(Symbol::Ampersand))
    name = "__addressof__";
  else
    return parseExponent();
  auto call = std::make_unique<ast::Call>();
  if (!(call->object = parseFactor()))
    throw std::runtime_error("I'm too lazy to keep writing error messages");
  call->name = std::move(name);
  return call;
}


std::unique_ptr<ast::Expression> Parser::parseExponent()
{
  auto expression = parsePostfixExpression();
  if (!expression)
    return nullptr;
  if (accept(Symbol::Caret)) {
    auto call = std::make_unique<ast::Call>();
    call->object = std::move(expression);
    call->name = "__pow__";
    if (!(expression = parseFactor()))
      throw std::runtime_error("foo");
    call->args.push_back(std::move(expression));
    return call;
  }
  return expression;
}


std::unique_ptr<ast::Expression> Parser::parsePostfixExpression()
{
  auto expression = parseSimpleExpression();
  while (true) {
    if (
      parseMethodOrAccess(expression) ||
      parseCall(expression) ||
      parseIndex(expression)
    ) continue;
    return expression;
  }
}


bool Parser::parseMethodOrAccess(std::unique_ptr<ast::Expression>& expression)
{
  if (!accept(Symbol::Period))
    return false;
  auto name = expectIdentifier();
  if (name.empty())
    throw std::runtime_error("expected identifier after '.'");
  auto call = std::make_unique<ast::Call>();
  call->name = std::move(name);
  if (accept(Symbol::OpenParenthesis)) {
    if (!accept(Symbol::CloseParenthesis)) {
      do {
        auto arg = parseExpression();
        if (!arg)
          throw std::runtime_error("expected argument");
        call->args.push_back(std::move(arg));
      } while (accept(Symbol::Comma));
      expect(Symbol::CloseParenthesis);
    }
  }
  call->object = std::move(expression);
  expression = std::move(call);
  return true;
}


bool Parser::parseCall(std::unique_ptr<ast::Expression>& expression)
{
  if (!accept(Symbol::OpenParenthesis))
    return false;
  auto call = std::make_unique<ast::Call>();
  call->name = "__call__";
  do {
    auto arg = parseExpression();
    if (!arg)
      throw std::runtime_error("expected argument");
    call->args.push_back(std::move(arg));
  } while (accept(Symbol::Comma));
  expect(Symbol::CloseParenthesis);
  call->object = std::move(expression);
  expression = std::move(call);
  return true;
}


bool Parser::parseIndex(std::unique_ptr<ast::Expression>& expression)
{
  if (!accept(Symbol::OpenSquareBracket))
    return false;
  auto call = std::make_unique<ast::Call>();
  call->name = "__index__";
  do {
    auto arg = parseExpression();
    if (!arg)
      throw std::runtime_error("expected argument");
    call->args.push_back(std::move(arg));
  } while (accept(Symbol::Comma));
  expect(Symbol::CloseSquareBracket);
  call->object = std::move(expression);
  expression = std::move(call);
  return true;
}


std::unique_ptr<ast::Expression> Parser::parseSimpleExpression()
{
  if (auto expression = parseIdentifier())
    return expression;
  if (auto literal = parseLiteral())
    return literal;
  if (accept(Symbol::OpenParenthesis)) {
    auto expression = parseExpression();
    if (!expression)
      throw std::runtime_error("expected expression in parenthesis");
    expect(Symbol::CloseParenthesis);
    return expression;
  }
  return nullptr;
}


std::unique_ptr<ast::Expression> Parser::parseIdentifier()
{
  auto identifier_string = getIdentifierString();
  if (identifier_string.empty())
    return nullptr;
  if (accept(Symbol::OpenParenthesis)) {
    auto call = std::make_unique<ast::Call>();
    auto obj = std::make_unique<ast::Identifier>();
    obj->value = "__self__";
    call->object = std::move(obj);
    call->name = std::move(identifier_string);
    if (!accept(Symbol::CloseParenthesis)) {
      do {
        auto arg = parseExpression();
        if (!arg)
          throw std::runtime_error("expected argument");
        call->args.push_back(std::move(arg));
      } while (accept(Symbol::Comma));
      expect(Symbol::CloseParenthesis);
    }
    return call;
  }
  auto identifier = std::make_unique<ast::Identifier>();
  identifier->value = std::move(identifier_string);
  return identifier;
}


std::unique_ptr<ast::Expression> Parser::parseLiteral()
{
  if (auto ptr = lexer.current().getIntegerLiteral()) {
    auto integer = std::make_unique<ast::Integer>();
    integer->value = *ptr;
    lexer.next();
    return integer;
  }
  if (auto ptr = lexer.current().getRealLiteral()) {
    auto real = std::make_unique<ast::Real>();
    real->value = *ptr;
    lexer.next();
    return real;
  }
  if (auto ptr = lexer.current().getStringLiteral()) {
    auto str = std::make_unique<ast::String>();
    str->value = std::move(*ptr);
    lexer.next();
    return str;
  }
  if (auto ptr = lexer.current().getCharacterLiteral()) {
    auto character = std::make_unique<ast::Character>();
    character->value = *ptr;
    lexer.next();
    return character;
  }
  if (auto ptr = lexer.current().getBooleanLiteral()) {
    auto boolean = std::make_unique<ast::Bool>();
    boolean->value = *ptr;
    lexer.next();
    return boolean;
  }
  return nullptr;
}


std::string Parser::getIdentifierString()
{
  if (auto ptr = lexer.current().getIdentifier()) {
    auto str = std::move(*ptr);
    lexer.next();
    return str;
  }
  return "";
}


std::string Parser::expectIdentifier()
{
  auto ptr = lexer.current().getIdentifier();
  if (!ptr)
    throw std::runtime_error("expected identifier");
  auto result = std::move(*ptr);
  lexer.next();
  return result;
}


void Parser::expect(Keyword keyword)
{
  if (!accept(keyword))
    throw std::runtime_error(support::concatenate("expected keyword '", keywordToString(keyword), '\''));
}


void Parser::expect(Symbol symbol)
{
  if (!accept(symbol)) {
    std::cerr << lexer.current();
    throw std::runtime_error(support::concatenate("expected symbol '", symbolToString(symbol), "' (~line ", lexer.current().begin.line, ")"));
  }
}


bool Parser::accept(Keyword keyword)
{
  if (auto ptr = lexer.current().getKeyword(); ptr && *ptr == keyword) {
    lexer.next();
    return true;
  }
  return false;
}


bool Parser::accept(Symbol symbol)
{
  if (auto ptr = lexer.current().getSymbol(); ptr && *ptr == symbol) {
    lexer.next();
    return true;
  }
  return false;
}
