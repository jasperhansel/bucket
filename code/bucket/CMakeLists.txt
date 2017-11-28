set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6.0.0)
  message(STATUS "detected clang 6.0.0 or greater")
  if(BUCKET_WARN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-shadow-field-in-constructor -Wno-weak-vtables -Wno-documentation-unknown-command")
  endif()
  if(BUCKET_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined,address -fno-omit-frame-pointer -g")
  endif()
  if(BUCKET_OPTIMIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -flto -fstrict-vtable-pointers -ffast-math -fwhole-program-vtables -march=native -mtune=native")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.2.0)
  message(STATUS "detected gcc 7.2.0 or greater")
  if(BUCKET_WARN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
  endif()
  if(BUCKET_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined,address -fno-omit-frame-pointer -g")
  endif()
  if(BUCKET_OPTIMIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -flto -ffast-math -fipa-pta -march=native -mtune=native")
  endif()
endif()

add_executable(bucket
  abstract_syntax_tree/printer.cxx
  code_generator/code_generator.cxx
  compiler_objects/class.cxx
  compiler_objects/field.cxx
  compiler_objects/method.cxx
  compiler_objects/module.cxx
  compiler_objects/object.cxx
  compiler_objects/scope.cxx
  frontend/lexer.cxx
  frontend/parser.cxx
  frontend/sourcefile.cxx
  frontend/token.cxx
  support/unicodecharacter.cxx
  support/unicodefilereader.cxx
  main.cxx
)

target_include_directories(bucket PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(bucket utf8proc)

if(BUCKET_ACCELERATE_BUILD)
  include(cotire)
  cotire(bucket)
endif()
