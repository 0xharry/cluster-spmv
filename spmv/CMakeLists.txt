find_package(OpenMP REQUIRED)

add_library(spmv spmv.cpp)
target_link_libraries(spmv PRIVATE read_data OpenMP::OpenMP_CXX)

target_include_directories(spmv PRIVATE 
${CMAKE_SOURCE_DIR}/basic
${CMAKE_SOURCE_DIR}/read_data
)

if (ENABLE_TESTS)
    add_executable(test_spmv test.cpp)
    target_link_libraries(test_spmv PRIVATE spmv)
endif(ENABLE_TESTS)
