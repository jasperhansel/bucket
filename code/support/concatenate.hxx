// concatenate.hxx
// Defines the function template concatenate, which takes any number of
// arguments, converts them to strings, and returns the strings joined together.
// The actual implementation of concatenate() below is pure C++17 recursive
// unsafe variadic constexpr template voodoo.

#ifndef BUCKET_SUPPORT_CONCATENATE_HXX
#define BUCKET_SUPPORT_CONCATENATE_HXX

#include "common.hxx"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>

namespace support {

namespace details {

// Helper Templates:
// These function similarly to the templates found in the standard header
// <type_traits>.

template <typename> struct is_ordinary_signed_integer                           : std::false_type {};
template <> struct is_ordinary_signed_integer<signed char>                      : std::true_type  {};
template <> struct is_ordinary_signed_integer<short>                            : std::true_type  {};
template <> struct is_ordinary_signed_integer<int>                              : std::true_type  {};
template <> struct is_ordinary_signed_integer<long>                             : std::true_type  {};
template <> struct is_ordinary_signed_integer<long long>                        : std::true_type  {};

template <typename> struct is_ordinary_unsigned_integer                         : std::false_type {};
template <> struct is_ordinary_unsigned_integer<unsigned char>                  : std::true_type  {};
template <> struct is_ordinary_unsigned_integer<unsigned short>                 : std::true_type  {};
template <> struct is_ordinary_unsigned_integer<unsigned>                       : std::true_type  {};
template <> struct is_ordinary_unsigned_integer<unsigned long>                  : std::true_type  {};
template <> struct is_ordinary_unsigned_integer<unsigned long long>             : std::true_type  {};

template <typename> struct is_c_style_string                                    : std::false_type {};
template <> struct is_c_style_string<char*>                                     : std::true_type  {};
template <> struct is_c_style_string<const char*>                               : std::true_type  {};
template <> struct is_c_style_string<volatile char*>                            : std::true_type  {};
template <> struct is_c_style_string<const volatile char*>                      : std::true_type  {};

template <typename> struct is_char_array_ref                                    : std::false_type {};
template <std::size_t N> struct is_char_array_ref<char (&) [N]>                 : std::true_type  {};
template <std::size_t N> struct is_char_array_ref<const char (&) [N]>           : std::true_type  {};
template <std::size_t N> struct is_char_array_ref<volatile char (&) [N]>        : std::true_type  {};
template <std::size_t N> struct is_char_array_ref<const volatile char (&) [N]>  : std::true_type  {};

}

// ConcatenateHelper:
// To use concatenate with a type, one must define an explicit specialization of
// the ConcatenateHelper template struct for that type. To use concatenate with
// a set of types, a partial specialization must be defined with an enable_if
// that checks whether a given type is in the set. Keep in mind that there are
// no implicit conversions between types, so if you define a specialization for
// Foo, concatenate will work with Foo, but not with Foo&.

template <typename T, typename Enable = void>
struct ConcatenateHelper {

  // sizeAsString():
  // This function takes a value and returns how many characters it would take
  // up in a string (not including a null terminator).
  static constexpr std::size_t sizeAsString(T value) noexcept;

  // writeAsString():
  // This function takes a value, converts it to a string, and writes it to the
  // buffer 'buf'. One must only modify the characters from buf[0] to buf[N - 1]
  // inclusive where N is the return value of sizeAsString(value). This function
  // should return buf + N. Do not write a null terminator to the buffer.
  static char* writeAsString(char* BUCKET_RESTRICT buf, T value) noexcept;

};

// char specialization

template <typename T>
struct ConcatenateHelper<T, typename std::enable_if<std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, char>>::type> {

  static constexpr std::size_t sizeAsString(char) noexcept
  {
    return 1;
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf, char value) noexcept
  {
    *(buf++) = value;
    return buf;
  }

};

// bool specialization

template <typename T>
struct ConcatenateHelper<T, typename std::enable_if<std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, bool>>::type> {

  static constexpr std::size_t sizeAsString(bool value) noexcept
  {
    return value ? 4 : 5; // since true has 4 letters and false has five
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf, bool value) noexcept
  {
    if (value) {
      *(buf++) = 't'; *(buf++) = 'r'; *(buf++) = 'u'; *(buf++) = 'e';
    }
    else {
      *(buf++) = 'f'; *(buf++) = 'a'; *(buf++) = 'l'; *(buf++) = 's'; *(buf++) = 'e';
    }
    return buf;
  }

};

// specialization for unsigned integers

template <typename T>
struct ConcatenateHelper<T, typename std::enable_if<details::is_ordinary_unsigned_integer<std::remove_cv_t<std::remove_reference_t<T>>>::value>::type> {

  static constexpr std::size_t sizeAsString(std::remove_cv_t<std::remove_reference_t<T>> value) noexcept
  {
    std::size_t result = 0;
    do {
      value /= 10;
      result++;
    } while (value);
    return result;
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf, std::remove_cv_t<std::remove_reference_t<T>> value) noexcept
  {
    auto old_buf = buf;
    do {
      *(buf++) = value % 10 + '0';
      value /= 10;
    } while (value);
    std::reverse(old_buf, buf);
    return buf;
  }

};

// specialization for signed integers

template <typename T>
struct ConcatenateHelper<T, typename std::enable_if<details::is_ordinary_signed_integer<std::remove_cv_t<std::remove_reference_t<T>>>::value>::type> {

  static constexpr std::size_t sizeAsString(std::remove_cv_t<std::remove_reference_t<T>> value) noexcept
  {
    std::size_t result = value < 0 ? 1 : 0; // for the minus sign
    do {
      value /= 10;
      result++;
    } while (value);
    return result;
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf, std::remove_cv_t<std::remove_reference_t<T>> value) noexcept
  {
    if (value < 0) {
      *(buf++) = '-';
      value = -value;
    }
    auto digits_start = buf;
    do {
      *(buf++) = value % 10 + '0';
      value /= 10;
    } while (value);
    std::reverse(digits_start, buf);
    return buf;
  }

};

// specialization for c style strings

template <typename T>
struct ConcatenateHelper<T, typename std::enable_if<details::is_c_style_string<std::remove_cv_t<std::remove_reference_t<T>>>::value>::type> {

  static constexpr std::size_t sizeAsString(std::remove_cv_t<std::remove_reference_t<T>> value)
  {
    return std::char_traits<char>::length(value);
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf, std::remove_cv_t<std::remove_reference_t<T>> value)
  {
    while (*value != '\0')
      *(buf++) = *(value++);
    return buf;
  }

};

// specialization for char arrays

template <typename T>
struct ConcatenateHelper<T, typename std::enable_if<details::is_char_array_ref<T>::value>::type> {

  static constexpr std::size_t sizeAsString(T value)
  {
    return sizeof(value) - 1; // one is subtracted because sizeof() includes a
    // null terminator, but writeAsString() doesn't write a null terminator.
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf, T value)
  {
    return std::copy(value, value + sizeof(value) - 1, buf);
  }

};

namespace details {

// RecursiveConcatenateHelper:
// A recursive variadic template which generalizes ConcatenateHelper for an
// arbitrary number of arguments.

// base template

template <typename... Args>
struct RecursiveConcatenateHelper {

  static constexpr std::size_t sizeAsString(Args&&... args) noexcept;

  static char* writeAsString(char* BUCKET_RESTRICT buf, Args&&... args) noexcept;

};

// recursive partial specialization

template <typename First, typename... Rest>
struct RecursiveConcatenateHelper<First, Rest...> {

  static constexpr std::size_t sizeAsString(First&& first, Rest&&... rest) noexcept
  {
    return ConcatenateHelper<First>::sizeAsString(std::forward<First>(first)) +
           RecursiveConcatenateHelper<Rest...>::sizeAsString(std::forward<Rest>(rest)...);
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf, First&& first, Rest&&... rest) noexcept
  {
    return RecursiveConcatenateHelper<Rest...>::writeAsString(
      ConcatenateHelper<First>::writeAsString(buf, std::forward<First>(first)),
      std::forward<Rest>(rest)...
    );
  }

};

// base case explicit specialization

template <>
struct RecursiveConcatenateHelper<> {

  static constexpr std::size_t sizeAsString() noexcept
  {
    return 0;
  }

  static char* writeAsString(char* BUCKET_RESTRICT buf) noexcept
  {
    return buf;
  }

};

}

// Concatenate:
// Converts each argument to a string and returns the result of joining those
// strings together. All of this is done with just a single dynamic allocation.

template <typename... Args>
std::string concatenate(Args&&... args)
{
  auto total_size = details::RecursiveConcatenateHelper<Args...>::sizeAsString(std::forward<Args>(args)...);
  std::string result;
  result.resize(total_size);
  [[maybe_unused]] auto ptr = details::RecursiveConcatenateHelper<Args...>::writeAsString(
    &result[0], std::forward<Args>(args)...
  );
  #ifdef BUCKET_DEBUG_BUILD
  assert(ptr - &result[0] == static_cast<decltype(ptr - &result[0])>(total_size));
  #endif
  return result;
}

}

#endif
