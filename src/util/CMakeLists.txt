file(GLOB SRC_FILES *.cpp)

add_library(util STATIC ${SRC_FILES})
target_link_libraries(util tinyxml2)
