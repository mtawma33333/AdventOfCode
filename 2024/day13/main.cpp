#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>

struct Machine
{
  int ax, ay;       // Button A 移动量
  int bx, by;       // Button B 移动量
  long long px, py; // Prize 位置
};

std::vector<Machine> parseInput(const std::string &filename)
{
  std::vector<Machine> machines;
  std::ifstream file(filename);
  std::string line;

  // 正则表达式匹配按钮和奖品行
  std::regex buttonRegex(R"(Button [AB]: X\+(\d+), Y\+(\d+))");
  std::regex prizeRegex(R"(Prize: X=(\d+), Y=(\d+))");

  while (std::getline(file, line))
  {
    if (line.empty())
      continue;

    Machine machine;
    std::smatch match;

    // 解析 Button A
    if (std::regex_match(line, match, buttonRegex))
    {
      machine.ax = std::stoi(match[1]);
      machine.ay = std::stoi(match[2]);

      // 解析 Button B
      std::getline(file, line);
      if (std::regex_match(line, match, buttonRegex))
      {
        machine.bx = std::stoi(match[1]);
        machine.by = std::stoi(match[2]);

        // 解析 Prize
        std::getline(file, line);
        if (std::regex_match(line, match, prizeRegex))
        {
          machine.px = std::stoll(match[1]);
          machine.py = std::stoll(match[2]);

          machines.push_back(machine);
        }
      }
    }
  }

  return machines;
}

/* Part 1 */
long long solveMachine(const Machine &machine, bool IsmaxPresses = true)
{
  long long costA = 3;
  long long costB = 1;
  long long D = machine.ax * machine.by - machine.ay * machine.bx;

  if (D == 0)
  {
    // D=0 表示无解或无穷解
    return 0;
  }

  long long Da = machine.px * machine.by - machine.py * machine.bx;
  long long Db = machine.ax * machine.py - machine.ay * machine.px;

  // 检查是否为整数解
  if (Da % D != 0 || Db % D != 0)
  {
    return 0; // 不是整数解
  }

  long long a = Da / D;
  long long b = Db / D;

  // 检查约束条件：非负且不超过最大按压次数
  if (a >= 0 && b >= 0)
  {
    if (IsmaxPresses)
    {
      // 如果需要检查最大按压次数
      long long maxPresses = 100; // 假设最大按压次数为100
      if (a > maxPresses || b > maxPresses)
      {
        return 0; // 超过最大按压次数
      }
    }
    return 3 * a + b; // A按钮3代价，B按钮1代价
  }

  return 0;
}
long long solvePart1(const std::vector<Machine> &machines)
{
  // 求解单个机器的最小代价
  long long totalCost = 0;
  for (const auto &machine : machines)
  {
    totalCost += solveMachine(machine);
  }
  return totalCost;
}

/* Part 2 */
long long solvePart2(const std::vector<Machine> &machines)
{
  long long totalCost = 0;
  for (const auto &machine : machines)
  {
    // 调整奖品位置
    Machine adjustedMachine = machine;
    adjustedMachine.px += 10000000000000LL;
    adjustedMachine.py += 10000000000000LL;

    // 求解调整后的机器
    totalCost += solveMachine(adjustedMachine, false); // 不检查最大按压次数
  }
  return totalCost;
}

int main()
{
  std::vector<Machine> machines = parseInput("input.txt");

  std::cout << "Parsed " << machines.size() << " machines" << std::endl;

  long long part1Result = solvePart1(machines);
  std::cout << "Part 1: " << part1Result << std::endl;

  long long part2Result = solvePart2(machines);
  std::cout << "Part 2: " << part2Result << std::endl;

  return 0;
}