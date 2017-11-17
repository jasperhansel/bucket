#include "frontend/token.hxx"
using namespace frontend;


static const char* keywordToString(Keyword keyword)
{
  switch (keyword) {
    case Keyword::End:    return "end";
    case Keyword::If:     return "if";
    case Keyword::Elif:   return "elif";
    case Keyword::Else:   return "else";
    case Keyword::Do:     return "do";
    case Keyword::For:    return "for";
    case Keyword::Break:  return "break";
    case Keyword::Cycle:  return "cycle";
    case Keyword::Ret:    return "ret";
    case Keyword::And:    return "and";
    case Keyword::Or:     return "or";
    case Keyword::Not:    return "not";
    case Keyword::Class:  return "class";
    case Keyword::Method: return "method";
  }
}


static const char* symbolToString(Symbol symbol)
{
  switch (symbol) {
    case Symbol::OpenParenthesis:     return "(";
    case Symbol::CloseParenthesis:    return ")";
    case Symbol::OpenSquareBracket:   return "[";
    case Symbol::CloseSquareBracket:  return "]";
    case Symbol::Plus:                return "+";
    case Symbol::Minus:               return "-";
    case Symbol::Asterisk:            return "*";
    case Symbol::Slash:               return "/";
    case Symbol::Caret:               return "^";
    case Symbol::PercentSign:         return "%";
    case Symbol::SingleEquals:        return "=";
    case Symbol::DoubleEquals:        return "==";
    case Symbol::BangEquals:          return "!=";
    case Symbol::Greater:             return ">";
    case Symbol::GreaterOrEqual:      return ">=";
    case Symbol::Lesser:              return "<";
    case Symbol::LesserOrEqual:       return "<=";
    case Symbol::Period:              return ".";
    case Symbol::Comma:               return ",";
    case Symbol::Colon:               return ":";
    case Symbol::Newline:             return "newline";
    case Symbol::EndOfFile:           return "end-of-file";
  }
}


Token Token::identifier(SourceFile::Position begin, SourceFile::Position end, std::string&& identifier)
{
  return Token(begin, end, VariantType(std::in_place_index_t<1>(), std::move(identifier)));
}


Token Token::keyword(SourceFile::Position begin, SourceFile::Position end, Keyword keyword)
{
  return Token(begin, end, VariantType(std::in_place_index_t<2>(), keyword));
}


Token Token::symbol(SourceFile::Position begin, SourceFile::Position end, Symbol symbol)
{
  return Token(begin, end, VariantType(std::in_place_index_t<3>(), symbol));
}


Token Token::integerLiteral(SourceFile::Position begin, SourceFile::Position end, unsigned long integer_literal)
{
  return Token(begin, end, VariantType(std::in_place_index_t<4>(), integer_literal));
}


Token Token::realLiteral(SourceFile::Position begin, SourceFile::Position end, double real_literal)
{
  return Token(begin, end, VariantType(std::in_place_index_t<5>(), real_literal));
}


Token Token::stringLiteral(SourceFile::Position begin, SourceFile::Position end, std::string&& string_literal)
{
  return Token(begin, end, VariantType(std::in_place_index_t<6>(), std::move(string_literal)));
}


Token Token::booleanLiteral(SourceFile::Position begin, SourceFile::Position end, bool boolean_literal)
{
  return Token(begin, end, VariantType(std::in_place_index_t<7>(), boolean_literal));
}


std::string* Token::getIdentifier()
{
  return std::get_if<1>(&value);
}


Keyword* Token::getKeyword()
{
  return std::get_if<2>(&value);
}


Symbol* Token::getSymbol()
{
  return std::get_if<3>(&value);
}


unsigned long* Token::getIntegerLiteral()
{
  return std::get_if<4>(&value);
}


double* Token::getRealLiteral()
{
  return std::get_if<5>(&value);
}


std::string* Token::getStringLiteral()
{
  return std::get_if<6>(&value);
}


bool* Token::getBooleanLiteral()
{
  return std::get_if<7>(&value);
}


Token::Token(SourceFile::Position begin, SourceFile::Position end, VariantType&& value)
: begin(begin),
  end(end),
  value(std::move(value))
{}


std::ostream& frontend::operator<<(std::ostream& stream, Token& token)
{
  if (auto identifier_ptr = token.getIdentifier())
    stream << '<' << *identifier_ptr << '>';

  else if (auto keyword_ptr = token.getKeyword())
    stream << "<\033[1m" << keywordToString(*keyword_ptr) << "\033[0m>";

  else if (auto symbol_ptr = token.getSymbol())
    stream << "<\033[1m" << symbolToString(*symbol_ptr) << "\033[0m>";

  else if (auto integer_literal_ptr = token.getIntegerLiteral())
    stream << "<int: " << *integer_literal_ptr << '>';

  else if (auto real_literal_ptr = token.getRealLiteral())
    stream << "<real: " << *real_literal_ptr << '>';

  else if (auto string_literal_ptr = token.getStringLiteral())
    stream << "<\"" << *string_literal_ptr << "\">";

  else if (auto boolean_literal_ptr = token.getBooleanLiteral())
    stream << "<\033[31m" << (*boolean_literal_ptr ? "true" : "false") << "\033[0m>";

  return stream;
}
