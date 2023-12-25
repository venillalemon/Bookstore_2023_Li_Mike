cmake_minimum_required(VERSION 3.22)
project(code)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")

add_executable(code
        src/accounts.h
        src/books.h
        main.cpp
        src/loginstate.h
        src/finance.h
        src/error.h
        src/parser.h
        src/blocklist.h
        src/log.h
)
