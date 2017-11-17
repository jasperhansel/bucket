#pragma once
#include <fstream>


namespace frontend {


class SourceFile {

public:

  struct Position {
    Position();
    Position(unsigned line, unsigned column);
    unsigned line, column;
  };

  static constexpr auto eof = std::ifstream::traits_type::eof();

  explicit SourceFile(const char* path);

  int current() const noexcept;

  void next();

  Position position();

private:

  std::ifstream file;

  int current_character;

  Position current_position;

};


}
