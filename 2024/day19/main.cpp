#include <fstream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <tuple>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <queue>
#include "../../include/advent.hpp"

/**
 * Day 19: Linen Layout
 * pattern: 用于描述条纹颜色的字符串序列
 * design: 用于描述由条纹颜色组成的布料图案的字符串序列
 * ? part1: 计算 design 中能够由 pattern 组合而成的子串数量
 */

template<typename T>
void print_vector(const std::vector<T> &vec)
{
  for (const auto &item : vec)
  {
    std::cout << item << std::endl;
  }
}

std::vector<std::string> split(const std::string &s, char delimiter)
{
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;

  while (std::getline(ss, item, delimiter))
  {
    result.push_back(item);
  }

  return result;
}

std::tuple<std::vector<std::string>, std::vector<std::string>> read_file(const std::string &filename)
{
  std::ifstream file(filename);
  std::vector<std::string> patterns; // first line of input split by ,
  std::vector<std::string> designs; // subsequent lines of input
  bool read_pattern = true;
  std::string line;

  while (std::getline(file, line))
  {
    if (read_pattern)
    {
      patterns = split(line, ',');
      for (auto &pattern : patterns)
      {
        // Remove leading/trailing whitespace
        pattern.erase(0, pattern.find_first_not_of(" \t"));
        pattern.erase(pattern.find_last_not_of(" \t") + 1);
      }
      read_pattern = false;
    }
    else if (!line.empty())
    {
      designs.push_back(line);
    }
  }

  return { patterns, designs };
}

// 检查一个design是否可以由patterns组成
bool canFormDesign(const std::string &design, const std::vector<std::string> &patterns)
{
  int n = design.length();
  std::vector<bool> dp(n + 1, false);
  dp[0] = true; // 空字符串总是可以组成的

  for (int i = 1; i <= n; ++i)
  {
    for (const auto &pattern : patterns)
    {
      int patternLen = pattern.length();
      if (i >= patternLen && dp[i - patternLen])
      {
        if (design.substr(i - patternLen, patternLen) == pattern)
        {
          dp[i] = true;
          break; 
        }
      }
    }
  }

  return dp[n];
}

long long count_design_patterns(const std::string &design, const std::vector<std::string> &patterns)
{
  int design_len = design.length();
  std::vector<long long> dp(design_len + 1, 0);
  dp[0] = 1;
  
  for (int i = 1; i <= design_len; ++i) {
    for (const auto &pattern : patterns) {
      int patternLen = pattern.length();
      if (i >= patternLen && design.substr(i - patternLen, patternLen) == pattern) {
        dp[i] += dp[i - patternLen];
      }
    }
  }
  return dp[design_len];
}

int part1(const std::vector<std::string> &patterns, const std::vector<std::string> &designs)
{
  int count = 0;
  for (const auto &design : designs)
  {
    if (canFormDesign(design, patterns))
    {
      count++;
    }
  }
  return count;
}

long long part2(const std::vector<std::string> &patterns, const std::vector<std::string> &designs)
{
  long long count = 0;
  for (const auto &design : designs)
  {
    if (!canFormDesign(design, patterns))
    {
      continue;
    }
    count += count_design_patterns(design, patterns);
  }
  return count;
}



int main()
{
  auto [patterns, designs] = read_file("input.txt");
  std::cout << "=== Part1 ===" << std::endl;
  int result1 = part1(patterns, designs);
  std::cout << "Result: " << result1 << std::endl;

  std::cout << "=== Part2 ===" << std::endl;
  long long result2 = part2(patterns, designs);
  std::cout << "Result: " << result2 << std::endl;

  return 0;
}