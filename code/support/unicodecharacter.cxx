#include "common.hxx"
#include "support/unicodecharacter.hxx"
#include <cassert>

namespace support {

UnicodeCharacter::UnicodeCharacter(int ascii)
: mCodePoint{ascii}
{
  assert(0 <= ascii && ascii <= 0x7F);
  mBytes[0] = static_cast<utf8proc_uint8_t>(ascii);
}

bool UnicodeCharacter::isEndOfFile() const noexcept
{
  return mCodePoint == -1;
}

bool UnicodeCharacter::isLetter() const noexcept
{
  return mCodePoint == -1 ? false : utf8proc_category_string(mCodePoint)[0] == 'L';
}

int UnicodeCharacter::getAscii() const noexcept
{
  if (mCodePoint == -1)
    return -1;
  if (mBytes[0] <= 0x7F)
    return mBytes[0];
  return -1;
}

bool UnicodeCharacter::isAsciiDigit() const noexcept
{
  switch (mCodePoint) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return true;
    default:
      return false;
  }
}

bool UnicodeCharacter::isNumericDigit() const noexcept
{
  return mCodePoint == -1 ? false : utf8proc_category(mCodePoint) == UTF8PROC_CATEGORY_ND;
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

bool UnicodeCharacter::operator==(UnicodeCharacter other) const noexcept
{
  return mCodePoint == other.mCodePoint;
}

bool UnicodeCharacter::operator!=(UnicodeCharacter other) const noexcept
{
  return !(*this == other);
}

UnicodeCharacter::UnicodeCharacter() noexcept
: mCodePoint{-1}
{}

UnicodeCharacter::UnicodeCharacter(utf8proc_int32_t code_point, std::array<utf8proc_uint8_t, 4> bytes, [[maybe_unused]] unsigned char number_of_bytes) noexcept
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
