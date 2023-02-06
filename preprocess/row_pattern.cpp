#include <cassert>
#include <fstream>

#define MLPACK_PRINT_INFO
#define MLPACK_PRINT_WARN
#include <mlpack.hpp>

#include "reorder.hpp"

/****************************** Evaluate row pattern ***************************************/

inline T_Dist eval_row_length(const T_Idx *col_idx, T_Idx nnz_row) {
    assert(nnz_row>0 && col_idx!=nullptr);
    return (T_Dist)(col_idx[nnz_row-1] - col_idx[0] + 1);
}

inline T_Dens eval_row_density(T_Idx length_row, T_Idx nnz_row) {
    return ((T_Dens)nnz_row/length_row);
}

/**
 * block metrics: {num_blocks, ave_block_len}
*/
inline auto eval_row_block(const T_Idx *col_idx, T_Idx nnz_row) {
    struct {
        T_Dist num;
        T_Dist ave_len;
    } block = {0, 0.};
    T_Idx block_len = 1;
    bool in_block = false;

    auto within_a_block = [col_idx](T_Idx i) {
        return (col_idx[i-1] == (col_idx[i]-1));
    };
    auto leaving_a_block = [&in_block]() {
        return (true == in_block);
    };
    auto update_block_metrics = [&block, &block_len]() {
        block.ave_len = (block.ave_len * block.num + block_len) / (block.num + 1);
        ++block.num;
    };

    for (T_Idx i=1; i<nnz_row; ++i) {
        if (within_a_block(i)) {
            in_block = true;
            ++block_len;
        } else if (leaving_a_block()) {
            update_block_metrics();
            in_block = false;
            block_len = 1;
        }
    }

    if (leaving_a_block()) {
        update_block_metrics();
    }

    return block;
}

/****************************** CsrRowPattern ***************************************/

CsrRowPattern::CsrRowPattern(const CSR &mat) : _ref_mat(mat), _pattern(mat._num_rows) {};

int CsrRowPattern::num_metrics()
{
    return RowMetrics::_num_mertrics;
}

// evaluate sparse pattern of each row
void CsrRowPattern::evaluate()
{
    for (T_Idx i=0; i<_ref_mat._num_rows; ++i) {
        auto [nnz_row, p_col, p_val] = _ref_mat.get_row(i);
        if (nnz_row == 0) {
            _pattern[i].set_default_val(i);
        } else {
            T_Dist length  = eval_row_length(p_col, nnz_row);
            auto density = eval_row_density(length, nnz_row);
            auto block   = eval_row_block(p_col, nnz_row);
            _pattern[i] = {(float)i, (float)nnz_row, length, density, block.num, block.ave_len};
        }
    }
}

void CsrRowPattern::load_csv(const std::string &filename)
{
    std::ifstream f(filename);
    char tmp;
    for (auto& m : _pattern) {
        f >> m._row_idx     >> tmp \
          >> m._nnz         >> tmp \
          >> m._length      >> tmp \
          >> m._density     >> tmp \
          >> m._num_block   >> tmp \
          >> m._len_block   ;
    }
}

void CsrRowPattern::save_csv(const std::string &filename)
{
    std::ofstream f(filename);
    for (auto& m : _pattern) {
        f << m._row_idx     << ',' \
          << m._nnz         << ',' \
          << m._length      << ',' \
          << m._density     << ',' \
          << m._num_block   << ',' \
          << m._len_block   << std::endl;
    }
}

arma::mat tmp_middle;
#define METRICS_CSV "metric.csv"
#define StdSCALE_CSV "std_scale.csv"
void CsrRowPattern::scale()
{
    using namespace mlpack::data;
    arma::mat input;
    Load(METRICS_CSV, input);
    // MinMaxScaler scale;
    StandardScaler scale;
    scale.Fit(input);
    scale.Transform(input, tmp_middle);
    // Save(StdSCALE_CSV, tmp_middle);
}

#define IDX_ID_MAP "index_id_map.csv"
void CsrRowPattern::cluster_order()
{
    std::cout << "PCA" << std::endl;
    using namespace mlpack;
    PCA<> pca(true);
    pca.Apply(tmp_middle, 2);

    arma::Row<size_t> assignments;
    constexpr size_t num_clusters = 1000;
    KMeans<> kmeans; // kmeans.MaxIterations() = 500;
    kmeans.Cluster(tmp_middle, num_clusters, assignments);

    std::vector<size_t> row_idx_map = arma::conv_to<std::vector<size_t>>::from(assignments);
    std::cout << "sizeof assignments.n_elem = " << assignments.n_elem << std::endl;

    std::array<std::vector<int>, num_clusters> group_rowidx_map;
    for (int i=0; i<assignments.n_elem; ++i) {
        group_rowidx_map[assignments[i]].emplace_back(i);
    }

    std::ofstream f(IDX_ID_MAP);
    for (int i=0; i<num_clusters; ++i) {
        for (int rowidx : group_rowidx_map[i])
        f << rowidx << ' ';
    }
    tmp_middle.clear();
}


// std::cout << "DBSCAN START" << std::endl;
// const double epsilon = 1;
// DBSCAN<> model(epsilon, 2);
// // Perform clustering using DBSCAN, an return the number of clusters. 
// arma::Row<size_t> assignments;
// std::cout << "DBSCAN.Cluster" << std::endl;
// const size_t numCluster = model.Cluster(tmp_middle, assignments);
// std::cout << "Number of clusters: " << numCluster << std::endl;
// std::cout << "DBSCAN END" << std::endl;

// int num_noise = 0;
// for (int i=0; i<assignments.n_elem; ++i) {
//     if (assignments[i] == SIZE_MAX) {
//         row_idx_map.emplace_back(-1);
//         ++num_noise;
//         std::cout << i << " noise\n";
//     } else {
//         row_idx_map.emplace_back(assignments[i]);
//         std::cout << i << ": " << row_idx_map[i] << std::endl;
//     }
// }
// std::cout << "number of noise points: " << num_noise << std::endl;


/**************************** reorder_row_idx ****************************/

// std::vector<T_Idx> reorder_row_idx()
// {
//     using arma::mat;
//     using namespace mlpack::data;

//     mat input;
//     Load(StdSCALE_CSV, input);
//     return {};
// }
