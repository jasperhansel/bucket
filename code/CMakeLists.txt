cmake_minimum_required(VERSION 3.9.4 FATAL_ERROR)

project(bucket VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(BUCKET_ACCELERATE_BUILD "Uses cotire to speed up the build, but may cause build errors" OFF)

option(BUCKET_SANITIZE "Enables runtime checks for undefined behavior" OFF)

option(BUCKET_OPTIMIZE "Enables the highest level of compiler optimization" ON)

option(BUCKET_WARN "Enables the highest warning level" ON)

add_subdirectory(utf8proc)

add_subdirectory(bucket)
