# Common Header
include_directories(include)

# global librarys
find_package(spdlog CONFIG REQUIRED)
link_libraries(spdlog::spdlog spdlog::spdlog_header_only)
find_package(doctest CONFIG REQUIRED)
link_libraries(doctest::doctest)

# Parallel Programming: Concepts And Practice
add_subdirectory(ppcp)

