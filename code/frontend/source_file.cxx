#include "miscellaneous/common.hxx"
#include "frontend/source_file.hxx"
#include "miscellaneous/concatenate.hxx"
#include <stdexcept>
using namespace frontend;


SourceFile::Position::Position()
: line(0),
  column(0)
{}


SourceFile::Position::Position(unsigned line, unsigned column)
: line(line),
  column(column)
{}


SourceFile::SourceFile(const char* path)
: file(path),
  current_position(1, 1)
{
  if (!file)
    throw std::runtime_error(misc::concatenate("unable to open file '", path, '\''));
  current_character = file.get();
  if (current_character == eof && !file.eof())
    throw std::runtime_error("failed to read character from file");
}


int SourceFile::current() const noexcept
{
  return current_character;
}


void SourceFile::next()
{
  if (current_character == eof)
    return;
  else if (current_character == '\n') {
    current_position.line++;
    current_position.column = 1;
  }
  else {
    current_position.column++;
  }
  current_character = file.get();
  if (current_character == eof)
    if (!file.eof())
      throw std::runtime_error("failed to read character from file");
}


SourceFile::Position SourceFile::position()
{
  return current_position;
}
