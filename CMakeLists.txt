cmake_minimum_required(VERSION 3.10)
project(TextMiningCompiler)
project(TextMiningApp)
project(test1)
project(test2)
project(test3)
project(test4)
project(test5)
project(test6)
project(test7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3")

set (SRC
    src/ptrie.cc
    src/bloom.cc
    )

set (COMPILER
     src/text-mining-compiler.cc
     )
   
set (APP
     src/text-mining-app.cc
    )

set (TEST1
     tests/test1.cc
     )
set (TEST2
     tests/test2.cc
     )
set (TEST3
     tests/test3.cc
     )
set (TEST4
     tests/test4.cc
     )
set (TEST5
     tests/test5.cc
     )
set (TEST6
     tests/test6.cc
     )
set (TEST7
     tests/test7.cc
     )

# Main binary

add_executable(TextMiningCompiler ${COMPILER} ${SRC})
add_executable(TextMiningApp ${APP} ${SRC})
add_executable(test1 ${TEST1} ${SRC})
add_executable(test2 ${TEST2} ${SRC})
add_executable(test3 ${TEST3} ${SRC})
add_executable(test4 ${TEST4} ${SRC})
add_executable(test5 ${TEST5} ${SRC})
add_executable(test6 ${TEST6} ${SRC})
add_executable(test7 ${TEST7} ${SRC})

target_link_libraries(TextMiningCompiler)
target_link_libraries(TextMiningApp)
target_link_libraries(test1)
target_link_libraries(test2)
target_link_libraries(test3)
target_link_libraries(test4)
target_link_libraries(test5)
target_link_libraries(test6)
target_link_libraries(test7)
