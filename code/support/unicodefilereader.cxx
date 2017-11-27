#include "common.hxx"
#include "support/unicodefilereader.hxx"
#include <cassert>
#include <stdexcept>

namespace support {

UnicodeFileReader::UnicodeFileReader(const char* path)
: mFile{path}
{
  // check to make sure the file is open
  if (!mFile)
    throw std::runtime_error("unable to open file");

  // read bytes into buffer
  mFile.read(reinterpret_cast<char*>(mBuffer.data()), 4);
  if (!mFile && !mFile.eof())
    throw std::runtime_error("failed to read from file");

  auto bytes_read = mFile.gcount();
  assert(0 <= bytes_read && bytes_read <= 4);
  mBytesInBuffer = static_cast<unsigned char>(bytes_read);

  // check for end of file
  if (mBytesInBuffer == 0)
    return;

  // check for byte order mark
  if (mBytesInBuffer >= 3 && mBuffer[0] == 0xEF && mBuffer[1] == 0xBB && mBuffer[2] == 0xBF) {
    mCodePointLength = 3;
    next();
  }

  // decode bytes to get the code point
  decode();
}

UnicodeCharacter UnicodeFileReader::currentCharacter() const noexcept
{
  return mBytesInBuffer == 0 ? UnicodeCharacter() : UnicodeCharacter(mCodePoint, mBuffer, mCodePointLength);
}

void UnicodeFileReader::next()
{
  // check for eof
  if (mBytesInBuffer == 0)
    return;

  // remove bytes corresponding to current code point from the buffer and shift
  // the remaining bytes to the start of the buffer
  for (unsigned char i = 0; i != 4 - mCodePointLength; ++i)
    mBuffer[i] = mBuffer[i + mCodePointLength];

  // get a pointer to the start of the bytes to fill
  auto bytes_to_fill = reinterpret_cast<char*>(mBuffer.data() + 4 - mCodePointLength);

  // read bytes from file
  mFile.read(bytes_to_fill, mCodePointLength);
  if (!mFile && !mFile.eof())
    throw std::runtime_error("failed to read from file");

  // update the number of bytes in the buffer
  auto bytes_read = mFile.gcount();
  assert(0 <= bytes_read && bytes_read <= 4);
  mBytesInBuffer = mBytesInBuffer - mCodePointLength + static_cast<unsigned char>(bytes_read);

  // check for eof
  if (mBytesInBuffer == 0)
    return;

  // decode bytes to get the code point
  decode();
}

void UnicodeFileReader::decode()
{
  auto return_code = utf8proc_iterate(mBuffer.data(), mBytesInBuffer, &mCodePoint);
  if (return_code == UTF8PROC_ERROR_INVALIDUTF8)
    throw std::runtime_error("unable to decode unicode");
  assert(return_code == 1 || return_code == 2 || return_code == 3 || return_code == 4);
  mCodePointLength = static_cast<unsigned char>(return_code);
}

}
