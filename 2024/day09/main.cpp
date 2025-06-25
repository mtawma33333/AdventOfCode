#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

string readInput(const string &filename)
{
  ifstream file(filename);
  string line;
  if (getline(file, line))
  {
    return line;
  }
  return "";
}

vector<int> parseDiskMap(const string &input)
{
  vector<int> disk;
  for (size_t i = 0; i < input.size(); i += 2)
  {
    int block = input[i] - '0';
    for (int j = 0; j < block; j++)
    {
      disk.push_back(i / 2);
    }
    int space = input[i + 1] - '0';
    for (int j = 0; j < space; j++)
    {
      disk.push_back(-1); // -1 表示空闲块
    }
  }
  return disk;
}

long long calculateChecksum(const vector<int> &disk)
{
  long long checksum = 0;
  for (size_t i = 0; i < disk.size(); i++)
  {
    if (disk[i] == -1)
      continue;
    checksum += i * disk[i];
  }
  return checksum;
}

// Part 1: 压缩磁盘（逐个块移动）
vector<int> compactDiskBlocks(vector<int> disk)
{
  int left = 0;
  int right = disk.size() - 1;
  while (right >= left)
  {
    if (disk[right] == -1)
      right--;
    else if (disk[left] != -1)
      left++;
    else if (disk[left] == -1 && disk[right] != -1)
    {
      swap(disk[left], disk[right]);
      left++;
    }
    else
      right--;
  }
  return disk;
}

// Part 2: 压缩磁盘（整个文件移动）
vector<int> compactDiskFiles(vector<int> disk)
{
  int maxFileId = 0;
  for (int block : disk)
  {
    if (block != -1)
    {
      maxFileId = max(maxFileId, block);
    }
  }

  for (int fileId = maxFileId; fileId >= 0; fileId--)
  {
    int fileStart = -1, fileSize = 0;
    for (int i = 0; i < disk.size(); i++)
    {
      if (disk[i] == fileId)
      {
        if (fileStart == -1)
          fileStart = i;
        fileSize++;
      }
    }

    if (fileStart == -1)
      continue;

    int freeStart = -1;
    int currentFreeSize = 0;

    for (int i = 0; i < fileStart; i++)
    {
      if (disk[i] == -1)
      {
        if (freeStart == -1)
          freeStart = i;
        currentFreeSize++;

        if (currentFreeSize >= fileSize)
        {
          for (int j = 0; j < fileSize; j++)
          {
            disk[freeStart + j] = fileId;
            disk[fileStart + j] = -1;
          }
          break;
        }
      }
      else
      {
        freeStart = -1;
        currentFreeSize = 0;
      }
    }
  }

  return disk;
}

// 打印磁盘状态（调试用）
void printDisk(const vector<int> &disk)
{
  for (const auto &block : disk)
  {
    if (block == -1)
    {
      cout << ". ";
    }
    else
    {
      cout << block << " ";
    }
  }
  cout << endl;
}

int main()
{
  // 读取输入
  string input = readInput("input.txt");
  if (input.empty())
  {
    cout << "无法读取输入文件" << endl;
    return 1;
  }
  // 解析磁盘映射
  vector<int> originalDisk = parseDiskMap(input);
  // printDisk(originalDisk);

  // Part 1: 块级压缩
  vector<int> diskPart1 = compactDiskBlocks(originalDisk);
  long long result1 = calculateChecksum(diskPart1);
  cout << "Part 1 结果: " << result1 << endl;

  // Part 2: 文件级压缩
  vector<int> diskPart2 = compactDiskFiles(originalDisk);
  long long result2 = calculateChecksum(diskPart2);
  cout << "Part 2 结果: " << result2 << endl;

  return 0;
}