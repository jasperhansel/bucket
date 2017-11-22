#pragma once
#include "miscellaneous/common.hxx"
#include "abstract_syntax_tree/abstract_syntax_tree.hxx"
#include "frontend/lexer.hxx"
#include "frontend/token.hxx"
#include <memory>
#include <string>


namespace frontend {


class Parser {

public:

  explicit Parser(const char* path);

  ast::Program parse();

private:

  Lexer lexer;

  std::unique_ptr<ast::GlobalStatement> parseGlobalStatement();

  std::unique_ptr<ast::Class> parseClassDefinition();

  std::unique_ptr<ast::Method> parseMethodDefinition();

  std::unique_ptr<ast::Field> parseMemberVariable();

  std::unique_ptr<ast::Statement> parseStatement();

  std::unique_ptr<ast::If> parseIf();

  std::unique_ptr<ast::Loop> parseLoop();

  std::unique_ptr<ast::Expression> parseExpression();

  std::unique_ptr<ast::Expression> parseOrExpression();

  std::unique_ptr<ast::Expression> parseAndExpression();

  std::unique_ptr<ast::Expression> parseEqualityExpression();

  std::unique_ptr<ast::Expression> parseComparisonExpression();

  std::unique_ptr<ast::Expression> parseArithmeticExpression();

  std::unique_ptr<ast::Expression> parseTerm();

  std::unique_ptr<ast::Expression> parseFactor();

  std::unique_ptr<ast::Expression> parseExponent();

  std::unique_ptr<ast::Expression> parsePostfixExpression();

  bool parseMethodOrAccess(std::unique_ptr<ast::Expression>& expression);

  bool parseCall(std::unique_ptr<ast::Expression>& expression);

  bool parseIndex(std::unique_ptr<ast::Expression>& expression);

  std::unique_ptr<ast::Expression> parseSimpleExpression();

  std::unique_ptr<ast::Expression> parseIdentifier();

  std::unique_ptr<ast::Expression> parseLiteral();

  std::string getIdentifierString();

  std::string expectIdentifier();

  bool accept(Keyword keyword);

  bool accept(Symbol symbol);

  void expect(Keyword keyword);

  void expect(Symbol symbol);

};


}
