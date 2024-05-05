# Parallel Programming: Concepts And Practice

set(BOOK_PPCP_SOURCES
    "main.cpp"
    "e_01_helloworld.cpp"
    "e_02_promise_future.cpp"
    "e_03_dispatch_param.cpp"
    "e_04_false_sharing.cpp"
    )
add_executable(book_ppcp ${BOOK_PPCP_SOURCES})
