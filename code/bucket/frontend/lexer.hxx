#ifndef BUCKET_FRONTEND_LEXER_HXX
#define BUCKET_FRONTEND_LEXER_HXX

#include "common.hxx"
#include "frontend/sourcefile.hxx"
#include "frontend/token.hxx"

namespace frontend {

class Lexer {

public:

  explicit Lexer(const char* path);

  Token& currentToken();

  void next();

private:

  SourceFile mSourceFile;

  Token mCurrentToken;

  void lexSymbol(Symbol symbol);

  void lexStringLiteral();

  void lexCharacterLiteral();

  void lexNumberOrPeriod();

  void lexSlash();

  void lexIdentifierOrKeyword();

};

}

#endif
