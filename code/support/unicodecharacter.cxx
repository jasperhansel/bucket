#include "common.hxx"
#include "support/unicodecharacter.hxx"
#include <cassert>

namespace support {

bool UnicodeCharacter::isEndOfFile() const noexcept
{
  return mCodePoint == -1;
}

UnicodeCharacter::operator std::string_view() noexcept
{
  return std::string_view(reinterpret_cast<char*>(mBytes.data()), numberOfBytes());
}

std::ostream& operator<<(std::ostream& stream, UnicodeCharacter character)
{
  stream << std::string_view(character);
  return stream;
}

UnicodeCharacter::UnicodeCharacter() noexcept
: mCodePoint{-1}
{}

UnicodeCharacter::UnicodeCharacter(utf8proc_int32_t code_point, std::array<utf8proc_uint8_t, 4> bytes, unsigned char number_of_bytes) noexcept
: mCodePoint{code_point},
  mBytes{bytes}
{
  assert(numberOfBytes() == number_of_bytes);
}

unsigned char UnicodeCharacter::numberOfBytes() const noexcept
{
  if (mCodePoint == -1)
    return 0;
  if (mBytes[0] <= 0x7F)
    return 1;
  if (mBytes[0] <= 0xDF)
    return 2;
  if (mBytes[0] <= 0xEF)
    return 3;
  return 4;
}

}
