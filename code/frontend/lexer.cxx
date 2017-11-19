#include "miscellaneous/common.hxx"
#include "frontend/lexer.hxx"
#include "frontend/source_file.hxx"
#include "frontend/token.hxx"
#include "miscellaneous/concatenate.hxx"
#include <cassert>
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
      throw std::runtime_error("unexpected character in program");

    case '\'':
      lexCharacterLiteral();
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
