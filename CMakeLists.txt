CMAKE_MINIMUM_REQUIRED(VERSION 3.9.4)


PROJECT(bucket)


INCLUDE_DIRECTORIES(code)


SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)


FILE(GLOB_RECURSE SOURCES code/*.cxx)


IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6.0.0)
  MESSAGE(STATUS "Clang detected")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-shadow-field-in-constructor -Wno-weak-vtables")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1 -fsanitize=undefined,address -fno-omit-frame-pointer")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -flto -fstrict-vtable-pointers -ffast-math -fno-rtti -fwhole-program-vtables -march=native -mtune=native")
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.2.0)
  MESSAGE(STATUS "GCC detected")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -fsanitize=undefined,address -fno-omit-frame-pointer")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -flto -ffast-math -fno-rtti -march=native -mtune=native")
ENDIF()


ADD_EXECUTABLE(bucket ${SOURCES})
