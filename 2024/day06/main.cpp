#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>

using namespace std;

/* enmu for direction */
enum Direction
{
  UP = 0,
  RIGHT = 1,
  DOWN = 2,
  LEFT = 3
};

/* function for char to direction */
Direction charToDirection(char dir)
{
  switch (dir)
  {
  case '^':
    return UP;
  case '>':
    return RIGHT;
  case 'v':
    return DOWN;
  case '<':
    return LEFT;
  default:
    throw invalid_argument("无效的方向字符");
  }
}

/* fucntion for find guard and return the corr */
pair<int, int> findGuard(const vector<string> &grid)
{
  for (int i = 0; i < grid.size(); ++i)
  {
    for (int j = 0; j < grid[i].size(); ++j)
    {
      char cell = grid[i][j];
      if (cell == '^' || cell == 'v' || cell == '<' || cell == '>')
      {
        return {i, j};
      }
    }
  }
  return {-1, -1};
}

// 状态结构：位置 + 方向
struct State
{
  int row, col, dir;

  bool operator<(const State &other) const
  {
    if (row != other.row)
      return row < other.row;
    if (col != other.col)
      return col < other.col;
    return dir < other.dir;
  }

  bool operator==(const State &other) const
  {
    return row == other.row && col == other.col && dir == other.dir;
  }
};

// 预计算原始路径的详细信息
vector<State> computeOriginalPath(const vector<string> &grid, int startRow, int startCol, Direction startDir)
{
  vector<State> path;
  int currentRow = startRow;
  int currentCol = startCol;
  int currentDir = startDir;

  // 方向向量
  int directionRows[] = {-1, 0, 1, 0}; // UP, RIGHT, DOWN, LEFT
  int directionCols[] = {0, 1, 0, -1};

  while (true)
  {
    // 记录当前状态
    path.push_back({currentRow, currentCol, currentDir});

    // 计算下一个位置
    int nextRow = currentRow + directionRows[currentDir];
    int nextCol = currentCol + directionCols[currentDir];

    // 检查边界
    if (nextRow < 0 || nextRow >= grid.size() ||
        nextCol < 0 || nextCol >= grid[0].size())
    {
      break; // 离开地图
    }

    // 检查障碍物
    if (grid[nextRow][nextCol] == '#')
    {
      // 遇到障碍物，转向
      currentDir = (currentDir + 1) % 4;
    }
    else
    {
      // 前进
      currentRow = nextRow;
      currentCol = nextCol;
    }
  }

  return path;
}

// 优化的循环检测函数
bool detectLoopOptimized(const vector<string> &grid, const vector<State> &originalPath,
                         int obstacleRow, int obstacleCol, int startRow, int startCol, Direction startDir)
{
  // 找到警卫第一次会遇到新障碍物的位置
  int startIndex = -1;
  for (int i = 0; i < originalPath.size(); i++)
  {
    State state = originalPath[i];

    // 计算下一个位置
    int directionRows[] = {-1, 0, 1, 0};
    int directionCols[] = {0, 1, 0, -1};
    int nextRow = state.row + directionRows[state.dir];
    int nextCol = state.col + directionCols[state.dir];

    // 如果下一个位置是新障碍物的位置
    if (nextRow == obstacleRow && nextCol == obstacleCol)
    {
      startIndex = i;
      break;
    }
  }

  // 如果警卫永远不会遇到这个障碍物，不会形成循环
  if (startIndex == -1)
  {
    return false;
  }

  // 从遇到障碍物的位置开始模拟
  State startState = originalPath[startIndex];

  set<State> visitedStates;
  int currentRow = startState.row;
  int currentCol = startState.col;
  int currentDir = startState.dir;

  // 方向向量
  int directionRows[] = {-1, 0, 1, 0};
  int directionCols[] = {0, 1, 0, -1};

  while (true)
  {
    State currentState = {currentRow, currentCol, currentDir};

    // 检查是否形成循环
    if (visitedStates.count(currentState))
    {
      return true;
    }

    visitedStates.insert(currentState);

    // 计算下一个位置
    int nextRow = currentRow + directionRows[currentDir];
    int nextCol = currentCol + directionCols[currentDir];

    // 检查边界
    if (nextRow < 0 || nextRow >= grid.size() ||
        nextCol < 0 || nextCol >= grid[0].size())
    {
      return false; // 离开地图，不是循环
    }

    // 检查障碍物（包括新添加的）
    if (grid[nextRow][nextCol] == '#' ||
        (nextRow == obstacleRow && nextCol == obstacleCol))
    {
      // 遇到障碍物，转向
      currentDir = (currentDir + 1) % 4;
    }
    else
    {
      // 前进
      currentRow = nextRow;
      currentCol = nextCol;
    }
  }
}

int main()
{
  ifstream file("input.txt");
  if (!file.is_open())
  {
    cerr << "无法打开文件 input.txt" << endl;
    return 1;
  }

  vector<string> grid;
  string line;

  while (getline(file, line))
  {
    grid.push_back(line);
  }
  file.close();
  /* Part 1 */
  cout << "地图大小: " << grid.size() << " x " << (grid.empty() ? 0 : grid[0].size()) << endl;

  // 找到警卫的初始位置和方向
  pair<int, int> guardPosition = findGuard(grid);
  int guardRow = guardPosition.first;
  int guardCol = guardPosition.second;
  char guardDirection = grid[guardRow][guardCol];

  if (guardRow == -1 || guardCol == -1)
  {
    cerr << "未找到警卫的初始位置" << endl;
    return 1;
  }

  cout << "警卫初始位置: (" << guardRow << ", " << guardCol << "), 方向: " << guardDirection << endl;

  int currentDirection = charToDirection(guardDirection);

  set<pair<int, int>> visitedPositions;

  int currentRow = guardRow;
  int currentCol = guardCol;

  // 移动规则：
  // 1. 如果前方没有障碍物('#')，向前移动一步
  // 2. 如果前方有障碍物，向顺时针转90度
  // 3. 如果走出地图边界，停止移动
  // 4. 记录每个访问过的位置
  visitedPositions.insert({currentRow, currentCol}); // 记录起始位置
  while (true)
  {
    int nextRow = currentRow;
    int nextCol = currentCol;
    switch (currentDirection)
    {
    case UP:
      nextRow--;
      break;
    case RIGHT:
      nextCol++;
      break;
    case DOWN:
      nextRow++;
      break;
    case LEFT:
      nextCol--;
      break;
    }
    if (nextRow < 0 || nextRow >= grid.size() || nextCol < 0 || nextCol >= grid[0].size())
    {
      // 如果走出地图边界，停止移动
      break;
    }
    if (grid[nextRow][nextCol] == '#')
    {
      // 如果前方有障碍物，向顺时针转90度
      currentDirection = (currentDirection + 1) % 4;
    }
    else
    {
      // 否则，向前移动一步
      currentRow = nextRow;
      currentCol = nextCol;
      visitedPositions.insert({currentRow, currentCol});
    }
  }

  cout << "警卫访问了 " << visitedPositions.size() << " 个不同的位置" << endl;

  /* Part 2 */

  // 预计算原始路径
  vector<State> originalPath = computeOriginalPath(grid, guardRow, guardCol, charToDirection(guardDirection));

  int loopCount = 0;

  // 只在原始路径经过的位置测试障碍物
  for (const auto &pos : visitedPositions)
  {
    int testRow = pos.first;
    int testCol = pos.second;

    // 跳过起始位置
    if (testRow == guardRow && testCol == guardCol)
    {
      continue;
    }

    // 使用优化的循环检测
    if (detectLoopOptimized(grid, originalPath, testRow, testCol, guardRow, guardCol, charToDirection(guardDirection)))
    {
      loopCount++;
    }
  }

  cout << "Part 2 结果: " << loopCount << endl;

  return 0;
}
