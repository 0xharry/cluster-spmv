#pragma once

#include <cassert>
#include <cstdio>
#include <type_traits>
#include <string>
#include <string_view>
#include <vector>

#define CLR "\e[0m"
#define BLD "\e[1m"
#define UDL "\e[4m"
#define RED "\e[31m"
#define yellow "\e[33m"
#define GRN "\e[32m"
#define BG_RED "\e[41;37m"

#define RIGHT_CHECK BLD "[" GRN "√" CLR BLD "]"
#define WRONG_CHECK BLD "[" RED "x" CLR BLD "]"

#define TEST_SUCCEED \
  printf(BLD \
  "[" GRN "passed" CLR BLD "]" CLR \
  " %s()\n====================================\n",\
  __func__);

template<typename T>
struct is_string
        : public std::disjunction<
                std::is_same<char *, typename std::decay_t<T>>,
                std::is_same<const char *, typename std::decay_t<T>>,
                std::is_same<std::string_view, typename std::decay_t<T>>,
                std::is_same<std::string, typename std::decay_t<T>>
        > {
};

template<typename T>
constexpr bool is_string_v = is_string<T>::value;

template <typename T>
struct is_pair : std::false_type {};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

//specialize a type for all of the STL containers.
namespace is_stl_container_impl{
  template <typename T>                 struct is_stl_container                        :  std::false_type{};
  // template <typename T, std::size_t N>  struct is_stl_container<std::array<T,N>>       :  std::true_type{};
  template <typename... Args>           struct is_stl_container<std::vector<Args...>>  :  std::true_type{};
  // template <typename... Args> struct is_stl_container<std::deque             <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::list              <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::forward_list      <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::set               <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::multiset          <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::map               <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::multimap          <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::unordered_set     <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::unordered_multiset<Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::unordered_map     <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::unordered_multimap<Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::stack             <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::queue             <Args...>>:std::true_type{};
  // template <typename... Args> struct is_stl_container<std::priority_queue    <Args...>>:std::true_type{};
}

//type trait to utilize the implementation type traits as well as decay the type
template <typename T> struct is_stl_container {
  static constexpr bool const value = is_stl_container_impl::is_stl_container<std::decay_t<T>>::value;
};

template<class ValueType>
constexpr bool check_eq(ValueType val0, ValueType val1)
{
  if constexpr (std::is_floating_point_v<ValueType>) {
    constexpr double EPS = 1e-3;
    return (std::abs(val0 - val1) < EPS)? true: false;
  } else {
    return (val0 == val1);
  }
}

template<class ValueType>
constexpr const char* fmt4printf()
{
  return  std::is_arithmetic_v<ValueType>?
            std::is_floating_point_v<ValueType>?
              "%e"
            : std::is_unsigned_v<ValueType>?
                "%lu"
              : std::is_same_v<ValueType, int>?
                  "%d"
                : "%ld"
          : "%s";
}

// auto detect printf val fmt
template<class ValueType>
void printv(ValueType val)
{
  if constexpr (is_pair<ValueType>::value) {
    static_assert(std::is_same_v<
      typename ValueType::first_type,
      typename ValueType::second_type>);
    constexpr const char* val_fmt = fmt4printf<typename ValueType::first_type>();
    printf("{");
    printf(val_fmt, val.first);
    printf(", ");
    printf(val_fmt, val.second);
    printf("}");
  } else if constexpr (is_stl_container<ValueType>::value) {
    constexpr const char* val_fmt = fmt4printf<typename ValueType::value_type>();
    printf("vector{");
    bool is_first_entry = true;
    for (auto&& v : val) {
      if (is_first_entry)
        is_first_entry = false;
      else
        printf(", ");
      printf(val_fmt, v);
    }
    printf("}");
  } else {
    constexpr const char* val_fmt = fmt4printf<ValueType>();
    printf(val_fmt, val);
  }
}

template<class ValueType>
void print_result(bool succeed, ValueType val0, ValueType val1)
{
  if (succeed) {
    printf(RIGHT_CHECK " {");
    printv(val0);
  } else {
    printf(WRONG_CHECK " {");
    printv(val0);
    printf(" != ");
    printv(val1);
  }
  printf("} " CLR "\n");
}

template<>
void print_result(bool succeed, bool val0, bool val1)
{
  if (succeed) printf(RIGHT_CHECK);
  else         printf(WRONG_CHECK);

  printf(CLR "\n");
}

template<class ValueType>
bool test_check_eq(ValueType val0, ValueType val1)
{
  bool succeed = check_eq(val0, val1);
  print_result(succeed, val0, val1);
  return succeed;
}

// #define test_eq(val0, val1) \
//   printf(BLD "+ { " #val0 " == " #val1 " } "); \
//   test_check_eq(val0, val1);

// #define test_true(exp) \
//   printf(BLD "+ { " #exp " } ? "); \
//   test_check_eq(true, exp);



#define test_eq(val0, val1) {           \
  bool succeed = check_eq(val0, val1);  \
  printf(BLD "+ ");                     \
  if (succeed) printf(RIGHT_CHECK);     \
  else         printf(WRONG_CHECK);     \
  printf(" { " #val0 " == " #val1 " } ");\
  print_result(succeed, val0, val1);    \
  }

#define test_true(exp) {                \
  bool succeed = check_eq(true, exp);  \
  printf(BLD "+ ");                     \
  if (succeed) printf(RIGHT_CHECK);     \
  else         printf(WRONG_CHECK);     \
  printf(BLD " { " #exp " } ? "); \
  print_result(succeed, true, exp);    \
  }
