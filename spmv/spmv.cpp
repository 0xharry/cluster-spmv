#include <vector>
#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include "spmv.hpp"
#include "read_data.hpp"

CSR read_mtx(const char *file_path)
{
    CSR mat = read_data(file_path);
    return mat;
}

std::vector<T_Idx> read_order(const char *file_path, size_t num_row)
{
    std::vector<T_Idx> row_order(num_row);
    FILE *fp = fopen(file_path, "r");
    for (size_t i = 0; i < num_row; ++i){
        fscanf(fp, "%d", &row_order[i]);
    }
    fclose(fp);
    return row_order;
}

inline void rvp(const CSR &mat, T_Idx row_idx, const double* v, double& y)
{
    double sum = 0.;
    T_Idx col_idx;
    T_Idx ptr_begin = mat._row_ptr[row_idx];
    T_Idx ptr_end   = mat._row_ptr[row_idx+1];
    for (size_t ptr = ptr_begin; ptr < ptr_end; ++ptr) {
        col_idx = mat._col_idx[ptr];
        sum += mat._val[ptr] * v[col_idx];
    }
    y = sum;
}

void solve_spmv(const CSR &mat, const double* x, double* y)
{
    const size_t num_row = mat._num_rows;
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < num_row; i++) {
        rvp(mat, i, x, y[i]);
    }
}

void solve_spmv(const CSR &mat, const std::vector<T_Idx> &row_order, const double* x, double* y)
{
    T_Idx row_idx;
    const size_t num_row = mat._num_rows;
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < num_row; i++) {
        row_idx = row_order[i];
        rvp(mat, row_idx, x, y[row_idx]);
    }
}

void spmv(const char *mtx_path, const char *row_order_path)
{
    /* prepare data */
    constexpr size_t warmtimes = 10;
    constexpr size_t looptimes = 1000;
    auto mat = read_mtx(mtx_path);
    auto row_order = read_order(row_order_path, mat._num_rows);
    std::vector<double> x(mat._num_cols, 1);
    std::vector<double> y(mat._num_rows);

    /* warm up */
    double clock_start = omp_get_wtime();
    for (size_t i = 0; i < warmtimes; ++i)
    {
        solve_spmv(mat, x.data(), y.data());
        solve_spmv(mat, row_order, x.data(), y.data());
    }
    printf("warmup time %.3lfs\n", (omp_get_wtime() - clock_start));

    /* do reordered spmv */
    clock_start = omp_get_wtime();
    for (size_t i = 0; i < looptimes; ++i)
    {
        solve_spmv(mat, row_order, x.data(), y.data());
    }
    printf("%.3lfs (aveage reordered spmv time)\n", (omp_get_wtime() - clock_start));

    /* do naive spmv */
    clock_start = omp_get_wtime();
    for (size_t i = 0; i < looptimes; ++i)
    {
        solve_spmv(mat, x.data(), y.data());
    }
    printf("%.3lfs (aveage spmv time)\n", (omp_get_wtime() - clock_start));
}
