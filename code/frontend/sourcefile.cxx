#include "common.hxx"
#include "frontend/sourcefile.hxx"

namespace frontend {

SourceFile::SourceFile(const char* path)
: mFileReader{path},
  mPosition{1, 1}
{}

support::UnicodeCharacter SourceFile::currentCharacter() const noexcept
{
  return mFileReader.currentCharacter();
}

void SourceFile::next()
{
  if (mFileReader.currentCharacter() == '\n') {
    mPosition.line++;
    mPosition.column = 1;
  }
  else if (!mFileReader.currentCharacter().isEndOfFile())
    mPosition.column++;
  mFileReader.next();
}

SourceFile::Position SourceFile::position() const noexcept
{
  return mPosition;
}

}
