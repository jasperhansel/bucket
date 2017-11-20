#pragma once
#include "miscellaneous/common.hxx"
#include "frontend/source_file.hxx"
#include "frontend/token.hxx"


namespace frontend {


class Lexer {

public:

  explicit Lexer(const char* path);

  Token& current();

  void next();

private:

  SourceFile source_file;

  Token current_token;

  void lexSymbol(Symbol symbol);

  void lexStringLiteral();

  void lexCharacterLiteral();

  void lexNumberOrPeriod();

  void lexSlash();

  void lexIdentifierOrKeyword();

};


}
