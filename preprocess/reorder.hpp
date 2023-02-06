#pragma once

#include "data_type.hpp"
#include <string>

typedef float T_Dens;
typedef float T_Dist;

/****************************** RowMetrics ***************************************/

struct RowMetrics
{
    static constexpr int _num_mertrics = 6;
    float _row_idx;
    float _nnz;
    float _length;
    float _density;
    float _num_block;
    float _len_block;
    // std::vector<T_Idx> _pattern_section;

    void set_default_val(T_Idx idx) {
        _row_idx = idx;
        _nnz = 0;
        _length = 0;
        _density = 0;
        _num_block = 0;
        _len_block = 0;
    }

    bool operator==(const RowMetrics& other) const {
        bool is_eq = (
            (std::abs(_row_idx   - other._row_idx  ) < 0.05) &&
            (std::abs(_nnz       - other._nnz      ) < 0.05) &&
            (std::abs(_length    - other._length   ) < 0.05) &&
            (std::abs(_density   - other._density  ) < 0.05) &&
            (std::abs(_num_block - other._num_block) < 0.05) &&
            (std::abs(_len_block - other._len_block) < 0.05)
        );
        return is_eq;
    }

    T_Dist manhattan_distance(const RowMetrics& m) {
        T_Dist dist = std::abs(m._row_idx   - _row_idx)
                    + std::abs(m._nnz       - _nnz)
                    + std::abs(m._length    - _length)
                    + std::abs(m._density   - _density)
                    + std::abs(m._num_block - _num_block)
                    + std::abs(m._len_block - _len_block);
        return dist;
    }
};

class CsrRowPattern
{
public:
    CsrRowPattern(const CSR& mat);
    ~CsrRowPattern() = default;

    int  num_metrics();
    void evaluate();
    void load_csv(const std::string &filename);
    void save_csv(const std::string &filename);
    void scale();
    void cluster_order();
    const std::vector<RowMetrics>& get_raw_pattern() {
        return _pattern;
    };

private:
    // using RowPattern = void*;
    std::vector<RowMetrics> _pattern;
    const CSR& _ref_mat;
};
