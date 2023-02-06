#include "preprocess.hpp"
#include "reorder.hpp"

#define METRICS_CSV "metric.csv"
#define StdSCALE_CSV "std_scale.csv"

// std::vector<T_Idx> reorder_row_idx();

void reorder(const CSR& matcsr)
{
    CsrRowPattern pattern(matcsr);
    pattern.evaluate();
    pattern.save_csv(METRICS_CSV);
    pattern.scale();
    pattern.cluster_order();
    // pattern.load_csv(StdSCALE_CSV);
    // auto row_idx_map = reorder_row_idx();
}
