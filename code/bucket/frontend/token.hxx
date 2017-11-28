#ifndef BUCKET_FRONTEND_TOKEN_HXX
#define BUCKET_FRONTEND_TOKEN_HXX

#include "common.hxx"
#include "frontend/sourcefile.hxx"
#include "support/unicodecharacter.hxx"
#include <ostream>
#include <string>
#include <string_view>
#include <variant>

namespace frontend {

enum class Keyword {
  End, If, Elif, Else, Do, For, Break, Cycle, Ret, And, Or, Not, Class, Method
};

std::string_view keywordToString(Keyword keyword) noexcept;

enum class Symbol {
  OpenParenthesis, CloseParenthesis, OpenSquareBracket, CloseSquareBracket,
  Plus, Minus, Asterisk, Slash, Caret, PercentSign, SingleEquals, DoubleEquals,
  BangEquals, Greater, GreaterOrEqual, Lesser, LesserOrEqual, Period, Comma,
  Colon, Ampersand, Newline, EndOfFile
};

std::string_view symbolToString(Symbol symbol) noexcept;

class Token {

public:

  SourceFile::Position begin, end;

  Token() = default;

  static Token identifier(SourceFile::Position begin, SourceFile::Position end, std::string&& identifier);
  static Token keyword(SourceFile::Position begin, SourceFile::Position end, Keyword keyword);
  static Token symbol(SourceFile::Position begin, SourceFile::Position end, Symbol symbol);
  static Token integerLiteral(SourceFile::Position begin, SourceFile::Position end, unsigned long integer_literal);
  static Token realLiteral(SourceFile::Position begin, SourceFile::Position end, double real_literal);
  static Token stringLiteral(SourceFile::Position begin, SourceFile::Position end, std::string&& string_literal);
  static Token characterLiteral(SourceFile::Position begin, SourceFile::Position end, support::UnicodeCharacter character_literal);
  static Token booleanLiteral(SourceFile::Position begin, SourceFile::Position end, bool boolean_literal);

  std::string* getIdentifier();
  Keyword* getKeyword();
  Symbol* getSymbol();
  unsigned long* getIntegerLiteral();
  double* getRealLiteral();
  std::string* getStringLiteral();
  support::UnicodeCharacter* getCharacterLiteral();
  bool* getBooleanLiteral();

  friend std::ostream& operator<<(std::ostream& stream, Token& token);

private:

  std::variant<
    std::monostate,             // Empty
    std::string,                // Identifier
    Keyword,                    // Keyword
    Symbol,                     // Symbol
    unsigned long,              // Integer Literal
    double,                     // Real Literal
    std::string,                // String Literal
    support::UnicodeCharacter,  // Character Literal
    bool                        // Boolean Literal
  > value;

  using VariantType = decltype(value);

  Token(SourceFile::Position begin, SourceFile::Position end, VariantType&& value);

};

}

#endif
