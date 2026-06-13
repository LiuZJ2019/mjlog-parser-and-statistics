cmake_minimum_required(VERSION 3.10)
project(tenhou_paipu_parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(src)
include_directories(src/thirdparty)

add_subdirectory(src/util)
add_subdirectory(src/parser)

add_library(tinyxml2 STATIC src/thirdparty/tinyxml2.cpp)
target_include_directories(tinyxml2 PUBLIC src/thirdparty)

add_compile_options(-Werror)

find_package(Threads REQUIRED)
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE parser util tinyxml2 Threads::Threads)

option(ENABLE_TESTS "Enable unit tests" OFF)
if(ENABLE_TESTS)
    enable_testing()
    message(STATUS "ENABLE_TESTS")
    add_subdirectory(tests)
endif()

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(--coverage)
        add_link_options(--coverage)
        message(STATUS "Coverage enabled: adding --coverage flags")
    else()
        message(WARNING "Coverage not supported for this compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

option(ENABLE_ASAN "Enable AddressSanitizer for memory leak detection" OFF)

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fsanitize=address -g -O1 -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
        message(STATUS "AddressSanitizer enabled")
    else()
        message(WARNING "AddressSanitizer not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()
