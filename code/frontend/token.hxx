#pragma once
#include "miscellaneous/common.hxx"
#include "frontend/source_file.hxx"
#include <ostream>
#include <string>
#include <utility>
#include <variant>


namespace frontend {


enum class Keyword {
  End, If, Elif, Else, Do, For, Break, Cycle, Ret, And, Or, Not, Class, Method
};


const char* keywordToString(Keyword keyword) noexcept;


enum class Symbol {
  OpenParenthesis, CloseParenthesis, OpenSquareBracket, CloseSquareBracket,
  Plus, Minus, Asterisk, Slash, Caret, PercentSign, SingleEquals, DoubleEquals,
  BangEquals, Greater, GreaterOrEqual, Lesser, LesserOrEqual, Period, Comma,
  Colon, Ampersand, Newline, EndOfFile
};


const char* symbolToString(Symbol symbol) noexcept;


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
  static Token characterLiteral(SourceFile::Position begin, SourceFile::Position end, char character_literal);
  static Token booleanLiteral(SourceFile::Position begin, SourceFile::Position end, bool boolean_literal);

  std::string* getIdentifier();
  Keyword* getKeyword();
  Symbol* getSymbol();
  unsigned long* getIntegerLiteral();
  double* getRealLiteral();
  std::string* getStringLiteral();
  char* getCharacterLiteral();
  bool* getBooleanLiteral();

private:

  std::variant<
    std::monostate, // Empty
    std::string,    // Identifier
    Keyword,        // Keyword
    Symbol,         // Symbol
    unsigned long,  // Integer Literal
    double,         // Real Literal
    std::string,    // String Literal
    char,           // Character Literal
    bool            // Boolean Literal
  > value;

  using VariantType = decltype(value);

  Token(SourceFile::Position begin, SourceFile::Position end, VariantType&& value);

};


std::ostream& operator<<(std::ostream& stream, Token& token);


}
