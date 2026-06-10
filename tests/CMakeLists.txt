# 自动获取 GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# 如果你的 tinyxml2 没有单独建库，就创建一个临时库目标（避免重复编译）
if(NOT TARGET tinyxml2)
    add_library(tinyxml2 STATIC ${CMAKE_SOURCE_DIR}/src/thirdparty/tinyxml2.cpp)
    target_include_directories(tinyxml2 PUBLIC ${CMAKE_SOURCE_DIR}/src/thirdparty)
endif()

# 辅助函数：为每个模块添加测试（保留原有，可按需使用）
function(add_test_module MODULE_NAME)
    set(TEST_EXE "test_${MODULE_NAME}")
    set(TEST_SRC "${CMAKE_CURRENT_SOURCE_DIR}/test_${MODULE_NAME}.cpp")
    add_executable(${TEST_EXE} ${TEST_SRC})
    target_link_libraries(${TEST_EXE}
            PRIVATE
            gtest_main
            gtest
            parser
            util
            tinyxml2
            )
    target_include_directories(${TEST_EXE} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    add_test(NAME ${TEST_EXE} COMMAND $<TARGET_FILE:${TEST_EXE}>)
    add_custom_command(TARGET ${TEST_EXE} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:${TEST_EXE}>/data
            COMMENT "Copying test data to build directory"
            )
endfunction()

add_test_module(parser)
add_test_module(parser_action)
add_test_module(parser_tenpai)

# ========== 新增：合并所有测试的单一目标 ==========
set(ALL_TEST_SRCS
        test_parser.cpp
        test_parser_action.cpp
        test_parser_tenpai.cpp
        )

add_executable(test_all ${ALL_TEST_SRCS})
target_link_libraries(test_all
        PRIVATE
        gtest_main
        gtest
        parser
        util
        tinyxml2
        )
target_include_directories(test_all PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME test_all COMMAND $<TARGET_FILE:test_all>)

add_custom_command(TARGET test_all POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:test_all>/data
        COMMENT "Copying test data to test_all build directory"
        )