#include "common.hxx"
#include "frontend/lexer.hxx"
#include "support/concatenate.hxx"
#include <cassert>
#include <cctype>
#include <cstring>
#include <stdexcept>

namespace frontend {

Lexer::Lexer(const char* path)
: mSourceFile(path)
{
  next();
}

Token& Lexer::currentToken()
{
  return mCurrentToken;
}

void Lexer::next()
{
  if (mSourceFile.currentCharacter().isEndOfFile()) {
    lexSymbol(Symbol::EndOfFile);
    return;
  }

  if (mSourceFile.currentCharacter().isLetter() || mSourceFile.currentCharacter() == '_') {
    lexIdentifierOrKeyword();
    return;
  }

  auto ascii = mSourceFile.currentCharacter().getAscii();

  if (ascii == -1)
    throw std::runtime_error("unknown unicode character");

  switch (ascii) {

    case '\n':
      lexSymbol(Symbol::Newline);
      break;

    case '\t':
    case '\v':
    case '\f':
    case ' ':
      mSourceFile.next();
      next();
      break;

    case '!':
      {
        auto begin = mSourceFile.position();
        mSourceFile.next();
        if (mSourceFile.currentCharacter() != '=')
          throw std::runtime_error(support::concatenate("expected \'=\' after \'!\' (line ", mSourceFile.position().line, ", column ", mSourceFile.position().column, ')'));
        mCurrentToken = Token::symbol(begin, mSourceFile.position(), Symbol::BangEquals);
        mSourceFile.next();
        break;
      }

    case '"':
      lexStringLiteral();
      break;

    case '%':
      lexSymbol(Symbol::PercentSign);
      break;

    case '&':
      lexSymbol(Symbol::Ampersand);
      break;

    case '\'':
      lexCharacterLiteral();
      break;

    case '(':
      lexSymbol(Symbol::OpenParenthesis);
      break;

    case ')':
      lexSymbol(Symbol::CloseParenthesis);
      break;

    case '*':
      lexSymbol(Symbol::Asterisk);
      break;

    case '+':
      lexSymbol(Symbol::Plus);
      break;

    case ',':
      lexSymbol(Symbol::Comma);
      break;

    case '-':
      lexSymbol(Symbol::Minus);
      break;

    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      lexNumberOrPeriod();
      break;

    case '/':
      lexSlash();
      break;

    case ':':
      lexSymbol(Symbol::Colon);
      break;

    case '<':
      {
        Symbol symbol;
        auto begin = mSourceFile.position();
        mSourceFile.next();
        if (mSourceFile.currentCharacter() == '=') {
          symbol = Symbol::LesserOrEqual;
          mSourceFile.next();
        }
        else
          symbol = Symbol::Lesser;
        mCurrentToken = Token::symbol(begin, mSourceFile.position(), symbol);
        break;
      }
    case '=':
      {
        Symbol symbol;
        auto begin = mSourceFile.position();
        mSourceFile.next();
        if (mSourceFile.currentCharacter() == '=') {
          symbol = Symbol::DoubleEquals;
          mSourceFile.next();
        }
        else
          symbol = Symbol::SingleEquals;
        mCurrentToken = Token::symbol(begin, mSourceFile.position(), symbol);
        break;
      }
    case '>':
      {
        Symbol symbol;
        auto begin = mSourceFile.position();
        mSourceFile.next();
        if (mSourceFile.currentCharacter() == '=') {
          symbol = Symbol::GreaterOrEqual;
          mSourceFile.next();
        }
        else
          symbol = Symbol::Greater;
        mCurrentToken = Token::symbol(begin, mSourceFile.position(), symbol);
        break;
      }

    case '[':
      lexSymbol(Symbol::OpenSquareBracket);
      break;

    case '\\':
    case ']':
      lexSymbol(Symbol::CloseSquareBracket);
      break;

    case '^':
      lexSymbol(Symbol::Caret);
      break;

    default:
      throw std::runtime_error(support::concatenate("unexpected character"));

  }
}

void Lexer::lexSymbol(Symbol symbol)
{
  auto begin = mSourceFile.position();
  mSourceFile.next();
  mCurrentToken = Token::symbol(begin, mSourceFile.position(), symbol);
}

void Lexer::lexStringLiteral()
{
  auto begin = mSourceFile.position();
  assert(mSourceFile.currentCharacter() == '"');
  mSourceFile.next();
  std::string s;
  while (mSourceFile.currentCharacter() != '"') {
    if (mSourceFile.currentCharacter().isEndOfFile())
      throw std::runtime_error(support::concatenate("string literal starting on line ", mSourceFile.position().line, ", column ", mSourceFile.position().column, " not closed"));
    support::UnicodeCharacter character;
    if (mSourceFile.currentCharacter() == '\\') {
      mSourceFile.next();
      auto ascii = mSourceFile.currentCharacter().getAscii();
      if (ascii == -1)
        throw std::runtime_error("invalid escape sequence");
      switch (ascii) {
        case 'a':
          character = '\a';
          break;
        case 'b':
          character = '\b';
          break;
        case 'f':
          character = '\f';
          break;
        case 'n':
          character = '\n';
          break;
        case 'r':
          character = '\r';
          break;
        case 't':
          character = '\t';
          break;
        case 'v':
          character = '\v';
          break;
        case '\\':
          character = '\\';
          break;
        case '\'':
          character = '\'';
          break;
        case '"':
          character = '"';
          break;
        default:
          throw std::runtime_error(support::concatenate("invalid escape sequence (line ", mSourceFile.position().line, ", column ", mSourceFile.position().column - 1, ")"));
      }
    }
    else
      character = mSourceFile.currentCharacter();
    s += character;
    mSourceFile.next();
  }
  mSourceFile.next();
  mCurrentToken = Token::stringLiteral(begin, mSourceFile.position(), move(s));
}

void Lexer::lexCharacterLiteral()
{
  auto begin = mSourceFile.position();
  assert(mSourceFile.currentCharacter() == '\'');
  mSourceFile.next();
  support::UnicodeCharacter character;
  if (mSourceFile.currentCharacter() == '\'')
    throw std::runtime_error(support::concatenate("empty character literal (", mSourceFile.position().line, ", column ", mSourceFile.position().column, ")"));
  if (mSourceFile.currentCharacter() == '\\') {
    mSourceFile.next();
    auto ascii = mSourceFile.currentCharacter().getAscii();
    if (ascii == -1)
      throw std::runtime_error("invalid escape sequence");
    switch (ascii) {
      case 'a':
        character = '\a';
        break;
      case 'b':
        character = '\b';
        break;
      case 'f':
        character = '\f';
        break;
      case 'n':
        character = '\n';
        break;
      case 'r':
        character = '\r';
        break;
      case 't':
        character = '\t';
        break;
      case 'v':
        character = '\v';
        break;
      case '\\':
        character = '\\';
        break;
      case '\'':
        character = '\'';
        break;
      case '"':
        character = '"';
        break;
      default:
        throw std::runtime_error(support::concatenate("invalid escape sequence (line ", mSourceFile.position().line, ", column ", mSourceFile.position().column - 1, ")"));
    }
  }
  else
    character = mSourceFile.currentCharacter();
  mSourceFile.next();
  if (mSourceFile.currentCharacter() != '\'')
    throw std::runtime_error(support::concatenate("character literal has multiple characters (line ", mSourceFile.position().line, ", column ", mSourceFile.position().column, ")"));
  mSourceFile.next();
  mCurrentToken = Token::characterLiteral(begin, mSourceFile.position(), character);
}

void Lexer::lexNumberOrPeriod()
{
  auto begin = mSourceFile.position();
  std::string number;
  if (mSourceFile.currentCharacter() == '.') {
    mSourceFile.next();
    if (!mSourceFile.currentCharacter().isAsciiDigit()) {
      mCurrentToken = Token::symbol(begin, mSourceFile.position(), Symbol::Period);
      return;
    }
    number += '.';
    do {
      number += mSourceFile.currentCharacter();
      mSourceFile.next();
    } while (mSourceFile.currentCharacter().isAsciiDigit());
  }
  else {
    do {
      number += mSourceFile.currentCharacter();
      mSourceFile.next();
    } while (mSourceFile.currentCharacter().isAsciiDigit());
    if (mSourceFile.currentCharacter() == '.') {
      do {
        number += mSourceFile.currentCharacter();
        mSourceFile.next();
      } while (mSourceFile.currentCharacter().isAsciiDigit());
      mCurrentToken = Token();
    }
    else if (mSourceFile.currentCharacter() != 'e' && mSourceFile.currentCharacter() != 'E') {
      if (mSourceFile.currentCharacter().isLetter() || mSourceFile.currentCharacter() == '_')
        throw std::runtime_error("letter in number literal");
      mCurrentToken = Token::integerLiteral(begin, mSourceFile.position(), std::stoul(number));
      return;
    }
  }
  if (mSourceFile.currentCharacter() == 'e' || mSourceFile.currentCharacter() == 'E') {
    number += mSourceFile.currentCharacter();
    mSourceFile.next();
    if (mSourceFile.currentCharacter() == '+' || mSourceFile.currentCharacter() == '-') {
      number += mSourceFile.currentCharacter();
      mSourceFile.next();
    }
    if (!mSourceFile.currentCharacter().isAsciiDigit())
      throw std::runtime_error(support::concatenate("expected number in real literal exponent (", mSourceFile.position().line, ", column ", mSourceFile.position().column, ')'));
    do {
      number += mSourceFile.currentCharacter();
      mSourceFile.next();
    } while (mSourceFile.currentCharacter().isAsciiDigit());
  }
  if (mSourceFile.currentCharacter().isLetter() || mSourceFile.currentCharacter() == '_')
    throw std::runtime_error(support::concatenate("letter in number literal (line ", mSourceFile.position().line, ", column ", mSourceFile.position().column, ')'));
  mCurrentToken = Token::realLiteral(begin, mSourceFile.position(), std::stod(number));
}

void Lexer::lexSlash()
{
  auto begin = mSourceFile.position();
  mSourceFile.next();
  if (mSourceFile.currentCharacter() == '/') {
    do
      mSourceFile.next();
    while (mSourceFile.currentCharacter() != '\n' && !mSourceFile.currentCharacter().isEndOfFile());
    next();
  }
  else if (mSourceFile.currentCharacter() == '*') {
    mSourceFile.next();
    long long depth = 1;
    do {
      mSourceFile.next();
      if (mSourceFile.currentCharacter().isEndOfFile())
        throw std::runtime_error(support::concatenate("block comment starting on line ", mSourceFile.position().line, ", column ", mSourceFile.position().column, " not closed"));
      if (mSourceFile.currentCharacter() == '*') {
        mSourceFile.next();
        if (mSourceFile.currentCharacter() == '/')
          depth--;
      }
      else if (mSourceFile.currentCharacter() == '/') {
        mSourceFile.next();
        if (mSourceFile.currentCharacter() == '*')
          depth++;
      }
    } while (depth > 0);
    mSourceFile.next();
    next();
  }
  else
    mCurrentToken = Token::symbol(begin, mSourceFile.position(), Symbol::Slash);
}

void Lexer::lexIdentifierOrKeyword()
{
  auto begin = mSourceFile.position();
  std::string word;
  do {
    word += mSourceFile.currentCharacter();
    mSourceFile.next();
  } while (mSourceFile.currentCharacter().isNumericDigit() || mSourceFile.currentCharacter().isLetter() || mSourceFile.currentCharacter() == '_');
  Keyword keyword;
  if (word == "end")
    keyword = Keyword::End;
  else if (word == "if")
    keyword = Keyword::If;
  else if (word == "elif")
    keyword = Keyword::Elif;
  else if (word == "else")
    keyword = Keyword::Else;
  else if (word == "do")
    keyword = Keyword::Do;
  else if (word == "for")
    keyword = Keyword::For;
  else if (word == "break")
    keyword = Keyword::Break;
  else if (word == "cycle")
    keyword = Keyword::Cycle;
  else if (word == "ret")
    keyword = Keyword::Ret;
  else if (word == "and")
    keyword = Keyword::And;
  else if (word == "or")
    keyword = Keyword::Or;
  else if (word == "not")
    keyword = Keyword::Not;
  else if (word == "class")
    keyword = Keyword::Class;
  else if (word == "method")
    keyword = Keyword::Method;
  else if (word == "true") {
    mCurrentToken = Token::booleanLiteral(begin, mSourceFile.position(), true);
    return;
  }
  else if (word == "false") {
    mCurrentToken = Token::booleanLiteral(begin, mSourceFile.position(), false);
    return;
  }
  else {
    mCurrentToken = Token::identifier(begin, mSourceFile.position(), move(word));
    return;
  }
  mCurrentToken = Token::keyword(begin, mSourceFile.position(), keyword);
}

}
