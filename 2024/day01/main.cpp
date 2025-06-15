#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cctype>

//split a string by whitespace, handles multiple consecutive spaces
std::vector<std::string> splitByWhitespace(std::string text) {
  std::vector<std::string> result;
  std::string word = "";
  
  for (char c : text) {
    if (std::isspace(c)) {  
      if (!word.empty()) {
        result.push_back(word);
        word = "";
      }
    } else {
      word += c;
    }
  }

  if (!word.empty()) {
    result.push_back(word);
  }
  
  return result;
}

// quick sort implementation
void quickSort(std::vector<int> &arr, int left, int right) {
  if (left < right) {
    int pivot = arr[right];
    int i = left - 1;
    for (int j = left; j < right; j++) {
      if (arr[j] <= pivot) {
        i++;
        std::swap(arr[i], arr[j]);
      }
    }
    std::swap(arr[i + 1], arr[right]);
    int partitionIndex = i + 1;

    quickSort(arr, left, partitionIndex - 1);
    quickSort(arr, partitionIndex + 1, right);
  }
}

int main() {
  std::ifstream input("input.txt");
  std::string line;
  std::vector<int> left;
  std::vector<int> right;
  while (std::getline(input, line)) {
    std::vector<std::string> parts = splitByWhitespace(line);
    if (parts.size() == 2) {
      left.push_back(std::stoi(parts[0]));
      right.push_back(std::stoi(parts[1]));
    }
  }
  input.close();

  // sort left and right vectors for next steps
  quickSort(left, 0, left.size() - 1);
  quickSort(right, 0, right.size() - 1);

  /* Part 1 */

  int sum = 0;
  for (size_t i = 0; i < left.size(); ++i) {
    sum += std::abs(left[i] - right[i]);
  }
  std::cout << "Sum of distances: " << sum << std::endl;

  /* Part 2 */
  /* score = num_left * times of appearance in right */
  int similarity_score = 0;
  size_t i = 0, j = 0;
  while (i < left.size() && j < right.size()) {
    if (left[i] < right[j]) {
      i++;
    } else if (left[i] > right[j]) {
      j++;
    } else {
      // found a match
      similarity_score += left[i];
      j++;
    }
  }
  std::cout << "Similarity score: " << similarity_score << std::endl;
  return 0;
}