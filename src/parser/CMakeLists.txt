file(GLOB SRC_FILES *.cpp)

add_library(parser STATIC ${SRC_FILES})

target_link_libraries(parser util)
