#include "read_data.hpp"
#include "test.hpp"
extern "C" {
#include "mmio.h"
}

#define DATASET_DIR "/spmv/dataset"
#define TEST_FILE "/spmv/dataset/test_pattern.txt"

int main()
{
    printf("test data_type\n");

    FILE *fp = fopen(TEST_FILE, "r");
    assert(fp != NULL);
    MM_typecode matcode;
    int ret_flag;
    ret_flag = mm_read_banner(fp, &matcode);
    assert(!ret_flag);
    ret_flag = (mm_is_matrix(matcode) && mm_is_pattern(matcode) && 
                mm_is_sparse(matcode) && mm_is_general(matcode));
    assert(ret_flag);
    T_Idx num_rows, num_cols, nnz;
    ret_flag = mm_read_mtx_crd_size(fp, &num_rows, &num_cols, &nnz);
    assert(!ret_flag);
    printf("num_rows %d, num_cols %d, nnz %d\n", num_rows, num_cols, nnz);
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
    {
        std::vector<T_Idx> ref_row_nnz {2,2,5,1,0}; // dummy tail(0)
        test_eq(nnz_at_row, ref_row_nnz);
    }
    // test COO
    {
    std::vector<T_Idx> row_idx {0,1,2,0,2,1,2,2,3,2};
    std::vector<T_Idx> col_idx {1,1,1,2,2,5,5,6,6,7};
    std::vector<T_Val> val     {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
    test_eq(matcoo._num_rows, 4);
    test_eq(matcoo._num_cols, 8);
    test_eq(matcoo._nnz, 10);
    test_eq(matcoo._row_idx, row_idx);
    test_eq(matcoo._col_idx, col_idx);
    test_eq(matcoo._val, val);
    }
    
    // update csr_row_ptr
    decltype(nnz_at_row)& csr_row_ptr = nnz_at_row;
    test_true(csr_row_ptr.data() == nnz_at_row.data());
    for (T_Idx row_nnz, row_begin=0, i=0; i<=num_rows; ++i) {
        row_nnz = nnz_at_row[i];
        csr_row_ptr[i] = row_begin;
        row_begin += row_nnz;
    }
    test_eq(csr_row_ptr[num_rows], nnz);

    CSR matcsr(std::move(matcoo), std::move(csr_row_ptr));
    // test CSR
    {
    std::vector<T_Idx> row_ptr {0,2,4,9,10};
    std::vector<T_Idx> col_idx {1,2,1,5,1,2,5,6,7,6};
    std::vector<T_Val> val     {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
    test_eq(matcsr._num_rows, 4);
    test_eq(matcsr._num_cols, 8);
    test_eq(matcsr._nnz, 10);
    test_eq(matcsr._row_ptr, row_ptr);
    test_eq(matcsr._col_idx, col_idx);
    test_eq(matcsr._val, val);
    CSR data = read_data(TEST_FILE);
    test_true(matcsr == data);
    }

    TEST_SUCCEED

    return 0;
}