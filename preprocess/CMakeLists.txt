find_package(OpenMP)

add_library(preprocess preprocess.cpp reorder.cpp row_pattern.cpp)

target_include_directories(preprocess PRIVATE 
${CMAKE_SOURCE_DIR}/basic
${CMAKE_SOURCE_DIR}/read_data
)

target_link_libraries(preprocess PRIVATE data_type read_data armadillo -fopenmp)

if (ENABLE_TESTS)
    add_executable(test_reorder test_reorder.cpp)
    target_link_libraries(test_reorder PRIVATE preprocess)
    target_include_directories(test_reorder PRIVATE 
    ${CMAKE_SOURCE_DIR}/basic
    ${CMAKE_SOURCE_DIR}/read_data
    )
endif(ENABLE_TESTS)
