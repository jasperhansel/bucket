#ifndef BUCKET_FRONTEND_SOURCEFILE_HXX
#define BUCKET_FRONTEND_SOURCEFILE_HXX

#include "common.hxx"
#include "support/unicodecharacter.hxx"
#include "support/unicodefilereader.hxx"

namespace frontend {

class SourceFile {

public:

  struct Position {
    unsigned line, column;
    constexpr Position() : line{0}, column{0} {}
    constexpr Position(unsigned line, unsigned column) : line{line}, column{column} {}
  };

  explicit SourceFile(const char* path);

  support::UnicodeCharacter currentCharacter() const noexcept;

  void next();

  Position position() const noexcept;

private:

  support::UnicodeFileReader mFileReader;

  Position mPosition;

};

}

#endif
