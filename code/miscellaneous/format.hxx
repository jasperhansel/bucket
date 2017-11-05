#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>


namespace misc {


#if defined __clang__
  #define RESTRICT __restrict__
#elif defined __GNUC__
  #define RESTRICT __restrict__
#elif defined _MSC_VER
  #define RESTRICT __restrict
#else
  #define RESTRICT
#endif


inline constexpr std::size_t sizeAsString(char) noexcept
{
  return 1;
}


inline char* writeAsString(char* RESTRICT buf, char value) noexcept
{
  *(buf++) = value;
  return buf;
}


inline constexpr std::size_t sizeAsString(bool value) noexcept
{
  return value ? 4 : 5;
}


inline char* writeAsString(char* RESTRICT buf, bool value) noexcept
{
  if (value) {
    *(buf++) = 't'; *(buf++) = 'r'; *(buf++) = 'u'; *(buf++) = 'e';
  }
  else {
    *(buf++) = 'f'; *(buf++) = 'a'; *(buf++) = 'l'; *(buf++) = 's'; *(buf++) = 'e';
  }
  return buf;
}


template <typename T> struct is_ordinary_signed_integer : std::false_type {};
template <> struct is_ordinary_signed_integer<signed char> : std::true_type {};
template <> struct is_ordinary_signed_integer<short> : std::true_type {};
template <> struct is_ordinary_signed_integer<int> : std::true_type {};
template <> struct is_ordinary_signed_integer<long> : std::true_type {};
template <> struct is_ordinary_signed_integer<long long> : std::true_type {};


template <typename T>
inline constexpr typename std::enable_if<is_ordinary_signed_integer<T>::value, std::size_t>::type sizeAsString(T value) noexcept
{
  std::size_t result = value < 0 ? 1 : 0;
  do {
    value /= 10;
    result++;
  } while (value);
  return result;
}


template <typename T>
inline typename std::enable_if<is_ordinary_signed_integer<T>::value, char*>::type writeAsString(char* RESTRICT buf, T value) noexcept
{
  if (value < 0) {
    *(buf++) = '-';
    value = -value;
  }
  auto old_buf = buf;
  do {
    *(buf++) = value % 10 + '0';
    value /= 10;
  } while (value);
  std::reverse(old_buf, buf);
  return buf;
}


template <typename T> struct is_ordinary_unsigned_integer : std::false_type {};
template <> struct is_ordinary_unsigned_integer<unsigned char> : std::true_type {};
template <> struct is_ordinary_unsigned_integer<unsigned short> : std::true_type {};
template <> struct is_ordinary_unsigned_integer<unsigned> : std::true_type {};
template <> struct is_ordinary_unsigned_integer<unsigned long> : std::true_type {};
template <> struct is_ordinary_unsigned_integer<unsigned long long> : std::true_type {};


template <typename T>
inline constexpr typename std::enable_if<is_ordinary_unsigned_integer<T>::value, std::size_t>::type sizeAsString(T value) noexcept
{
  std::size_t result = 0;
  do {
    value /= 10;
    result++;
  } while (value);
  return result;
}


inline constexpr std::size_t sizeAsString(const char* RESTRICT value) noexcept
{
  std::size_t result = 0;
  while (*(value++) != '\0')
    result++;
  return result;
};


template <typename T>
inline typename std::enable_if<is_ordinary_unsigned_integer<T>::value, char*>::type writeAsString(char* RESTRICT buf, T value) noexcept
{
  auto old_buf = buf;
  do {
    *(buf++) = value % 10 + '0';
    value /= 10;
  } while (value);
  std::reverse(old_buf, buf);
  return buf;
}


template <std::size_t N>
inline constexpr std::size_t sizeAsString(const char (&) [N]) noexcept
{
  return N;
}


template <std::size_t N>
inline char* writeAsString(char* RESTRICT buf, const char (&value) [N]) noexcept
{
  return std::copy(value, value + N, buf);
}


inline char* writeAsString(char* RESTRICT buf, const char* RESTRICT value) noexcept
{
  while (*value != '\0')
    *(buf++) = *(value++);
  return buf;
};


inline constexpr std::size_t sizeAsStringHelper() noexcept
{
  return 0;
}


template <typename First, typename... Rest>
inline constexpr std::size_t sizeAsStringHelper(First&& first, Rest&&... rest) noexcept
{
  return sizeAsString(std::forward<First>(first)) + sizeAsStringHelper(std::forward<Rest>(rest)...);
}


inline char* writeAsStringHelper(char* RESTRICT buf) noexcept
{
  return buf;
}


template <typename First, typename... Rest>
inline char* writeAsStringHelper(char* RESTRICT buf, First&& first, Rest&&... rest) noexcept
{
  return writeAsStringHelper(writeAsString(buf, std::forward<First>(first)), std::forward<Rest>(rest)...);
}


template <typename... Args>
std::string format(Args&&... args)
{
  auto total_size = sizeAsStringHelper(std::forward<Args>(args)...);
  std::string result;
  result.resize(total_size);
  #ifdef NDEBUG
  writeAsStringHelper(&result[0], std::forward<Args>(args)...);
  #else
  auto ptr = writeAsStringHelper(&result[0], std::forward<Args>(args)...);
  assert(ptr - &result[0] == static_cast<decltype(ptr - &result[0])>(total_size));
  #endif
  return result;
};


}
