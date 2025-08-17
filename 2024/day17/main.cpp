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
#include <climits>
#include "../../include/advent.hpp"

/**
 * Day 17: Chronospatial Computer
 * 时空计算机
 *    程序: 3-bit数字（0 到 7）的列表
 *    三个寄存器: A, B, C (存储整数值)
 *    指令: opcode (1个3-bit) + 后面的 operand (1个3-bit)
 *      计算机通过 instruction pointer 读取程序列表
 *      除跳转指令外，指令指针在处理每条指令后增加2
 *      如果计算机试图读取程序末尾之后的操作码, 它就会停止
 *    
 *    operand 分为 literal-operand 和 combo-operand:
 *      - literal-operand: 直接使用operand的值
 *      - combo-operand: 根据operand值解释为不同含义
 *    
 *    opcode 指令集:
 *      0: adv. 除法运算. A = A // 2^combo-operand. 结果存储在A寄存器中
 *      1: bxl. 位异或运算. B = B XOR literal-operand. 结果存储在B寄存器中
 *      2: bst. 模运算. B = combo-operand % 8. 结果存储在B寄存器中
 *      3: jnz. 跳转指令. 如果A != 0，则指令指针 = literal-operand. 如果A == 0，则正常递增指令指针
 *      4: bxc. 位异或运算. B = B XOR C. (operand被忽略但仍然存在). 结果存储在B寄存器中
 *      5: out. 输出指令. 输出 combo-operand % 8 的值 (多个值用逗号分隔)
 *      6: bdv. 除法运算. B = A // 2^combo-operand. 结果存储在B寄存器中
 *      7: cdv. 除法运算. C = A // 2^combo-operand. 结果存储在C寄存器中
 *    
 *    combo-operand 解释:
 *      0: 字面值 0
 *      1: 字面值 1
 *      2: 字面值 2
 *      3: 字面值 3
 *      4: A寄存器的值
 *      5: B寄存器的值
 *      6: C寄存器的值
 *      7: 保留值，不会出现在有效程序中
 */

using instruction = std::pair<int, int>;
using program = std::vector<int>;

struct computer
{
  long long a_register;
  long long b_register;
  long long c_register;
  int instruction_pointer;

  computer() : a_register(0), b_register(0), c_register(0), instruction_pointer(0)
  {
  }

  private:
  long long get_combo_value(int operand)
  {
    switch (operand)
    {
    case 0:
    case 1:
    case 2:
    case 3:
      return operand;
    case 4:
      return a_register;
    case 5:
      return b_register;
    case 6:
      return c_register;
    default:
      throw std::runtime_error("Invalid combo operand: " + std::to_string(operand));
    }
  }

  public:
  void print_registers() const
  {
    std::cout << "Register A: " << a_register << std::endl;
    std::cout << "Register B: " << b_register << std::endl;
    std::cout << "Register C: " << c_register << std::endl;
  }

  std::vector<int> execute(const program &prog)
  {
    std::vector<int> output;
    instruction_pointer = 0;

    while (instruction_pointer < static_cast<int>(prog.size()) - 1)
    {
      int opcode = prog[instruction_pointer];
      int operand = prog[instruction_pointer + 1];

      switch (opcode)
      {
      case 0: // adv - A除法
        a_register = a_register / (1 << get_combo_value(operand));
        break;

      case 1: // bxl - B异或字面值
        b_register = b_register ^ operand;
        break;

      case 2: // bst - B设为combo值模8
        b_register = get_combo_value(operand) % 8;
        break;

      case 3: // jnz - 条件跳转
        if (a_register != 0)
        {
          instruction_pointer = operand;
          continue; // 跳过正常的指针递增
        }
        break;

      case 4: // bxc - B异或C
        b_register = b_register ^ c_register;
        break;

      case 5: // out - 输出
        output.push_back(get_combo_value(operand) % 8);
        break;

      case 6: // bdv - B除法
        b_register = a_register / (1 << get_combo_value(operand));
        break;

      case 7: // cdv - C除法
        c_register = a_register / (1 << get_combo_value(operand));
        break;
      }

      instruction_pointer += 2;
    }

    return output;
  }

  long long find_initial_a(const program &prog)
  {
    int shift_amount = find_adv_operand(prog);
    if (shift_amount == -1)
    {
      std::cerr << "No adv instruction found in program!" << std::endl;
      return -1;
    }

    return search_recursive(prog, prog.size() - 1, 0, shift_amount);
  }

  private:
  int find_adv_operand(const program &prog)
  {
    for (size_t i = 0; i < prog.size() - 1; i += 2)
    {
      if (prog[i] == 0) // adv指令
      {
        int operand = prog[i + 1];
        // 只支持literal operand 1-3，因为combo operand在这里会很复杂
        if (operand >= 1 && operand <= 3)
        {
          return operand;
        }
        else if (operand == 0)
        {
          return 0; // A = A // 1，A不会改变，可能会无限循环
        }
        else
        {
          std::cerr << "Warning: adv instruction uses combo operand " << operand << ", this is complex!" << std::endl;
          return 3; // 默认假设是3，但可能不准确
        }
      }
    }
    return -1; // 没有找到adv指令
  }

  long long search_recursive(const program &target, int pos, long long current_a, int shift_amount)
  {
    // 递归终止条件：如果已经处理完所有位置
    if (pos < 0)
    {
      return current_a;
    }

    // 根据shift_amount确定搜索范围
    int max_digit = (1 << shift_amount) - 1; // 2^shift_amount - 1

    // 尝试当前位置的所有可能值 (0 到 max_digit)
    for (int digit = 0; digit <= max_digit; digit++)
    {
      // 构建新的A值：左移shift_amount位 + 当前数字
      long long new_a = (current_a << shift_amount) | digit;

      // 测试这个A值是否能产生正确的输出
      if (test_single_output(target, pos, new_a, shift_amount))
      {
        // 递归搜索下一个位置
        long long result = search_recursive(target, pos - 1, new_a, shift_amount);
        if (result != -1)
        {
          return result;
        }
      }
    }

    return -1; // 没有找到有效解
  }

  bool test_single_output(const program &target, int expected_pos, long long test_a, int shift_amount)
  {
    // 创建临时计算机状态
    computer temp = *this;
    temp.a_register = test_a;
    temp.b_register = 0;
    temp.c_register = 0;
    temp.instruction_pointer = 0;

    // 运行程序
    std::vector<int> output = temp.execute(target);

    // 检查输出长度
    if (output.empty())
    {
      return false;
    }

    // 计算预期的输出位置
    // 由于我们从后往前搜索，需要映射到正确的输出位置
    int cycles_expected = target.size() - expected_pos;
    int output_pos = output.size() - cycles_expected;

    if (output_pos < 0 || output_pos >= static_cast<int>(output.size()))
    {
      return false;
    }

    // 检查这个位置的输出是否匹配目标
    return output[output_pos] == target[expected_pos];
  }

  public:
};

std::tuple<computer, program> read_file(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  computer comp;
  program prog;
  std::string line;

  // 读取寄存器A
  if (std::getline(file, line))
  {
    if (auto pos = line.find("Register A: "); pos != std::string::npos)
    {
      comp.a_register = std::stoi(line.substr(pos + 12));
    }
  }

  // 读取寄存器B
  if (std::getline(file, line))
  {
    if (auto pos = line.find("Register B: "); pos != std::string::npos)
    {
      comp.b_register = std::stoi(line.substr(pos + 12));
    }
  }

  // 读取寄存器C
  if (std::getline(file, line))
  {
    if (auto pos = line.find("Register C: "); pos != std::string::npos)
    {
      comp.c_register = std::stoi(line.substr(pos + 12));
    }
  }

  // 跳过空行
  std::getline(file, line);

  // 读取程序
  if (std::getline(file, line))
  {
    if (auto pos = line.find("Program: "); pos != std::string::npos)
    {
      std::string program_str = line.substr(pos + 9);
      std::stringstream ss(program_str);
      std::string token;

      while (std::getline(ss, token, ','))
      {
        prog.push_back(std::stoi(token));
      }
    }
  }
  return { comp, prog };
}

void print_output(const std::vector<int> &output)
{
  if (output.empty())
  {
    std::cout << "\n";
    return;
  }

  for (size_t i = 0; i < output.size(); ++i)
  {
    if (i > 0)
      std::cout << ",";
    std::cout << output[i];
  }
  std::cout << "\n";
}

void print_program(const program &prog)
{
  for (size_t i = 0; i < prog.size(); ++i)
  {
    if (i > 0)
      std::cout << ",";
    std::cout << prog[i];
  }
  std::cout << "\n";
}

int main()
{
  auto [computer, program] = read_file("input.txt");

  // Part 1: 使用输入文件中的初始值
  std::cout << "=== Part 1 ===" << std::endl;
  computer.print_registers();
  std::cout << "Program: ";
  print_program(program);

  auto comp_copy = computer; // 保存原始状态
  std::vector<int> output = comp_copy.execute(program);
  std::cout << "Output: ";
  print_output(output);

  // Part 2: 寻找能让程序输出自己的初始A值
  std::cout << "\n=== Part 2 ===" << std::endl;
  std::cout << "Searching for initial A value..." << std::endl;

  computer.a_register = 0; // 重置搜索状态
  computer.b_register = 0;
  computer.c_register = 0;

  long long result_a = computer.find_initial_a(program);

  if (result_a != -1)
  {
    std::cout << "Found initial A value: " << result_a << std::endl;

    // 验证结果
    computer.a_register = result_a;
    computer.b_register = 0;
    computer.c_register = 0;
    std::vector<int> verify_output = computer.execute(program);

    std::cout << "Verification - Target: ";
    print_program(program);
    std::cout << "Verification - Output: ";
    print_output(verify_output);
  }
  else
  {
    std::cout << "❌ No solution found." << std::endl;
  }

  return 0;
}