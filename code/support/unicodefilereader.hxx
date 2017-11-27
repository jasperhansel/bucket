#ifndef BUCKET_SUPPORT_UNICODEFILEREADER_HXX
#define BUCKET_SUPPORT_UNICODEFILEREADER_HXX

#include "common.hxx"
#include "support/unicodecharacter.hxx"
#include <array>
#include <fstream>
#include <utf8proc.h>

namespace support {

class UnicodeFileReader {

public:

  explicit UnicodeFileReader(const char* path);

  UnicodeCharacter currentCharacter() const noexcept;

  void next();

private:

  std::ifstream mFile;

  utf8proc_int32_t mCodePoint;

  std::array<utf8proc_uint8_t, 4> mBuffer;

  unsigned char mBytesInBuffer;

  unsigned char mCodePointLength;

  void decode();

};

}

#endif
