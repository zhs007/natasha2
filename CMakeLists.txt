cmake_minimum_required(VERSION 3.0.0)
project(maintest VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_definitions(-DNATASHA_COUNTRTP)
add_definitions(-DNATASHA_RUNINCPP)
add_definitions(-DNATASHA_DEBUG)
add_definitions(-DNATASHA_MONEYTYPE_INT64)
add_definitions(-DNATASHA_SYMBOLTYPE_INT)

include_directories(./include)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
# aux_source_directory(./test DIR_ROOT_SRCS)

add_subdirectory(src)
add_subdirectory(libfortuna)
add_subdirectory(protoc)
add_subdirectory(tlod)
add_subdirectory(museum)

add_executable(maintest ./test/test.cpp)

target_link_libraries(maintest libnatasha2)
target_link_libraries(maintest libprotoc)
target_link_libraries(maintest libfortuna)
target_link_libraries(maintest libtlod)
target_link_libraries(maintest libmuseum)
target_link_libraries(maintest libprotobuf.a)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
