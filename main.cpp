#include "header.hpp"

#define TEST_FILE "/spmv/dataset/test_pattern.txt"
#define NEW_ORDER "group_size.csv"

int main()
{
    preprocess(TEST_FILE);
    spmv(TEST_FILE, NEW_ORDER);
    return 0;
}
