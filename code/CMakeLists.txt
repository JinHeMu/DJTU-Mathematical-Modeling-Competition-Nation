cmake_minimum_required(VERSION 3.26)
project(code C)

set(CMAKE_C_STANDARD 11)




add_executable(code
        solpos00.h
        stest00.c
        solpos.c
)
#        solpos.c)
target_link_libraries(code m)