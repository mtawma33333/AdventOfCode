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
#include <numeric>
/**
 * Day 23: LAN Party
 * 网络地图(输入)提供了两台计算机之间连接列表
 *   每一行文字都代表一个连接, 如 "tc-td"
 *   连接没有方向性
 * ? part1: 找出由三台相互连接的计算机组成的所有集合。统计包含名称以 t 开头的计算机的集合数量
 * ? part2: 找出最大的相互连接的计算机集合, 将其中的计算机按字母顺序排列，然后用逗号连接起来
 */

using graph = std::unordered_map<std::string, std::set<std::string>>;
using connections = std::vector<std::pair<std::string, std::string>>;
using clique = std::set<std::string>;

connections read_input(const std::string &filename)
{
  std::ifstream file(filename);
  connections connections;
  std::string line;
  while (std::getline(file, line))
  {
    std::istringstream iss(line);
    std::string a, b;
    if (std::getline(iss, a, '-') && std::getline(iss, b))
    {
      connections.emplace_back(a, b);
    }
  }
  return connections;
}

graph build_graph(const connections &connections)
{
  graph g;
  for (const auto &[a, b] : connections)
  {
    g[a].insert(b);
    g[b].insert(a);
  }
  return g;
}

std::set<clique> find_cliques(const graph &g, int size)
{
  std::set<clique> unique_cliques;

  std::function<void(clique, std::set<std::string>, int)> dfs = [&](clique current_clique,
                                                                    std::set<std::string> candidates, int d) {
    if (d == 0)
    {
      unique_cliques.insert(current_clique);
      return;
    }
    if (current_clique.size() + candidates.size() < size)
    {
      return; // 剪枝
    }

    for (const auto &node : candidates)
    {
      // 避免重复：只考虑字典序大于当前团中最大节点的候选节点
      if (!current_clique.empty() && node <= *current_clique.rbegin())
      {
        continue;
      }

      clique new_clique = current_clique;
      new_clique.insert(node);

      std::set<std::string> new_candidates;
      for (const auto &candidate : candidates)
      {
        if (g.at(node).count(candidate))
        {
          new_candidates.insert(candidate);
        }
      }

      dfs(new_clique, new_candidates, d - 1);
    }
  };

  for (const auto &[node, _] : g)
  {
    clique initial_clique;
    initial_clique.insert(node);
    dfs(initial_clique, g.at(node), size - 1);
  }

  return unique_cliques;
}

int part1(connections &connections)
{
  graph graph = build_graph(connections);

  std::set<clique> unique_cliques = find_cliques(graph, 3);

  // 统计包含名称以 t 开头的计算机的集合数量
  int count = 0;
  for (const auto &clique : unique_cliques)
  {
    if (std::ranges::any_of(clique, [](const std::string &name) { return name.starts_with('t'); }))
    {
      count++;
    }
  }
  return count;
}

std::string part2(connections &connections)
{
  graph graph = build_graph(connections);
  std::set<clique> unique_cliques;

  // 从合理的大小开始搜索（通常最大团不会太大）
  int max_degree = 0;
  for (const auto &[node, neighbors] : graph)
  {
    max_degree = std::max(max_degree, (int)neighbors.size());
  }

  // 从最大度数+1开始向下搜索
  for (int size = std::min(max_degree + 1, (int)graph.size()); size > 0; size--)
  {
    std::cout << "Searching for cliques of size " << size << "...\n";
    auto cliques = find_cliques(graph, size);
    if (!cliques.empty())
    {
      unique_cliques = cliques;
      break;
    }
  }

  /* 返回字母顺序排列后的计算机名称，用逗号连接 */
  if (!unique_cliques.empty())
  {
    // std::set已经自动按字母顺序排列，使用accumulate连接
    const auto &max_clique = *unique_cliques.begin();
    if (max_clique.empty())
      return "";

    return std::accumulate(std::next(max_clique.begin()), max_clique.end(), *max_clique.begin(),
                           [](const std::string &a, const std::string &b) { return a + "," + b; });
  }

  return "";
}

int main()
{
  auto connections = read_input("input.txt");
  int result = part1(connections);
  std::cout << "Part 1: " << result << std::endl;

  std::string result2 = part2(connections);
  std::cout << "Part 2: " << result2 << std::endl;
  return 0;
}