file(GLOB_RECURSE SOURCE_FILES *.h *.cpp)
source_group(core FILES ${SOURCE_FILES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(ExampleDisplay STATIC ${SOURCE_FILES})
target_link_libraries(ExampleDisplay ${EXAMPLE_LIBS})
