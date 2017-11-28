#include "common.hxx"
#include "support/concatenate.hxx"
#include "compiler_objects/module.hxx"
#include "frontend/lexer.hxx"
#include "frontend/parser.hxx"
#include "frontend/sourcefile.hxx"
#include "frontend/token.hxx"
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>


static void read(const char* path)
{
  frontend::SourceFile source_file{path};
  while (!source_file.currentCharacter().isEndOfFile()) {
    std::cout << source_file.currentCharacter();
    source_file.next();
  }
}


static void lex(const char* path)
{
  frontend::Lexer lexer{path};
  while (!(lexer.currentToken().getSymbol() && *lexer.currentToken().getSymbol() == frontend::Symbol::EndOfFile)) {
    std::cout << lexer.currentToken();
    lexer.next();
  }
}


static void parse(const char* path)
{
  std::unique_ptr<ast::Class> program;
  {
    frontend::Parser parser{path};
    program = parser.parse();
  }
  std::cout << *program;
}


static void compile(const char* path)
{
  std::unique_ptr<ast::Class> program;
  {
    frontend::Parser parser{path};
    program = parser.parse();
  }
  cobjs::Module module{program.get()};
  module.init(program.get());
}


static void main_with_exceptions(int argc, char* argv[])
{
  if (argc == 3 && std::strcmp(argv[1], "--read") == 0) {
    read(argv[2]);
    return;
  }
  if (argc == 3 && std::strcmp(argv[1], "--lex") == 0) {
    lex(argv[2]);
    return;
  }
  if (argc == 3 && std::strcmp(argv[1], "--parse") == 0) {
    parse(argv[2]);
    return;
  }
  if (argc == 3 && std::strcmp(argv[1], "--compile") == 0) {
    compile(argv[2]);
    return;
  }
  throw std::runtime_error("bad command line arguments");
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
