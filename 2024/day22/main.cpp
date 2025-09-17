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

/**
 * Day 22: Monkey Market
 * input: 多行字符串，每行表示一个买家的 initial_secret
 * secret: 一个非负整数
 * evolves: 接受一个 secret，返回下一个 secret
 * mix: 把给定值与当前 secret 做按位异或，结果作为新的 secret
 * prune: 把当前 secret 对 16777216 取模，结果作为新的 secret 
 * evolves 过程:
 * - 计算 secret * 64。然后对结果 mix ,最后 prune
 * - 计算 secret // 32。然后对结果 mix ,最后 prune
 * - 计算 secret * 2048。然后对结果 mix ,最后 prune
 * 
 * ? part1: 对每个买家，计算其 initial_secret 经过 2000 次 evolves 后的 secret 之和
 * price: 由 secret 的个位数字决定
 * 条件: 只能选定 1 个长度为 4 的价格变化序列作为 交易目标
 * 当买家的 price 变化首次与交易目标匹配时，立刻卖出, 收益为该买家的当前 price
 * ? part2: 求最大收益
 */

long long evolve(long long secret)
{
  secret = ((secret * 64) ^ secret) & 0xFFFFFF;
  secret = ((secret / 32) ^ secret) & 0xFFFFFF;
  secret = ((secret * 2048) ^ secret) & 0xFFFFFF;
  return secret;
}

long long part1(const std::vector<long long> &initial_secrets)
{
  long long total = 0;
  for (long long secret : initial_secrets)
  {
    for (int i = 0; i < 2000; ++i)
    {
      secret = evolve(secret);
    }
    total += secret;
  }
  return total;
}

long long part2(const std::vector<long long> &initial_secrets)
{
  // 变化取值范围 [-9, 9]，偏移到 [0, 18] 后用 base-19 打包四元序列
  constexpr int BASE = 19;
  constexpr int K = 4;
  constexpr int KEY_SPACE = BASE * BASE * BASE * BASE;
  constexpr int P3 = BASE * BASE * BASE;

  // 全局收益表：key -> 该序列在所有买家的“首次命中价格”的和
  std::vector<long long> totals(KEY_SPACE, 0);

  // 时间戳数组：seenStamp[key] == stamp 表示“当前买家已见过该序列”
  std::vector<int> seenStamp(KEY_SPACE, 0);
  int stamp = 0; // 每处理一个买家 ++stamp，相当于 O(1) 清空 per-buyer set

  for (size_t buyerIdx = 0; buyerIdx < initial_secrets.size(); ++buyerIdx)
  {
    ++stamp; // 处理新买家，时间戳加一
    long long secret = initial_secrets[buyerIdx];
    int prev_price = static_cast<int>(secret % 10);

    int key = 0;
    int filled = 0;

    for (int step = 1; step <= 2000; ++step)
    {
      secret = evolve(secret);
      int price = static_cast<int>(secret % 10);
      int d = price - prev_price;
      int e = d + 9;

      if (filled < K)
      {
        key = key * BASE + e;
        ++filled;
      }
      else
      {
        key = (key % P3) * BASE + e;
      }
      if (filled == K)
      {
        if (seenStamp[key] != stamp)
        {
          totals[key] += price; // 在实际实现中打开本行
          seenStamp[key] = stamp; // 记为已见
        }
      }

      prev_price = price; // 滚动上一价格
    }
  }
  long long best = 0;
  for (auto v : totals)
    best = std::max(best, v);
  return best;
}

std::vector<long long> read_file(const std::string &filename)
{
  std::ifstream file(filename);
  std::vector<long long> secrets;
  std::string line;
  while (std::getline(file, line))
  {
    if (!line.empty())
    {
      secrets.push_back(std::stoi(line));
    }
  }
  return secrets;
}

int main()
{
  std::vector<long long> initial_secrets = read_file("input.txt");
  std::cout << "Part 1: " << part1(initial_secrets) << std::endl;
  std::cout << "Part 2: " << part2(initial_secrets) << std::endl;
  return 0;
}