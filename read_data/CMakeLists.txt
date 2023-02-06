
add_library(mmio mmio.c)

add_library(read_data read_data.cpp)
target_link_libraries(read_data PRIVATE mmio data_type)

if (ENABLE_TESTS)
    add_executable(test_read test_read.cpp)
    target_link_libraries(test_read PRIVATE read_data)
endif(ENABLE_TESTS)
