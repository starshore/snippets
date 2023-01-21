cmake_minimum_required(VERSION 3.13)
project(snippets)

# global librarys
find_package(spdlog CONFIG REQUIRED)
link_libraries(spdlog::spdlog spdlog::spdlog_header_only)
find_package(doctest CONFIG REQUIRED)
link_libraries(doctest::doctest)

# global includes
include_directories("include")

# Parallel Programming: Concepts And Practice
add_subdirectory("book/ppcp")
