#pragma once

#include <vector>
#include <cstdio>
#include <tuple>

typedef double T_Val;
typedef int T_Idx;

#define NO_COPY_AND_ASSIGNMENT(class_name) \
    class_name( const class_name& ) = delete;\
    class_name& operator=( const class_name& ) = delete;

struct SpMatBase
{
    T_Idx _num_rows, _num_cols, _nnz, foo;
};

struct COO : public SpMatBase
{
    std::vector<T_Idx> _row_idx, _col_idx;
    std::vector<T_Val> _val;

    COO(SpMatBase mat);
    COO(T_Idx nrows, T_Idx ncols, T_Idx nnz): COO(SpMatBase{nrows, ncols, nnz}) {};
    ~COO() = default;

    NO_COPY_AND_ASSIGNMENT(COO)

    void add(T_Idx order, T_Idx idx_row, T_Idx idx_col, T_Val entry_val=1);
    // get tuple[_row_idx, _col_idx, _val]
    std::tuple<T_Idx, T_Idx, T_Val> get(T_Idx order) const {
        return {_row_idx[order], _col_idx[order], _val[order]};
    }
};

struct CSR : public SpMatBase
{
    std::vector<T_Idx> _row_ptr, _col_idx;
    std::vector<T_Val> _val;

    CSR(SpMatBase mat);
    CSR(T_Idx nrows, T_Idx ncols, T_Idx nnz): CSR(SpMatBase{nrows, ncols, nnz}) {};
    CSR(CSR&& matcsr);
    CSR(COO&& matcoo, std::vector<T_Idx>&& row_ptr);
    ~CSR() = default;

    bool operator==(const CSR& mat) const;
    NO_COPY_AND_ASSIGNMENT(CSR)

    std::tuple<T_Idx, const T_Idx*, const T_Val*> get_row(T_Idx row_idx) const {
        T_Idx ptr_begin = _row_ptr[row_idx];
        T_Idx ptr_end   = _row_ptr[row_idx+1];
        T_Idx nnz_row   = ptr_end - ptr_begin;
        const T_Idx* p_col = &_col_idx[ptr_begin];
        const T_Val* p_val = &_val[ptr_begin];
        return {nnz_row, p_col, p_val};
    }
};

// struct CSC : public SpMatBase
// {
//     T_Idx *col_ptr, *row_idx;
//     T_Val *_val;
// };

// struct Context
// {
//     CSR *data;
// };
