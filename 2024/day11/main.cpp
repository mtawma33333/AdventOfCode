#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
using namespace std;

vector<long long> parseInput(const string &filename)
{
  ifstream file(filename);
  string line;
  getline(file, line);

  vector<long long> stones;
  stringstream ss(line);
  long long num;
  while (ss >> num)
  {
    stones.push_back(num);
  }
  return stones;
}

/* Part 1 */
vector<long long> blink(long long stone)
{
  vector<long long> result;
  /* rule 1: 0 -> 1 */
  if (stone == 0)
  {
    result.push_back(1);
    return result;
  }
  /* rule 2: split into 2 stone if even number of digits */
  string stoneStr = to_string(stone);
  if (stoneStr.size() % 2 == 0)
  {
    string left = stoneStr.substr(0, stoneStr.size() / 2);
    string right = stoneStr.substr(stoneStr.size() / 2);
    long long leftNum = stoll(left);
    long long rightNum = stoll(right);
    result.push_back(leftNum);
    result.push_back(rightNum);
    return result;
  }
  /* rule 3: stone *= 2024 */
  return {stone * 2024};
}

int solvePart1(vector<long long> stones, int blinking = 1)
{
  for (int i = 0; i < blinking; ++i)
  {
    vector<long long> newStones;
    for (auto stone : stones)
    {
      auto blinked = blink(stone);
      newStones.insert(newStones.end(), blinked.begin(), blinked.end());
    }
    stones = newStones;
  }

  return stones.size();
}

/* Part 2 */
map<pair<long long, int>, long long> memo;
long long solveStoneMemo(long long stone, int blinks)
{
  if (blinks == 0) return 1; // 一个石头
  pair<long long, int> key = {stone, blinks};

  if (memo.find(key) != memo.end()) return memo[key];
  long long result = 0;
  vector<long long> nextStones = blink(stone);

  for (long long nextStone : nextStones) result += solveStoneMemo(nextStone, blinks - 1);

  memo[key] = result;
  return result;
}

long long solvePart2(vector<long long> stones, int blinking = 1)
{
  long long total = 0;
  for (auto stone : stones) total += solveStoneMemo(stone, blinking);
  return total;
}

/* main */
int main()
{
  auto stones = parseInput("input.txt");
  for (auto &stone : stones)
  {
    cout << stone << " ";
  }
  cout << endl;

  cout << "Part 1: " << solvePart1(stones, 25) << endl;
  cout << "Part 2: " << solvePart2(stones, 75) << endl;

  return 0;
}