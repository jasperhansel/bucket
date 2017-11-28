project(utf8proc VERSION 2.1.0 LANGUAGES C)

add_library(utf8proc utf8proc.c)

target_include_directories(utf8proc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
