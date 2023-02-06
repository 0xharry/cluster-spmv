#include "preprocess.hpp"
#include "read_data.hpp"

void reorder(const CSR& mat);
// void write_data();

void preprocess(const char *mtx_path)
{
    CSR data = read_data(mtx_path);
    reorder(data);
    // write_data();
    return;
}
