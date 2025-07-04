#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <set>
#include <map>

using namespace std;

// 方向数组：上、右、下、左
int dx[] = {-1, 0, 1, 0};
int dy[] = {0, 1, 0, -1};

bool isValid(int x, int y, vector<string> grid)
{
  int rows = grid.size();
  int cols = grid[0].size();
  return x >= 0 && x < rows && y >= 0 && y < cols;
}

vector<string> parseInput(const string &filename)
{
  vector<string> grid;
  ifstream file(filename);
  string line;
  while (getline(file, line))
  {
    if (!line.empty())
    {
      grid.push_back(line);
    }
  }
  file.close();
  return grid;
}

/* Part 1 */
pair<int, int> BFS(int startX, int startY, const vector<string> &grid, vector<vector<bool>> &visited)
{
  int rows = grid.size();
  int cols = grid[0].size();
  char currentRegion = grid[startX][startY];
  queue<pair<int, int>> quene;
  quene.push({startX, startY});
  visited[startX][startY] = true;
  int area = 0;
  int perimeter = 0;

  while (!quene.empty())
  {
    int x = quene.front().first;
    int y = quene.front().second;
    quene.pop();
    area++;

    for (int d = 0; d < 4; ++d)
    {
      int newX = x + dx[d];
      int newY = y + dy[d];

      if (isValid(newX, newY, grid))
      {
        if (grid[newX][newY] == currentRegion && !visited[newX][newY])
        {
          visited[newX][newY] = true;
          quene.push({newX, newY});
        }
        else if (grid[newX][newY] != currentRegion)
        {
          perimeter++;
        }
      }
      else
      {
        perimeter++;
      }
    }
  }
  return make_pair(area, perimeter);
}

int solvePart1(const vector<string> &grid)
{
  int rows = grid.size();
  int cols = grid[0].size();
  int totalcost = 0;
  vector<vector<bool>> visited(rows, vector<bool>(cols, false));

  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      if (visited[i][j] == false)
      {
        pair<int, int> result = BFS(i, j, grid, visited);
        int area = result.first;
        int perimeter = result.second;
        totalcost += area * perimeter;
      }
    }
  }

  return totalcost;
}

/* Part 2 */
pair<int, int> BFS_WithSides(int startX, int startY, const vector<string> &grid, vector<vector<bool>> &visited)
{
  int rows = grid.size();
  int cols = grid[0].size();
  char currentRegion = grid[startX][startY];
  queue<pair<int, int>> quene;
  quene.push({startX, startY});
  visited[startX][startY] = true;

  vector<pair<int, int>> regionCells; // 存储区域内的所有格子

  // BFS收集区域内所有格子
  while (!quene.empty())
  {
    int x = quene.front().first;
    int y = quene.front().second;
    quene.pop();
    regionCells.push_back({x, y});

    for (int d = 0; d < 4; ++d)
    {
      int newX = x + dx[d];
      int newY = y + dy[d];

      if (isValid(newX, newY, grid) && grid[newX][newY] == currentRegion && !visited[newX][newY])
      {
        visited[newX][newY] = true;
        quene.push({newX, newY});
      }
    }
  }

  int area = regionCells.size();

  // 计算边数：使用角点计数法
  set<pair<int, int>> regionSet(regionCells.begin(), regionCells.end());
  int corners = 0;

  // 检查所有可能的角点
  for (int cx = 0; cx <= rows; ++cx)
  {
    for (int cy = 0; cy <= cols; ++cy)
    {
      // 检查角点(cx,cy)周围的4个格子
      vector<bool> around = {
          regionSet.count({cx - 1, cy - 1}) > 0, // 左上
          regionSet.count({cx - 1, cy}) > 0,     // 右上
          regionSet.count({cx, cy - 1}) > 0,     // 左下
          regionSet.count({cx, cy}) > 0          // 右下
      };

      int count = 0;
      for (bool b : around)
        if (b)
          count++;

      // 角点规则：1个或3个相邻格子属于区域时形成角
      if (count == 1 || count == 3)
      {
        corners++;
      }
      // 特殊情况：对角线情况也可能形成角
      else if (count == 2)
      {
        // 检查是否是对角线情况 (左上右下 或 右上左下)
        if ((around[0] && around[3] && !around[1] && !around[2]) ||
            (!around[0] && !around[3] && around[1] && around[2]))
        {
          corners += 2; // 对角线情况贡献2个角
        }
      }
    }
  }

  return make_pair(area, corners);
}

int solvePart2(const vector<string> &grid)
{
  int rows = grid.size();
  int cols = grid[0].size();
  int totalcost = 0;
  vector<vector<bool>> visited(rows, vector<bool>(cols, false));

  for (int i = 0; i < rows; ++i)
  {
    for (int j = 0; j < cols; ++j)
    {
      if (visited[i][j] == false)
      {
        pair<int, int> result = BFS_WithSides(i, j, grid, visited);
        int area = result.first;
        int sides = result.second;
        totalcost += area * sides;
      }
    }
  }

  return totalcost;
}

int main()
{
  vector<string> grid = parseInput("input.txt");
  cout << "Part 1: " << solvePart1(grid) << endl;
  cout << "Part 2: " << solvePart2(grid) << endl;

  return 0;
}