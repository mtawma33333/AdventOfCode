#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

long long connect(long long a, long long b)
{
  stringstream ss;
  ss << a << b;
  return stoll(ss.str());
}

// 校验输入是否有效的函数
bool helper(long long currentValue, const vector<long long> &data, size_t index, long long testValue)
{
  if (index == data.size())
  {
    return currentValue == testValue;
  }
  if (currentValue > testValue)
  {
    return false; // 如果当前值已经超过测试值，直接返回 false
  }
  return  helper(currentValue + data[index], data, index + 1, testValue) ||
          helper(currentValue * data[index], data, index + 1, testValue) ||
          helper(connect(currentValue, data[index]), data, index + 1, testValue);
}

// 校验输入是否有效的函数
bool isValidInput(long long testValue, const vector<long long> &data)
{
  /* 想法1 暴力解法: 用数组保存计算步骤, 对于每一步和上一步相乘或相加 如果最后有等于testValue的结果返回True */
  /* 尝试1 递归解法 使用递归计算 */
  return helper(data[0], data, 1, testValue);
}

int main()
{
  ifstream inputFile("input.txt");
  if (!inputFile.is_open())
  {
    cerr << "错误：无法打开 input.txt 文件" << endl;
    return 1;
  }
  string line;
  vector<long long> testValues;      // 存储每行的测试值（冒号前的数字）
  vector<vector<long long>> numbers; // 存储每行的数字序列（冒号后的数字）

  while (getline(inputFile, line))
  {
    size_t colonPos = line.find(':');

    string testValueStr = line.substr(0, colonPos);
    long long testValue = stoll(testValueStr);
    testValues.push_back(testValue);

    string numbersStr = line.substr(colonPos + 1);
    vector<long long> lineNumbers;

    stringstream ss(numbersStr);
    string numberStr;
    while (ss >> numberStr)
    {
      long long num = stoll(numberStr);
      lineNumbers.push_back(num);
    }

    numbers.push_back(lineNumbers);
  }
  inputFile.close();
  cout << "读取到 " << testValues.size() << " 行数据。" << endl;

  /* Part 1 */
  long long sum = 0;
  int count = 0;
  for (int i = 0; i < testValues.size(); ++i)
  {
    if (isValidInput(testValues[i], numbers[i]))
    {
      sum += testValues[i];
      count++;
    }
  }
  cout << "有效输入的总和: " << sum << endl;
  cout << "有效输入的数量: " << count << endl;
  return 0;
}