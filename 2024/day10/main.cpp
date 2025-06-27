#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

vector<string> readInput(const string &filename)
{
  ifstream file(filename);
  vector<string> grid;
  string line;
  while (getline(file, line))
  {
    grid.push_back(line);
  }
  return grid;
}

/* Part 1 */
set<pair<int, int>> dfs(const vector<string> &grid, int x, int y)
{
  /* 1. 检查边界 */
  if (x < 0 || x >= grid.size() || y < 0 || y >= grid[0].size())
    return {};

  /* 2. 检查是否为终点 */
  if (grid[x][y] == '9')
    return {{x, y}};

  set<pair<int, int>> result;
  const int dx[] = {-1, 1, 0, 0};
  const int dy[] = {0, 0, -1, 1};

  /* 3. 深度优先搜索四个方向 */
  for (int i = 0; i < 4; i++)
  {
    int nx = x + dx[i], ny = y + dy[i];
    if (nx >= 0 && nx < grid.size() && ny >= 0 && ny < grid[0].size() &&
        grid[nx][ny] == grid[x][y] + 1)
    {
      auto reachable = dfs(grid, nx, ny);
      result.insert(reachable.begin(), reachable.end());
    }
  }
  return result;
}

int findTrailheads(const vector<string> &grid)
{
  int totalScore = 0;
  /* 1. 遍历网格 */
  for (int i = 0; i < grid.size(); ++i)
  {
    for (int j = 0; j < grid[i].size(); ++j)
    {
      /* 2. 检查是否为起点 */
      char cell = grid[i][j];
      if (cell == '0')
      {
        /* 3. 计算得分 */
        totalScore += dfs(grid, i, j).size();
      }
    }
  }
  return totalScore;
}
/* Part 2 */
int dfsRating(const vector<string> &grid, int x, int y)
{
  /* 1. 检查边界 */
  if (x < 0 || x >= grid.size() || y < 0 || y >= grid[0].size())
    return 0;

  /* 2. 检查是否为终点 */
  if (grid[x][y] == '9')
    return 1;

  int rating = 0;
  const int dx[] = {-1, 1, 0, 0};
  const int dy[] = {0, 0, -1, 1};

  /* 3. 深度优先搜索四个方向 */
  for (int i = 0; i < 4; i++)
  {
    int nx = x + dx[i], ny = y + dy[i];
    if (nx >= 0 && nx < grid.size() && ny >= 0 && ny < grid[0].size() &&
        grid[nx][ny] == grid[x][y] + 1)
    {
      rating += dfsRating(grid, nx, ny);
    }
  }
  
  return rating;
}

int findTrailRatings(const vector<string> &grid)
{

  int total = 0;
  for (size_t i = 0; i < grid.size(); i++)
  {
    for (size_t j = 0; j < grid[i].size(); j++)
    {
      if (grid[i][j] == '0')
      {
        // Part 2 需要不同的DFS逻辑
        total += dfsRating(grid, i, j);
      }
    }
  }
  return total;
}

int main()
{
  vector<string> grid = readInput("input.txt");

  int result1 = findTrailheads(grid);
  cout << "Part 1: " << result1 << endl;

  int result2 = findTrailRatings(grid);
  cout << "Part 2: " << result2 << endl;

  return 0;
}