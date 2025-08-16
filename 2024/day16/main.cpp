#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "../../include/advent.hpp"

/**
 * * Day 16: Reindeer Maze
 * 驯鹿从面向东方的起始牌（S）开始，到达终点牌（E）
 * 迷宫中有墙壁（#）和通道（.），驯鹿只能在通道中移动
 * 驯鹿可以选择
 *  移动: 向面朝方向移动一格 (+1分)
 *  转向: 90度顺时针或逆时针旋转方向 (+1000分)
 * ? Part1: 如何求解通关迷宫的最小分数
 * ? Part2: 如何求解包含在迷宫最佳路径的通道 (包括起点和终点)
 */

const std::array<xy_pos_t, 4> directions = { xy_pos_t{ 1, 0 }, xy_pos_t{ 0, 1 }, xy_pos_t{ -1, 0 }, xy_pos_t{ 0, -1 } };

struct cell
{
  bool has_wall{ false };
};

struct state
{
  xy_pos_t pos{ 0, 0 };
  xy_pos_t dir{ 1, 0 }; /* 用向量表示方向 */
  long cost{ 0 };

  bool operator==(const state &other) const
  {
    return pos == other.pos && dir == other.dir;
  }
};

// 优先队列比较器
struct StateComparator
{
  bool operator()(const state &a, const state &b) const
  {
    return a.cost > b.cost;
  }
};

using maze_row_t = std::vector<cell>;
using maze_map_t = std::vector<maze_row_t>;
using start_point_t = xy_pos_t;
using end_point_t = xy_pos_t;

std::tuple<maze_map_t, start_point_t, end_point_t> read_file(std::string filename)
{
  maze_map_t maze;
  start_point_t start;
  end_point_t end;

  std::ifstream infile(filename);
  int y{ 0 };

  for (std::string line; std::getline(infile, line);)
  {
    if (line.empty())
      break;
    std::istringstream ss(line);
    maze_row_t row;
    int x{ 0 };
    char value{ 0 };
    while (ss >> value)
    {
      switch (value)
      {
      case '#':
        row.push_back(cell{ .has_wall = true });
        break;
      case 'S':
        start = { x, y };
        row.push_back(cell{});
        break;
      case 'E':
        end = { x, y };
        row.push_back(cell{});
        break;
      default:
        row.push_back(cell{});
        break;
      }
      x++;
    }
    maze.push_back(row);
    y++;
  }
  return { maze, start, end };
}

/* 单向 Dijkstra */
std::unordered_map<state, long> dijkstra_single_direction(const maze_map_t &maze, const xy_pos_t start_pos,
                                                          const xy_pos_t start_dir, bool reverse = false)
{
  std::priority_queue<state, std::vector<state>, StateComparator> pq;
  std::unordered_map<state, long> distances;
  xy_pos_t maze_dim = { static_cast<int>(maze[0].size()), static_cast<int>(maze.size()) };

  // 初始状态
  state initial_state = { start_pos, start_dir, 0 };
  pq.push(initial_state);
  distances[initial_state] = 0;

  while (!pq.empty())
  {
    state current = pq.top();
    pq.pop();

    // 如果已经有更好的距离，跳过
    auto it = distances.find(current);
    if (it != distances.end() && it->second < current.cost)
      continue;

    // 生成邻居状态 - 简化版本
    for (const auto &next_dir : directions)
    {
      // 计算移动代价：前进1分，转向1000分
      long move_cost = (next_dir == current.dir) ? 1 : 1000;
      xy_pos_t next_pos = current.pos;

      if (next_dir == current.dir && !reverse)
        next_pos = current.pos + current.dir;
      if (next_dir == current.dir && reverse)
        next_pos = current.pos - current.dir;

      // 边界和墙壁检查 - 使用头文件函数
      if (next_pos != current.pos) // 有位置移动才检查
      {
        if (!is_pos_on_map(next_pos, maze_dim) || maze[next_pos.second][next_pos.first].has_wall)
          continue;
      }

      state neighbor = { next_pos, next_dir, current.cost + move_cost };
      auto neighbor_it = distances.find(neighbor);
      if (neighbor_it == distances.end() || neighbor_it->second > neighbor.cost)
      {
        distances[neighbor] = neighbor.cost;
        pq.push(neighbor);
      }
    }
  }

  return distances;
}

/* Part 1: 找最短路径代价 */
long solve_part1(const maze_map_t &maze, const start_point_t start, const end_point_t end)
{
  auto distances = dijkstra_single_direction(maze, start, xy_pos_t{ 1, 0 }, false);

  long min_cost = LONG_MAX;
  for (const auto &[state, cost] : distances)
  {
    if (state.pos == end)
    {
      min_cost = std::min(min_cost, cost);
    }
  }

  return min_cost == LONG_MAX ? -1 : min_cost;
}

/* Part 2: 找所有最短路径经过的不重复格子数 */
long solve_part2(const maze_map_t &maze, const start_point_t start, const end_point_t end)
{
  // 正向 Dijkstra：从起点到所有状态
  auto forward_distances = dijkstra_single_direction(maze, start, xy_pos_t{ 1, 0 }, false);

  // 找到最短路径长度
  long min_cost = LONG_MAX;
  for (const auto &[state, cost] : forward_distances)
  {
    if (state.pos == end)
    {
      min_cost = std::min(min_cost, cost);
    }
  }

  if (min_cost == LONG_MAX)
    return -1;

  // 反向 Dijkstra：从所有可能的终点状态开始
  std::unordered_map<state, long> backward_distances;
  for (const auto &dir : directions)
  {
    auto back_dist = dijkstra_single_direction(maze, end, dir, true);
    for (const auto &[state, cost] : back_dist)
    {
      auto it = backward_distances.find(state);
      if (it == backward_distances.end() || it->second > cost)
      {
        backward_distances[state] = cost;
      }
    }
  }

  // 找到所有在最短路径上的状态
  std::unordered_set<xy_pos_t> optimal_cells;
  for (const auto &[state, forward_cost] : forward_distances)
  {
    auto it = backward_distances.find(state);
    if (it != backward_distances.end())
    {
      long total_cost = forward_cost + it->second;
      if (total_cost == min_cost)
      {
        optimal_cells.insert(state.pos);
      }
    }
  }

  return optimal_cells.size();
}

void print_map(const maze_map_t &maze, const start_point_t start, const end_point_t end)
{
  for (const auto &row : maze)
  {
    for (const auto &cell : row)
    {
      if (cell.has_wall)
        std::cout << '#';
      else
        std::cout << '.';
    }
    std::cout << '\n';
  }
  std::cout << "Start: (" << start.first << ", " << start.second << ")\n";
  std::cout << "End: (" << end.first << ", " << end.second << ")\n";
}

int main()
{
  auto [maze, start, end] = read_file("input.txt");

  /* Part 1 */
  long result1 = solve_part1(maze, start, end);
  std::cout << "Part 1: " << result1 << "\n";

  /* Part 2 */
  long result2 = solve_part2(maze, start, end);
  std::cout << "Part 2: " << result2 << "\n";

  return 0;
}
