#include "miscellaneous/common.hxx"
#include "frontend/lexer.hxx"
#include "frontend/source_file.hxx"
#include "frontend/token.hxx"
#include "miscellaneous/concatenate.hxx"
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
