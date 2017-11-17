#include "miscellaneous/common.hxx"
#include "frontend/lexer.hxx"
#include "frontend/source_file.hxx"
#include "frontend/token.hxx"
#include <cstring>
#include <exception>
#include <iostream>


static void read(const char* path)
{
  frontend::SourceFile source_file{path};
  while (source_file.current() != frontend::SourceFile::eof) {
    std::cout << static_cast<char>(source_file.current());
    source_file.next();
  }
}


static void lex(const char* path)
{
  frontend::Lexer lexer{path};
  while (!(lexer.current().getSymbol() && *lexer.current().getSymbol() == frontend::Symbol::EndOfFile)) {
    std::cout << lexer.current();
    lexer.next();
  }
}


static void main_with_exceptions(int argc, char* argv[])
{
  if (argc >= 3 && std::strcmp(argv[1], "--read") == 0) {
    read(argv[2]);
    return;
  }
  if (argc >= 3 && std::strcmp(argv[1], "--lex") == 0) {
    lex(argv[2]);
    return;
  }
}


int main(int argc, char* argv[]) noexcept
{
  try {
    main_with_exceptions(argc, argv);
    return 0;
  } catch (std::exception& e) {
    std::cerr << "bucket: \033[31merror:\033[0m " << e.what() << '\n';
    return 1;
  }
}
