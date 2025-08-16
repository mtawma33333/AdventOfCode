
#pragma once

#include <algorithm>
#include <cctype>
#include <locale>
#include <utility>
#include <span>

/**
 * 2D坐标处理工具
 * 1. 坐标运算符重载
 * 2. 坐标哈希函数
 * 3. 边界检查函数
 */

using xy_pos_t = std::pair<int, int>;

template <typename t_t, typename u_t>
auto operator+(const std::pair<t_t, u_t> &l, const std::pair<t_t, u_t> &r)
{
  return std::pair<t_t, u_t>{ l.first + r.first, l.second + r.second };
}

template <typename t_t, typename u_t>
auto operator-(const std::pair<t_t, u_t> &l, const std::pair<t_t, u_t> &r)
{
  return std::pair<t_t, u_t>{ l.first - r.first, l.second - r.second };
}

template <typename t_t, typename u_t>
auto operator-(const std::pair<t_t, u_t> &l)
{
  return std::pair<t_t, u_t>{ -l.first, -l.second };
}

template <>
struct std::hash<xy_pos_t>
{
  size_t operator()(const xy_pos_t &k) const
  {
    // Compute individual hash values for fields
    // and combine them using XOR
    // and bit shifting:

    return hash<int>{}(k.first) ^ (hash<int>{}(k.second) << 1u);
  }
};

inline bool is_pos_on_map(xy_pos_t pos, xy_pos_t dim)
{
  return !(pos.first < 0 || pos.second < 0 || pos.first >= dim.first || pos.second >= dim.second);
}

/* 为 AOC 通用状态结构提供哈希支持 */
template <typename State>
  requires requires(const State &s) {
    s.pos;
    s.dir;
  }
struct std::hash<State>
{
  size_t operator()(const State &s) const
  {
    return hash<xy_pos_t>{}(s.pos) ^ (hash<xy_pos_t>{}(s.dir) << 2u);
  }
};

/* 字符串处理工具 */
inline void ltrim(std::string &s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

inline void rtrim(std::string &s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

inline bool string_contains(const std::string &s1, const std::string &s2)
{
  return s1.find(s2) != std::string::npos;
}

/**
 * @brief 生成给定范围内的所有排列组合
 * @tparam t_t 数字类型
 * @tparam n_t 数字个数
 * @tparam max_value_t 最大值
 * @tparam reset_value_t 重置值, 默认为 0
 */
template <typename t_t, size_t n_t, t_t max_value_t, t_t reset_value_t = 0>
struct permutator
{
  private:
  std::array<t_t, n_t> m_nums{};

  public:
  permutator()
  {
    m_nums.fill(reset_value_t);
  }

  //return false when the permutation repeats
  bool next_permutation()
  {
    for (auto &num : m_nums)
    {
      if (num == max_value_t)
      {
        num = reset_value_t;
      }
      else
      {
        num++;
        break;
      }
    }
    return !std::all_of(m_nums.begin(), m_nums.end(), [](auto i) { return i == reset_value_t; });
  }

  std::span<const t_t, n_t> get_nums()
  {
    return std::span(m_nums);
  }
};