#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>
#include <unordered_map>
#include <functional>
#include <limits>
#include <array>
#include "../../include/advent.hpp"

// Shared keypad type tag used across helpers and DP
enum class KeypadType
{
  NUM,
  DIR
};

/**
 * Day 21: Keypad Conundrum
 * 存在两种不同的键盘布局
 * - 数字键盘
 *  - 只有一层
 *  - 负责代码输入
 * - 方向键盘
 *  - 有多层
 *  - 负责控制下一层键盘的移动
 * ? 对每一行代码，计算“在最顶层需要按的最少按键次数”，再乘以该代码的“数值部分”（去掉末尾 A 后的十进制整数）；最后把所有行的复杂度相加
 * ? part1: 方向键盘有2层
 * ? part2: 方向键盘有25层
数字键盘
+---+---+---+
| 7 | 8 | 9 |
+---+---+---+
| 4 | 5 | 6 |
+---+---+---+
| 1 | 2 | 3 |
+---+---+---+
    | 0 | A |
    +---+---+
方向键盘
    +---+---+
    | ^ | A |
+---+---+---+
| < | v | > |
+---+---+---+
*/

// === Keypad coordinate systems and maps ===
// Coordinate system: (x, y), x increases to the right, y increases downward.
// A position is valid if within dim and not in the corresponding hole set.
namespace keypad
{
using pos_map_t = std::unordered_map<char, xy_pos_t>;
using key_map_t = std::unordered_map<xy_pos_t, char>;
struct KeyPair
{
  char from;
  char to;
};
struct KeyPairHash
{
  size_t operator()(const KeyPair &k) const noexcept
  {
    return (static_cast<unsigned>(k.from) << 8) ^ static_cast<unsigned>(k.to);
  }
};
struct KeyPairEq
{
  bool operator()(const KeyPair &a, const KeyPair &b) const noexcept
  {
    return a.from == b.from && a.to == b.to;
  }
};
using cand_map_t = std::unordered_map<KeyPair, std::vector<std::string>, KeyPairHash, KeyPairEq>;

// Movement deltas for the directional keys
inline const std::unordered_map<char, xy_pos_t> MOVES{
  { '^', { 0, -1 } },
  { 'v', { 0, 1 } },
  { '<', { -1, 0 } },
  { '>', { 1, 0 } },
};

// Numeric keypad layout (width=3, height=4)
// y=0: 7 8 9
// y=1: 4 5 6
// y=2: 1 2 3
// y=3:   0 A   (hole at x=0, y=3)
inline const xy_pos_t NUM_DIM{ 3, 4 };
inline const std::set<xy_pos_t> NUM_HOLES{ xy_pos_t{ 0, 3 } };
inline const pos_map_t NUM_TO_POS{
  { '7', { 0, 0 } }, { '8', { 1, 0 } }, { '9', { 2, 0 } }, { '4', { 0, 1 } }, { '5', { 1, 1 } }, { '6', { 2, 1 } },
  { '1', { 0, 2 } }, { '2', { 1, 2 } }, { '3', { 2, 2 } }, { '0', { 1, 3 } }, { 'A', { 2, 3 } },
};
inline const key_map_t NUM_TO_KEY{
  { { 0, 0 }, '7' }, { { 1, 0 }, '8' }, { { 2, 0 }, '9' }, { { 0, 1 }, '4' }, { { 1, 1 }, '5' }, { { 2, 1 }, '6' },
  { { 0, 2 }, '1' }, { { 1, 2 }, '2' }, { { 2, 2 }, '3' }, { { 1, 3 }, '0' }, { { 2, 3 }, 'A' },
};

// Directional keypad layout (width=3, height=2)
// y=0:   ^ A   (hole at x=0, y=0)
// y=1: < v >
inline const xy_pos_t DIR_DIM{ 3, 2 };
inline const std::set<xy_pos_t> DIR_HOLES{ xy_pos_t{ 0, 0 } };
inline const pos_map_t DIR_TO_POS{
  { '^', { 1, 0 } }, { 'A', { 2, 0 } }, { '<', { 0, 1 } }, { 'v', { 1, 1 } }, { '>', { 2, 1 } },
};
inline const key_map_t DIR_TO_KEY{
  { { 1, 0 }, '^' }, { { 2, 0 }, 'A' }, { { 0, 1 }, '<' }, { { 1, 1 }, 'v' }, { { 2, 1 }, '>' },
};

inline bool is_valid_on_num(xy_pos_t p)
{
  return is_pos_on_map(p, NUM_DIM) && !NUM_HOLES.contains(p);
}

inline bool is_valid_on_dir(xy_pos_t p)
{
  return is_pos_on_map(p, DIR_DIM) && !DIR_HOLES.contains(p);
}

// --- Reusable helpers (declared in namespace to avoid duplication) ---

inline xy_pos_t pos_of(KeypadType K, char key)
{
  if (K == KeypadType::NUM)
    return NUM_TO_POS.at(key);
  return DIR_TO_POS.at(key);
}

inline bool is_valid(KeypadType K, xy_pos_t p)
{
  if (K == KeypadType::NUM)
    return is_valid_on_num(p);
  return is_valid_on_dir(p);
}

inline bool path_is_legal(KeypadType K, xy_pos_t startPos, const std::string &moves)
{
  auto p = startPos;
  for (char m : moves)
  {
    p = p + MOVES.at(m);
    if (!is_valid(K, p))
      return false;
  }
  return true;
}

inline std::string bfs_shortest_moves(KeypadType K, char fromKey, char toKey)
{
  xy_pos_t start = pos_of(K, fromKey);
  xy_pos_t goal = pos_of(K, toKey);
  if (start == goal)
    return std::string();
  static const std::array<char, 4> DIRS{ '^', 'v', '<', '>' };
  std::queue<std::pair<xy_pos_t, std::string>> q;
  std::set<xy_pos_t> vis;
  q.push({ start, std::string() });
  vis.insert(start);
  while (!q.empty())
  {
    auto [p, path] = q.front();
    q.pop();
    for (char d : DIRS)
    {
      auto np = p + MOVES.at(d);
      if (!is_valid(K, np) || vis.contains(np))
        continue;
      auto npth = path;
      npth.push_back(d);
      if (np == goal)
        return npth;
      vis.insert(np);
      q.push({ np, std::move(npth) });
    }
  }
  return std::string();
}

inline std::vector<std::string> gen_candidates(KeypadType K, char fromKey, char toKey)
{
  std::vector<std::string> out;
  auto a = pos_of(K, fromKey);
  auto b = pos_of(K, toKey);
  int dx = b.first - a.first;
  int dy = b.second - a.second;
  auto rep = [](char c, int n) { return std::string(std::max(0, n), c); };
  std::string horiz = rep('>', dx) + rep('<', -dx);
  std::string vert = rep('v', dy) + rep('^', -dy);

  if (path_is_legal(K, a, horiz + vert))
    out.push_back(horiz + vert);
  if (dx != 0 && dy != 0)
  {
    if (path_is_legal(K, a, vert + horiz))
      out.push_back(vert + horiz);
  }
  if (dx == 0 && dy == 0 && out.empty())
    out.push_back("");

  if (out.empty())
  {
    auto m = bfs_shortest_moves(K, fromKey, toKey);
    if (!m.empty() || fromKey == toKey)
      out.push_back(std::move(m));
  }
  return out;
}

inline const cand_map_t &get_candidates_map(KeypadType K)
{
  // Lazy precompute for all from/to pairs on the given keypad
  static cand_map_t num_cand;
  static bool num_ready = false;
  static cand_map_t dir_cand;
  static bool dir_ready = false;

  if (K == KeypadType::NUM)
  {
    if (!num_ready)
    {
      for (const auto &fi : NUM_TO_POS)
      {
        for (const auto &ti : NUM_TO_POS)
        {
          KeyPair kp{ fi.first, ti.first };
          num_cand.emplace(kp, gen_candidates(KeypadType::NUM, fi.first, ti.first));
        }
      }
      num_ready = true;
    }
    return num_cand;
  }
  else
  {
    if (!dir_ready)
    {
      for (const auto &fi : DIR_TO_POS)
      {
        for (const auto &ti : DIR_TO_POS)
        {
          KeyPair kp{ fi.first, ti.first };
          dir_cand.emplace(kp, gen_candidates(KeypadType::DIR, fi.first, ti.first));
        }
      }
      dir_ready = true;
    }
    return dir_cand;
  }
}
}

// --- Global reusable DP memo for press costs ---
namespace
{
struct CostKey
{
  int level;
  bool isDir;
  char fromK;
  char toK;
};
struct CostKeyHash
{
  size_t operator()(const CostKey &k) const noexcept
  {
    size_t h = 1469598103934665603ull; // FNV-like mix
    auto mix = [&](size_t v) { h ^= v + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2); };
    mix(std::hash<int>{}(k.level));
    mix(std::hash<int>{}(k.isDir ? 1 : 0));
    mix(std::hash<unsigned char>{}(static_cast<unsigned char>(k.fromK)));
    mix(std::hash<unsigned char>{}(static_cast<unsigned char>(k.toK)));
    return h;
  }
};
struct CostKeyEq
{
  bool operator()(const CostKey &a, const CostKey &b) const noexcept
  {
    return a.level == b.level && a.isDir == b.isDir && a.fromK == b.fromK && a.toK == b.toK;
  }
};
static std::unordered_map<CostKey, long long, CostKeyHash, CostKeyEq> PRESS_MEMO;

long long press_cost(int level, KeypadType K, char fromK, char toK)
{
  CostKey key{ level, K == KeypadType::DIR, fromK, toK };
  if (auto it = PRESS_MEMO.find(key); it != PRESS_MEMO.end())
    return it->second;

  const auto &cand_map = keypad::get_candidates_map(K);
  auto itc = cand_map.find(keypad::KeyPair{ fromK, toK });
  const auto &paths = (itc != cand_map.end()) ? itc->second : keypad::gen_candidates(K, fromK, toK);

  long long best = std::numeric_limits<long long>::max();
  for (const auto &moves : paths)
  {
    if (level == 0)
    {
      long long cost = static_cast<long long>(moves.size() + 1); // + 'A'
      if (cost < best)
        best = cost;
    }
    else
    {
      long long cost = 0;
      char prev = 'A';
      for (char ch : moves)
      {
        cost += press_cost(level - 1, KeypadType::DIR, prev, ch);
        prev = ch;
      }
      cost += press_cost(level - 1, KeypadType::DIR, prev, 'A');
      if (cost < best)
        best = cost;
    }
  }

  PRESS_MEMO.emplace(key, best);
  return best;
}
}

long long shortest_sequence(const std::string &code, int directional_keypad_layer)
{
  // Accumulate over the numeric code on the numeric keypad, starting at 'A'
  long long total = 0;
  char prevNum = 'A';
  for (char t : code)
  {
    total += press_cost(directional_keypad_layer, KeypadType::NUM, prevNum, t);
    prevNum = t;
  }
  return total;
}

int compute_code_value(const std::string &codes)
{
  int code_value = 0;
  for (char ch : codes)
  {
    if (ch >= '0' && ch <= '9')
    {
      code_value = code_value * 10 + (ch - '0');
    }
    else if (ch == 'A')
    {
      break;
    }
  }
  return code_value;
}

long long compute_code_complexity(const std::string &code, int directional_keypad_layer)
{
  int code_value = compute_code_value(code);
  long long line_complexity = shortest_sequence(code, directional_keypad_layer);
  return line_complexity * code_value;
}

long long solve(std::vector<std::string> codes, int directional_keypad_layer)
{
  long long total_complexity = 0;

  for (const auto &code : codes)
  {
    total_complexity += compute_code_complexity(code, directional_keypad_layer);
  }

  return total_complexity;
}

std::vector<std::string> read_file(const std::string &filename)
{
  std::ifstream file(filename);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line))
  {
    lines.push_back(line);
  }
  return lines;
}

int main()
{
  std::vector<std::string> codes = read_file("input.txt");

  std::cout << "=== Part1 ===" << std::endl;
  long long result1 = solve(codes, 2);
  std::cout << "Part1 result: " << result1 << std::endl;

  std::cout << "\n=== Part2 ===" << std::endl;
  long long result2 = solve(codes, 25);
  std::cout << "Part2 result: " << result2 << std::endl;

  return 0;
}
