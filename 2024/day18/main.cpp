#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <queue>
#include "../../include/advent.hpp"

/**
 * Day 18: RAM Run
 * 地图为 70x70 的网格
 * ? Part1: 找到 网格损坏 1024 byte 后的最短路径
 * ? Part2: 找到 第一个阻断 起点到终点的 损坏网格坐标
 */
struct cell
{
  bool has_wall{ false };
  bool visited{ false };
  int distance{ 0 };
};

using memory_row = std::vector<cell>;
using memory_map = std::vector<memory_row>;

using bytes = std::vector<xy_pos_t>;

bytes read_file()
{
  bytes result;
  std::ifstream file("input.txt");
  std::string line;

  while (std::getline(file, line))
  {
    if (line.empty())
      continue;

    // 找到逗号分隔符
    size_t comma_pos = line.find(',');
    if (comma_pos != std::string::npos)
    {
      // 解析 x 和 y 坐标
      int x = std::stoi(line.substr(0, comma_pos));
      int y = std::stoi(line.substr(comma_pos + 1));

      // 添加到结果中
      result.push_back({ x, y });
    }
  }

  return result;
}

void print_map(const memory_map &map)
{
  for (const auto &row : map)
  {
    for (const auto &cell : row)
    {
      if (cell.has_wall)
        std::cout << "#";
      else
        std::cout << ".";
    }
    std::cout << std::endl;
  }
}

bool is_valid_move(const memory_map &map, int x, int y)
{
  // 边界检查
  if (x < 0 || x >= 71 || y < 0 || y >= 71)
    return false;

  // 检查是否可通行且未访问
  return !map[y][x].has_wall && !map[y][x].visited;
}

memory_map create_map(const bytes &data, int fall_byte)
{
  memory_map map(71, memory_row(71));

  for (int i = 0; i < fall_byte; ++i)
  {
    int x = data[i].first;
    int y = data[i].second;

    // 检查边界
    if (x < 0 || x >= 71 || y < 0 || y >= 71)
    {
      continue;
    }

    map[y][x].has_wall = true;
  }

  return map;
}

int bfs(memory_map &map, xy_pos_t start, xy_pos_t goal)
{
  std::queue<xy_pos_t> q;
  std::vector<xy_pos_t> directions = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 } };

  // 初始化起点
  map[start.second][start.first].visited = true;
  map[start.second][start.first].distance = 0;
  q.push(start);

  while (!q.empty())
  {
    xy_pos_t current = q.front();
    q.pop();

    // 检查是否到达目标
    if (current == goal)
      return map[current.second][current.first].distance;

    // 扩展邻居节点
    for (const auto &[dx, dy] : directions)
    {
      xy_pos_t neighbor = current + xy_pos_t{ dx, dy };
      int nx = neighbor.first; // x坐标
      int ny = neighbor.second; // y坐标

      if (is_valid_move(map, nx, ny))
      {
        map[ny][nx].visited = true;
        map[ny][nx].distance = map[current.second][current.first].distance + 1;
        q.push(neighbor);
      }
    }
  }
  // 如果无法到达目标，返回 -1
  return -1;
}

int part1(const bytes &data, int fall_byte)
{
  memory_map map = create_map(data, fall_byte);

  // 使用 BFS 寻找最短路径
  xy_pos_t start{ 0, 0 };
  xy_pos_t goal{ 70, 70 }; // 更新目标坐标
  return bfs(map, start, goal);
}

xy_pos_t part2(const bytes &data)
{
  // 二分搜索：找到第一个导致路径阻断的字节
  int left = 1;
  int right = static_cast<int>(data.size());
  int result_index = -1;

  while (left <= right)
  {
    int mid = (left + right) / 2;
    memory_map map = create_map(data, mid);

    // 使用 BFS 寻找最短路径
    xy_pos_t start{ 0, 0 };
    xy_pos_t goal{ 70, 70 };
    int distance = bfs(map, start, goal);

    if (distance == -1)
    {
      // 路径被阻断，答案在左半部分（包括当前位置）
      result_index = mid - 1; // 记录导致阻断的字节索引
      right = mid - 1;
    }
    else
    {
      // 路径仍然通畅，答案在右半部分
      left = mid + 1;
    }
  }

  if (result_index >= 0 && result_index < static_cast<int>(data.size()))
  {
    std::cout << "Part 2: path blocked at byte " << result_index << std::endl;
    return data[result_index];
  }

  return { -1, -1 }; // 如果没有找到阻断点，返回无效坐标
}

int main()
{
  // 读取输入
  bytes data = read_file();

  // 运行 Part 1
  int result = part1(data, 1024);
  std::cout << "Part 1 result: " << result << std::endl;

  // 运行 Part 2
  xy_pos_t part2_result = part2(data);
  std::cout << "Part 2 result: (" << part2_result.first << ", " << part2_result.second << ")" << std::endl;

  return 0;
}