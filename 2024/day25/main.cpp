#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <cstdint>
#include "../../include/advent.hpp"

/**
 * Day 25: Code Chronicle
 * 存在锁和钥匙，都以 7行 * 5列 的网格表示
 * - 锁：网格底层被填满，顶层留空
 * - 钥匙：网格顶层被填满，底层留空
 * - 如果锁和钥匙网格没有重叠，视为钥匙能打开锁
 * ? part1: 求 独特锁钥匙对的数量
 */

using heights_t = std::vector<int>; // 5个高度值，表示每列的高度

struct lock_key_data_t
{
  std::vector<heights_t> locks; // 所有锁的高度数据
  std::vector<heights_t> keys; // 所有钥匙的高度数据
};

lock_key_data_t load_input(const std::string &file)
{
  lock_key_data_t data;
  std::ifstream fs(file);
  std::string line;

  while (std::getline(fs, line))
  {
    if (line.empty())
      continue;

    std::vector<std::string> grid;
    grid.push_back(line);

    for (int i = 1; i < 7; ++i)
    {
      std::getline(fs, line);
      grid.push_back(line);
    }

    bool is_lock = (grid[0] == "#####" && grid[6] == ".....");
    bool is_key = (grid[0] == "....." && grid[6] == "#####");

    if (is_lock || is_key)
    {
      heights_t heights(5, 0);

      // 计算每列的高度
      for (int col = 0; col < 5; ++col)
      {
        for (int row = 0; row < 7; ++row)
        {
          if (grid[row][col] == '#')
          {
            heights[col]++;
          }
        }
      }

      if (is_lock)
      {
        data.locks.push_back(heights);
      }
      else
      {
        data.keys.push_back(heights);
      }
    }

    std::getline(fs, line);
  }

  std::cout << "Loaded " << data.locks.size() << " locks and " << data.keys.size() << " keys.\n";
  return data;
}



int part1(const lock_key_data_t &data)
{
  /* 为每一把锁匹配所有钥匙 */
  int count = 0;
  for (const auto &lock : data.locks)
  {
    for (const auto &key : data.keys)
    {
      bool can_open = true;
      for (int col = 0; col < 5; ++col)
      {
        if (lock[col] + key[col] > 7) // 锁和钥匙在同一列的高度和超过7，表示重叠
        {
          can_open = false;
          break;
        }
      }
      if (can_open)
      {
        std::cout << "Lock [";
        for (int h : lock) std::cout << h << " ";
        std::cout << "] can be opened by Key [";
        for (int h : key) std::cout << h << " ";
        std::cout << "]\n";
        count++;
      }
    }
  }

  return count;
}

int main()
{
  lock_key_data_t data = load_input("input.txt");

  std::cout << "Part 1: " << part1(data) << std::endl;
  std::cout << "Part 2: " << part2(data) << std::endl;

  return 0;
}
