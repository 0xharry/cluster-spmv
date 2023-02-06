cmake_minimum_required(VERSION 3.0)

project(spmv)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(ENABLE_TESTS ON)

####################################
include_directories(AFTER 
# ${CMAKE_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/basic
)

add_subdirectory(${PROJECT_SOURCE_DIR}/basic)
add_subdirectory(${PROJECT_SOURCE_DIR}/read_data)
add_subdirectory(${PROJECT_SOURCE_DIR}/preprocess)
add_subdirectory(${PROJECT_SOURCE_DIR}/spmv)


# ####################################
add_executable(main main.cpp)
target_link_libraries(main preprocess spmv)
target_include_directories( main PRIVATE
    ${PROJECT_SOURCE_DIR}/preprocess
    ${PROJECT_SOURCE_DIR}/spmv
)

