cmake_minimum_required(VERSION 3.13)

set(BOOK_PPCP_SOURCES
    "main.cpp"
    "e_01_helloworld.cpp"
    "e_02_promise_future.cpp"
    )
add_executable(book_ppcp ${BOOK_PPCP_SOURCES})
