#include <cassert>
#include <utility>
extern "C" {
#include "mmio.h"
}
#include "read_data.hpp"

// read SuiteSparse %MatrixMarket matrix coordinate pattern general%
CSR read_data(const char* file_path)
{
    FILE *fp = fopen(file_path, "r");
    assert(fp);

    // read and check banner
    MM_typecode matcode;
    mm_read_banner(fp, &matcode);
    // assert( mm_is_matrix(matcode) && mm_is_pattern(matcode) && 
    //         mm_is_sparse(matcode) && mm_is_general(matcode));

    // read and initialize COO info
    T_Idx num_rows, num_cols, nnz;
    mm_read_mtx_crd_size(fp, &num_rows, &num_cols, &nnz);
    COO matcoo(num_rows, num_cols, nnz);

    // read rows, columns, entries(pattern=1 / real=%lf)
    std::vector<T_Idx> nnz_at_row(num_rows+1, 0); // a[num_rows]={0}
    for (int id_row, id_col, i=0; i<nnz; i++) {
        fscanf(fp, "%d %d\n", &id_row, &id_col);
        /* adjust from 1-based to 0-based */
        --id_row; --id_col;
        matcoo.add(i, id_row, id_col);
        ++nnz_at_row[id_row];
    }
    fclose(fp);
    
    // update csr_row_ptr
    decltype(nnz_at_row)& csr_row_ptr = nnz_at_row;
    assert(csr_row_ptr.data() == nnz_at_row.data());
    for (T_Idx row_nnz, row_begin=0, i=0; i<=num_rows; ++i) {
        row_nnz = nnz_at_row[i];
        csr_row_ptr[i] = row_begin;
        row_begin += row_nnz;
    }
    assert(csr_row_ptr[num_rows] == nnz);

    // // test wirte to stdout
    // mm_write_banner(stdout, matcode);
    // mm_write_mtx_crd_size(stdout, num_rows, num_cols, nnz);

    return CSR(std::move(matcoo), std::move(csr_row_ptr));
}
