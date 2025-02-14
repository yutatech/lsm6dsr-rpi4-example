cmake_minimum_required(VERSION 3.22)
project(lsm6dsr_rpl4_example)

file(GLOB SRCS src/*.c*)
add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC
    include
)

set(RPL4_LOG_LEVEL "DEBUG")

add_subdirectory(lib/lsm6dsr-rpl4-lib)
add_subdirectory(lib/RPL4)

target_link_libraries(${PROJECT_NAME}
    lsm6dsr_rpl4_lib
    rpl4
)