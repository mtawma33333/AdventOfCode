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
#include "../../include/advent.hpp"

/**
 * Day 20: Race Condition
 * 地图为一条特别长、特别曲折的迷宫路径
 * 允许作弊: 可以穿墙
 * ? part1: 最多穿墙2步，计算可以节省大于等于100步的作弊方式数量
 * ? part2: 最多穿墙20步，计算可以节省大于等于100步的作弊方式数量
 */

// 常量定义
constexpr int PART1_MAX_CHEAT_STEPS = 2;
constexpr int PART2_MAX_CHEAT_STEPS = 20;
constexpr int MIN_TIME_SAVE_THRESHOLD = 100;
constexpr int MIN_CHEAT_DISTANCE = 2;

struct cell
{
  bool has_wall{ false };
  bool visited{ false };
  int distance{ 0 };
};

using race_map_row = std::vector<cell>;
using race_map = std::vector<race_map_row>;
std::vector<xy_pos_t> directions = { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

race_map get_distance_map(const race_map &map, const xy_pos_t &start)
{
  /* BFS to find the distance from start to each cell */
  race_map distance_map = map;
  std::queue<xy_pos_t> q;
  q.push(start);

  distance_map[start.first][start.second].visited = true;
  distance_map[start.first][start.second].distance = 0;

  while (!q.empty())
  {
    xy_pos_t current = q.front();
    q.pop();

    for (const auto &dir : directions)
    {
      xy_pos_t neighbor = current + dir;
      if (is_pos_on_map(neighbor, { distance_map.size(), distance_map[0].size() }) &&
          !distance_map[neighbor.first][neighbor.second].visited &&
          !distance_map[neighbor.first][neighbor.second].has_wall)
      {
        distance_map[neighbor.first][neighbor.second].visited = true;
        distance_map[neighbor.first][neighbor.second].distance =
            distance_map[current.first][current.second].distance + 1;
        q.push(neighbor);
      }
    }
  }

  return distance_map;
}

// 计算曼哈顿距离
inline int manhattan_distance(const xy_pos_t &offset)
{
  return abs(offset.first) + abs(offset.second);
}

// 检查是否为有效的作弊偏移
inline bool is_valid_cheat_offset(const xy_pos_t &offset, int max_cheat_steps)
{
  int dist = manhattan_distance(offset);
  return dist >= MIN_CHEAT_DISTANCE && dist <= max_cheat_steps;
}

// 生成所有在曼哈顿距离max_cheat_steps内的位置偏移
std::vector<xy_pos_t> get_cheat_positions(int max_cheat_steps)
{
  std::vector<xy_pos_t> positions;
  for (int dr = -max_cheat_steps; dr <= max_cheat_steps; ++dr)
  {
    for (int dc = -max_cheat_steps; dc <= max_cheat_steps; ++dc)
    {
      xy_pos_t offset{ dr, dc };
      if (is_valid_cheat_offset(offset, max_cheat_steps))
        positions.push_back(offset);
    }
  }
  return positions;
}

// 检查位置是否可以作为作弊起点
inline bool is_valid_cheat_start(const race_map &map, const race_map &distance_map, const xy_pos_t &pos)
{
  return !map[pos.first][pos.second].has_wall && distance_map[pos.first][pos.second].visited;
}

// 检查位置是否可以作为作弊终点
inline bool is_valid_cheat_end(const race_map &map, const race_map &distance_map, const xy_pos_t &pos)
{
  return is_pos_on_map(pos, { map.size(), map[0].size() }) && !map[pos.first][pos.second].has_wall &&
         distance_map[pos.first][pos.second].visited;
}

// 计算作弊节省的时间
inline int calculate_time_saved(const race_map &distance_map, const xy_pos_t &cheat_start, const xy_pos_t &cheat_end,
                                const xy_pos_t &offset, int normal_distance)
{
  int start_distance = distance_map[cheat_start.first][cheat_start.second].distance;
  int end_distance = distance_map[cheat_end.first][cheat_end.second].distance;
  int cheat_cost = manhattan_distance(offset);

  int cheat_path_length = start_distance + cheat_cost + (normal_distance - end_distance);
  return normal_distance - cheat_path_length;
}

// 处理单个作弊起点的所有可能作弊
void process_cheat_start(const race_map &original_map, const race_map &distance_map, const xy_pos_t &cheat_start,
                         const std::vector<xy_pos_t> &cheat_positions, int normal_distance, int min_time_save,
                         std::set<std::tuple<xy_pos_t, xy_pos_t>> &unique_cheats)
{
  for (const auto &offset : cheat_positions)
  {
    xy_pos_t cheat_end = cheat_start + offset;

    // 早期返回：如果不是有效的作弊终点，跳过
    if (!is_valid_cheat_end(original_map, distance_map, cheat_end))
      continue;

    // 计算节省的时间
    int time_saved = calculate_time_saved(distance_map, cheat_start, cheat_end, offset, normal_distance);

    // 如果节省时间足够，记录这个作弊路径
    if (time_saved >= min_time_save)
    {
      unique_cheats.insert(std::make_tuple(cheat_start, cheat_end));
    }
  }
}

int count_cheats(const race_map &original_map, const xy_pos_t &start, const xy_pos_t &end, int max_cheat_steps,
                 int min_time_save)
{
  // 计算距离地图和基础数据
  race_map distance_map = get_distance_map(original_map, start);
  int normal_distance = distance_map[end.first][end.second].distance;
  auto cheat_positions = get_cheat_positions(max_cheat_steps);

  std::set<std::tuple<xy_pos_t, xy_pos_t>> unique_cheats;

  // 遍历所有位置作为作弊起点
  for (int r = 0; r < original_map.size(); ++r)
  {
    for (int c = 0; c < original_map[r].size(); ++c)
    {
      xy_pos_t cheat_start{ r, c };

      // 早期返回：如果不是有效的作弊起点，跳过
      if (!is_valid_cheat_start(original_map, distance_map, cheat_start))
        continue;

      // 处理这个作弊起点的所有可能作弊
      process_cheat_start(original_map, distance_map, cheat_start, cheat_positions, normal_distance, min_time_save,
                          unique_cheats);
    }
  }

  return unique_cheats.size();
}

int part1(const race_map &original_map, const xy_pos_t &start, const xy_pos_t &end)
{
  return count_cheats(original_map, start, end, PART1_MAX_CHEAT_STEPS, MIN_TIME_SAVE_THRESHOLD);
}

int part2(const race_map &original_map, const xy_pos_t &start, const xy_pos_t &end)
{
  return count_cheats(original_map, start, end, PART2_MAX_CHEAT_STEPS, MIN_TIME_SAVE_THRESHOLD);
}

std::tuple<race_map, xy_pos_t, xy_pos_t> read_file(const std::string &filename)
{
  std::ifstream file(filename);
  race_map map;
  xy_pos_t start, end;

  if (file)
  {
    std::string line;
    while (std::getline(file, line))
    {
      race_map_row row;
      for (int col = 0; col < line.size(); ++col)
      {
        char c = line[col];
        cell new_cell;
        if (c == '#')
          new_cell.has_wall = true;
        if (c == 'S')
          start = { static_cast<int>(map.size()), col };
        if (c == 'E')
          end = { static_cast<int>(map.size()), col };
        row.push_back(new_cell);
      }
      map.push_back(row);
    }
  }
  return std::make_tuple(map, start, end);
}

void print_map(const race_map &map, const xy_pos_t &start, const xy_pos_t &end)
{
  for (int r = 0; r < map.size(); ++r)
  {
    for (int c = 0; c < map[r].size(); ++c)
    {
      if (start == xy_pos_t{ r, c })
        std::cout << 'S';
      else if (end == xy_pos_t{ r, c })
        std::cout << 'E';
      else if (map[r][c].has_wall)
        std::cout << '#';
      else
        std::cout << '.';
    }
    std::cout << std::endl;
  }
}

int main()
{
  auto [map, start, end] = read_file("input.txt");
  print_map(map, start, end);

  std::cout << "=== Part1 ===" << std::endl;
  int result1 = part1(map, start, end);
  std::cout << "Part1 result: " << result1 << std::endl;

  std::cout << "\n=== Part2 ===" << std::endl;
  int result2 = part2(map, start, end);
  std::cout << "Part2 result: " << result2 << std::endl;

  return 0;
}