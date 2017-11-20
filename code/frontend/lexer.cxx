#include "miscellaneous/common.hxx"
#include "frontend/lexer.hxx"
#include "frontend/source_file.hxx"
#include "frontend/token.hxx"
#include "miscellaneous/concatenate.hxx"
#include <cassert>
#include <cctype>
#include <cstring>
#include <stdexcept>
using namespace frontend;


Lexer::Lexer(const char* path)
: source_file(path)
{
  next();
}


Token& Lexer::current()
{
  return current_token;
}


void Lexer::next()
{
  switch (source_file.current()) {

    case '\n':
      lexSymbol(Symbol::Newline);
      break;

    case '\t':
    case '\v':
    case '\f':
    case ' ':
      source_file.next();
      next();
      break;

    case '!':
      {
        auto begin = source_file.position();
        source_file.next();
        if (source_file.current() != '=')
          throw std::runtime_error(misc::concatenate("expected \'=\' after \'!\' (line ", source_file.position().line, ", column ", source_file.position().column, ')'));
        current_token = Token::symbol(begin, source_file.position(), Symbol::BangEquals);
        source_file.next();
        break;
      }

    case '"':
      lexStringLiteral();
      break;

    case '#':
    case '$':
    case ';':
    case '?':
    case '@':
    case '`':
    case '{':
    case '|':
    case '}':
    case '~':
      throw std::runtime_error("unexpected character in program");

    case '%':
      lexSymbol(Symbol::PercentSign);
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
        auto begin = source_file.position();
        source_file.next();
        if (source_file.current() == '=') {
          symbol = Symbol::LesserOrEqual;
          source_file.next();
        }
        else
          symbol = Symbol::Lesser;
        current_token = Token::symbol(begin, source_file.position(), symbol);
        break;
      }
    case '=':
      {
        Symbol symbol;
        auto begin = source_file.position();
        source_file.next();
        if (source_file.current() == '=') {
          symbol = Symbol::DoubleEquals;
          source_file.next();
        }
        else
          symbol = Symbol::SingleEquals;
        current_token = Token::symbol(begin, source_file.position(), symbol);
        break;
      }
    case '>':
      {
        Symbol symbol;
        auto begin = source_file.position();
        source_file.next();
        if (source_file.current() == '=') {
          symbol = Symbol::GreaterOrEqual;
          source_file.next();
        }
        else
          symbol = Symbol::Greater;
        current_token = Token::symbol(begin, source_file.position(), symbol);
        break;
      }

    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
      lexIdentifierOrKeyword();
      break;

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

    case SourceFile::eof:
      lexSymbol(Symbol::EndOfFile);
      break;

    default:
      throw std::runtime_error(misc::concatenate("character not from bucket character set"));

  }
}


void Lexer::lexSymbol(Symbol symbol)
{
  auto begin = source_file.position();
  source_file.next();
  current_token = Token::symbol(begin, source_file.position(), symbol);
}


void Lexer::lexStringLiteral()
{
  auto begin = source_file.position();
  assert(source_file.current() == '"');
  source_file.next();
  std::string s;
  while (source_file.current() != '"') {
    if (source_file.current() == SourceFile::eof)
      throw std::runtime_error(misc::concatenate("string literal starting on line ", source_file.position().line, ", column ", source_file.position().column, " not closed"));
    char character;
    if (source_file.current() == '\\') {
      source_file.next();
      switch (source_file.current()) {
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
          throw std::runtime_error(misc::concatenate("invalid escape sequence (line ", source_file.position().line, ", column ", source_file.position().column - 1, ")"));
      }
    }
    else {
      character = static_cast<char>(source_file.current());
      static char character_set[] = "\t\n\v\f !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~";
      if (!std::strchr(character_set, character))
        throw std::runtime_error(misc::concatenate("invalid character in source file (line ", source_file.position().line, ", column ", source_file.position().column - 1, ")"));
    }
    s += character;
    source_file.next();
  }
  source_file.next();
  current_token = Token::stringLiteral(begin, source_file.position(), move(s));
}


void Lexer::lexCharacterLiteral()
{
  auto begin = source_file.position();
  assert(source_file.current() == '\'');
  source_file.next();
  char character;
  if (source_file.current() == '\'')
    throw std::runtime_error(misc::concatenate("empty character literal (", source_file.position().line, ", column ", source_file.position().column, ")"));
  if (source_file.current() == '\\') {
    source_file.next();
    switch (source_file.current()) {
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
        throw std::runtime_error(misc::concatenate("invalid escape sequence (line ", source_file.position().line, ", column ", source_file.position().column - 1, ")"));
    }
  }
  else {
    character = static_cast<char>(source_file.current());
    static char character_set[] = "\t\n\v\f !\"#$%&()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    if (!std::strchr(character_set, character))
      throw std::runtime_error(misc::concatenate("invalid character in source file (line ", source_file.position().line, ", column ", source_file.position().column - 1, ")"));
  }
  source_file.next();
  if (source_file.current() != '\'')
    throw std::runtime_error(misc::concatenate("character literal has multiple characters (line ", source_file.position().line, ", column ", source_file.position().column, ")"));
  source_file.next();
  current_token = Token::characterLiteral(begin, source_file.position(), character);
}


static bool isIdentifierCharacter(int character)
{
  switch (character) {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
      return true;
    default:
      return false;
  }
}


void Lexer::lexNumberOrPeriod()
{
  auto begin = source_file.position();
  std::string number;
  if (source_file.current() == '.') {
    source_file.next();
    if (!std::isdigit(source_file.current())) {
      current_token = Token::symbol(begin, source_file.position(), Symbol::Period);
      return;
    }
    number += '.';
    do {
      number += static_cast<char>(source_file.current());
      source_file.next();
    } while (std::isdigit(source_file.current()));
  }
  else {
    do {
      number += static_cast<char>(source_file.current());
      source_file.next();
    } while (std::isdigit(source_file.current()));
    if (source_file.current() == '.') {
      do {
        number += static_cast<char>(source_file.current());
        source_file.next();
      } while (std::isdigit(source_file.current()));
      current_token = Token();
    }
    else if (source_file.current() != 'e' && source_file.current() != 'E') {
      if (isIdentifierCharacter(source_file.current()))
        throw std::runtime_error("letter in number literal");
      current_token = Token::integerLiteral(begin, source_file.position(), stoul(number));
      return;
    }
  }
  if (source_file.current() == 'e' || source_file.current() == 'E') {
    number += static_cast<char>(source_file.current());
    source_file.next();
    if (source_file.current() == '+' || source_file.current() == '-') {
      number += static_cast<char>(source_file.current());
      source_file.next();
    }
    if (!std::isdigit(source_file.current()))
      throw std::runtime_error(misc::concatenate("expected number in real literal exponent (", source_file.position().line, ", column ", source_file.position().column, ')'));
    do {
      number += static_cast<char>(source_file.current());
      source_file.next();
    } while (std::isdigit(source_file.current()));
  }
  if (isIdentifierCharacter(source_file.current()))
    throw std::runtime_error(misc::concatenate("letter in number literal (line ", source_file.position().line, ", column ", source_file.position().column, ')'));
  current_token = Token::realLiteral(begin, source_file.position(), stod(number));
}


void Lexer::lexSlash()
{
  auto begin = source_file.position();
  source_file.next();
  if (source_file.current() == '/') {
    do
      source_file.next();
    while (source_file.current() != '\n' && source_file.current() != SourceFile::eof);
    next();
  }
  else if (source_file.current() == '*') {
    source_file.next();
    long long depth = 1;
    do {
      source_file.next();
      if (source_file.current() == SourceFile::eof)
        throw std::runtime_error(misc::concatenate("block comment starting on line ", source_file.position().line, ", column ", source_file.position().column, " not closed"));
      if (source_file.current() == '*') {
        source_file.next();
        if (source_file.current() == '/')
          depth--;
      }
      else if (source_file.current() == '/') {
        source_file.next();
        if (source_file.current() == '*')
          depth++;
      }
    } while (depth > 0);
    source_file.next();
    next();
  }
  else
    current_token = Token::symbol(begin, source_file.position(), Symbol::Slash);
}


static bool isIdentifierCharacterOrDigit(int character)
{
  switch (character) {
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
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
      return true;
    default:
      return false;
  }
}


void Lexer::lexIdentifierOrKeyword()
{
  auto begin = source_file.position();
  std::string word;
  do {
    word += static_cast<char>(source_file.current());
    source_file.next();
  } while (isIdentifierCharacterOrDigit(source_file.current()));
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
    current_token = Token::booleanLiteral(begin, source_file.position(), true);
    return;
  }
  else if (word == "false") {
    current_token = Token::booleanLiteral(begin, source_file.position(), false);
    return;
  }
  else {
    current_token = Token::identifier(begin, source_file.position(), move(word));
    return;
  }
  current_token = Token::keyword(begin, source_file.position(), keyword);
}
