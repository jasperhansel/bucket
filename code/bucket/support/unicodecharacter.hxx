#ifndef BUCKET_SUPPORT_UNICODECHARACTER_HXX
#define BUCKET_SUPPORT_UNICODECHARACTER_HXX

#include "common.hxx"
#include <array>
#include <ostream>
#include <string_view>
#include <utf8proc.h>

namespace support {

class UnicodeCharacter {

friend class UnicodeFileReader;

public:

  UnicodeCharacter() noexcept;

  UnicodeCharacter(int ascii);

  bool isEndOfFile() const noexcept;

  bool isLetter() const noexcept;

  int getAscii() const noexcept;

  bool isAsciiDigit() const noexcept;

  bool isNumericDigit() const noexcept;

  operator std::string_view() noexcept;

  std::string_view bytes() const noexcept;

  friend std::ostream& operator<<(std::ostream& stream, UnicodeCharacter character);

  bool operator==(UnicodeCharacter other) const noexcept;

  bool operator!=(UnicodeCharacter other) const noexcept;

private:

  utf8proc_int32_t mCodePoint;

  std::array<utf8proc_uint8_t, 4> mBytes;

  UnicodeCharacter(utf8proc_int32_t code_point, std::array<utf8proc_uint8_t, 4> bytes, unsigned char number_of_bytes) noexcept;

  unsigned char numberOfBytes() const noexcept;

};

}

#endif
