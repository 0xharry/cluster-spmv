#include "data_type.hpp"

#include <cassert>
#include <utility>
#include <cstdio>

/******************* COO *******************/
COO::COO(SpMatBase mat) : SpMatBase{mat}
{
    _col_idx.resize(mat._nnz);
    _row_idx.resize(mat._nnz);
    _val.resize(mat._nnz);

    assert(_col_idx.size() && _row_idx.size() && _val.size());
}

void COO::add(int order, T_Idx idx_row, T_Idx idx_col, T_Val entry_val)
{
    assert(order < _nnz);

    _row_idx[order] = idx_row;
    _col_idx[order] = idx_col;
    _val[order] = entry_val;
}

/******************* COO *******************/


/******************* CSR *******************/
CSR::CSR(SpMatBase mat) : SpMatBase{mat}
{
    _row_ptr.resize(mat._num_rows+1);
    _col_idx.resize(mat._nnz);
    _val.resize(mat._nnz);

    assert(_col_idx.size() && _row_ptr.size() && _val.size());
}

CSR::CSR(CSR&& mat) : SpMatBase{mat}
{
    _row_ptr = std::move(mat._row_ptr);
    _col_idx = std::move(mat._col_idx);
    _val = std::move(mat._val);

    assert(_col_idx.size() && _row_ptr.size() && _val.size());
    assert(!(mat._col_idx.size() || mat._row_ptr.size() || mat._val.size()));
}

// move COO => CSR
CSR::CSR(COO&& matcoo, std::vector<T_Idx>&& row_ptr): SpMatBase{matcoo}
{
    assert(row_ptr.size() == (_num_rows+1));

    _row_ptr = std::move(row_ptr);
    _col_idx.resize(_nnz);
    _val.resize(_nnz);

    // update _col_idx, _val
    std::vector<T_Idx> row_val_count(_num_rows, 0);
    for (T_Idx i=0; i<_nnz; ++i) {
        auto [row_idx, col_idx, val] = matcoo.get(i);
        T_Idx row_begin = _row_ptr[row_idx];
        T_Idx val_offset = row_begin + row_val_count[row_idx];
        _col_idx[val_offset] = col_idx;
        _val[val_offset] = val;
        ++row_val_count[row_idx];
    }
}

bool CSR::operator==(const CSR& mat) const
{
    return  (_num_rows == _num_rows) &&\
            (_num_cols == _num_cols) &&\
            (_nnz == _nnz) &&\
            (_row_ptr == _row_ptr) &&\
            (_col_idx == _col_idx) &&\
            (_val == _val);
}


/******************* CSR *******************/

