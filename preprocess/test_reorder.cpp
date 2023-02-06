#include "preprocess.hpp"
#include "read_data.hpp"
#include "reorder.hpp"
#include "test.hpp"

#define DATASET_DIR "/spmv/dataset"
#define TEST_FILE "/spmv/dataset/test_pattern.txt"
#define MTX_WEB_ND "/spmv/dataset/web-NotreDame.mtx"
#define MTX_WEB_SDF "/spmv/dataset/web-Stanford.mtx"
#define METRICS_CSV "metric.csv"
#define StdSCALE_CSV "std_scale.csv"

int main()
{
    CSR data = read_data(MTX_WEB_SDF);
    CsrRowPattern pattern(data);
    pattern.evaluate();

    {
        std::vector<RowMetrics> ref{
            {0, 2, 2, 1.0, 1, 2.0},
            {1, 2, 5, 0.4, 0, 0.0},
            {2, 5, 7, 0.7, 2, 2.5},
            {3, 1, 1, 1.0, 0, 0.0}
        };
        test_true(ref == pattern.get_raw_pattern());
    }

    pattern.save_csv(METRICS_CSV);
    pattern.scale();
    pattern.cluster_order();
    pattern.load_csv(StdSCALE_CSV);

    {
        std::vector<RowMetrics> std_ref{
            {-1.34164078649987e+00,-3.33333333333333e-01,-7.33799385705343e-01,8.93776172693556e-01,3.01511344577764e-01,7.68349819927832e-01},
            {-4.47213595499958e-01,-3.33333333333333e-01,5.24142418360959e-01,-1.52806972165217e+00,-9.04534033733291e-01,-9.87878339907213e-01},
            {4.47213595499958e-01,1.66666666666667e+00,1.36277028773849e+00,-2.59482623734937e-01,1.50755672288882e+00,1.20740685988659e+00},
            {1.34164078649987e+00,-1.00000000000000e+00,-1.15311332039411e+00,8.93776172693556e-01,-9.04534033733291e-01,-9.87878339907213e-01}
        };
        test_true(std_ref == pattern.get_raw_pattern());
    }

    return 0;
}
