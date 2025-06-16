#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cctype>
#include <cmath>

// split a string by whitespace, handles multiple consecutive spaces
std::vector<std::string> splitByWhitespace(std::string s)
{
  std::vector<std::string> words;
  std::istringstream iss(s);
  std::string word;

  while (iss >> word)
  {
    words.push_back(word);
  }
  return words;
}

bool isValidReport(const std::vector<int> &levels)
{
  /*cond1: report is ascend or descend */
  /*cond2: Any two adjacent levels differ by at least one and at most three */

  bool isAscending = true;
  bool isDescending = true;
  for (size_t i = 1; i < levels.size(); ++i)
  {
    if (levels[i] < levels[i - 1])
    {
      isAscending = false;
    }
    if (levels[i] > levels[i - 1])
    {
      isDescending = false;
    }
    if (std::abs(levels[i] - levels[i - 1]) < 1 || std::abs(levels[i] - levels[i - 1]) > 3)
    {
      return false; // condition 2 fails
    }
  }
  return isAscending || isDescending;
}

bool isValidReportWithOneRemoval(const std::vector<int> &levels)
{
  /* Check if we can remove one level to make the report valid */
  for (size_t i = 0; i < levels.size(); ++i)
  {
    std::vector<int> modifiedLevels = levels;
    modifiedLevels.erase(modifiedLevels.begin() + i);
    if (isValidReport(modifiedLevels))
    {
      return true;
    }
  }
  return false;
}

int main()
{
  /* one report per line. Each report is a list of numbers called levels that are separated by spaces. */
  std::ifstream input("input.txt");
  std::string line;
  std::vector<int> levels;
  std::vector<std::vector<int>> reports;

  while (std::getline(input, line))
  {
    std::vector<std::string> parts = splitByWhitespace(line);
    for (const auto &part : parts)
    {
      levels.push_back(std::stoi(part));
    }
    reports.push_back(levels);
    levels.clear();
  }
  input.close();

  /* Part 1 */
  int validReportsCount = 0;
  for (const auto &report : reports)
  {
    if (isValidReport(report))
    {
      validReportsCount++;
    }
  }

  /* Part 2 */
  int validReportsWithOneRemovalCount = 0;
  for (const auto &report : reports)
  {
    if (isValidReportWithOneRemoval(report))
    {
      validReportsWithOneRemovalCount++;
    }
  }

  std::cout << "Number of valid reports: " << validReportsCount << std::endl;
  std::cout << "Number of valid reports with one removal: " << validReportsWithOneRemovalCount << std::endl;

  return 0;
}