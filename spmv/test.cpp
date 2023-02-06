#include <omp.h>
#include "spmv.hpp"
#include "data_type.hpp"

#define DATASET_DIR "/spmv/dataset"
#define TEST_FILE "/spmv/dataset/test_pattern.txt"
#define MTX_WEB_ND "/spmv/dataset/web-NotreDame.mtx"
#define MTX_WEB_SDF "/spmv/dataset/web-Stanford.mtx"
#define NEW_ORDER "group_size.csv"
#define METRICS_CSV "metric.csv"
#define StdSCALE_CSV "std_scale.csv"

CSR read_mtx(const char* file_path);
std::vector<T_Idx> read_order(const char* file_path, size_t num_row);
void solve_spmv(const CSR& , const double*, double*);
void solve_spmv(const CSR& , const std::vector<T_Idx>&, const double*, double*);

int main()
{
    /* prepare data */
    constexpr size_t warmtimes = 10;
    constexpr size_t looptimes = 1000;
    auto mat = read_mtx(MTX_WEB_SDF);
    auto row_order = read_order(NEW_ORDER, mat._num_rows);
    std::vector<double> x(mat._num_cols, 1);
    std::vector<double> y(mat._num_rows);

    /* warm up */
    double clock_start = omp_get_wtime();
    for (size_t i = 0; i < warmtimes; ++i) {
        solve_spmv(mat, x.data(), y.data());
        solve_spmv(mat, row_order, x.data(), y.data());
    }
    printf("warmup time %.3lfs\n", (omp_get_wtime() - clock_start));

    /* do reordered spmv */
    clock_start = omp_get_wtime();
    for (size_t i = 0; i < looptimes; ++i) {
        solve_spmv(mat, row_order, x.data(), y.data());
    }
    printf("%.3lfs (aveage reordered spmv time)\n", (omp_get_wtime() - clock_start));

    /* do naive spmv */
    clock_start = omp_get_wtime();
    for (size_t i = 0; i < looptimes; ++i) {
        solve_spmv(mat, x.data(), y.data());
    }
    printf("%.3lfs (aveage spmv time)\n", (omp_get_wtime() - clock_start));

    return 0;
}

// int main()
// {
//     auto mat = read_mtx(TEST_FILE);
//     const size_t x_size = mat._num_cols;
//     const size_t y_size = mat._num_rows;
//     std::vector<double> x(x_size, 1);
//     std::vector<double> y(y_size);
//     solve_spmv(mat, x.data(), y.data());
//     for (size_t i = 0; i < y_size; i++) {
//         printf("%lu : %lf\n", i, y[i]);
//     }
//     return 0;
// }
