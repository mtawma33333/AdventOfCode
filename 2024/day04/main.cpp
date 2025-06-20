#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

/**
 * @brief 对输入网格进行 8 个方向的单词搜索 单词可以互相重叠
 * @param grid 输入的二维字符数组
 * @param word 要搜索的单词
 * @param row 起始行索引
 * @param col 起始列索引
 * @return 匹配到的单词数量
 */
int searchWord(const vector<string>& grid, const string& word, int row, int col) {
    int directions[8][2] = {
        {0, 1},   // 右
        {1, 1},   // 右下
        {1, 0},   // 下
        {1, -1},  // 左下
        {0, -1},  // 左
        {-1, -1}, // 左上
        {-1, 0},  // 上
        {-1, 1}   // 右上
    };
    int rows = grid.size();
    int cols = grid[0].size();
    int wordLength = word.length();
    int matchCount = 0;
    for (int direction = 0; direction < 8; ++direction) {
        int dRow = directions[direction][0];
        int dCol = directions[direction][1];
        int r = row, c = col;
        int k;
        
        // 检查当前方向是否匹配单词
        for (k = 0; k < wordLength; ++k) {
            if (r < 0 || r >= rows || c < 0 || c >= cols || grid[r][c] != word[k]) {
                break;
            }
            r += dRow;
            c += dCol;
        }
        
        if (k == wordLength) {
            matchCount++;
        }
    }
    return matchCount;
}

/**
 * @brief 对输入网格进行 形成X-形的单词搜索
 * @param grid 输入的二维字符数组
 * @param word 要搜索的单词
 * @param row 起始行索引
 * @param col 起始列索引
 * @return bool 是否找到形成X-形的单词
 */
int searchWordX(const vector<string>& grid, const string& word, int row, int col) {
    int rows = grid.size();
    int cols = grid[0].size();
    int wordLength = word.length();
    
    // 检查是否可以形成X-形
    if (wordLength < 3 || row < 1 || row >= rows - 1 || col < 1 || col >= cols - 1) {
        return 0;
    }
    
    // 检查中心字符
    if (grid[row][col] != word[1]) {
        return 0;
    }
    
    // 检查对角线方向的字符组合
    // 两个对角线方向都必须形成"MAS"或"SAM" 即 左上是头或尾 右下是尾或头
    return ((grid[row - 1][col - 1] == word[0] && grid[row + 1][col + 1] == word[2]) ||
            (grid[row - 1][col - 1] == word[2] && grid[row + 1][col + 1] == word[0])) &&
           ((grid[row - 1][col + 1] == word[0] && grid[row + 1][col - 1] == word[2]) ||
            (grid[row - 1][col + 1] == word[2] && grid[row + 1][col - 1] == word[0]));
}




int main() {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cerr << "无法打开文件 input.txt" << endl;
        return 1;
    }
    
    vector<string> grid;
    string line;
    
    while (getline(file, line)) {
        grid.push_back(line);
    }
    file.close();
    
    cout << "网格大小: " << grid.size() << " x " << (grid.empty() ? 0 : grid[0].size()) << endl;

    /* Part 1 */
    int result_1 = 0;
    string word = "XMAS";
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            result_1 += searchWord(grid, word, i, j);
        }
    }

    cout << "Part 1 结果: " << result_1 << endl;
    
    /* Part 2 */
    int result_2 = 0;
    word = "MAS";
    for (int i = 0; i < grid.size(); ++i) {
        for (int j = 0; j < grid[i].size(); ++j) {
            result_2 += searchWordX(grid, word, i, j);
        }
    }

    cout << "Part 2 结果: " << result_2 << endl;

    return 0;
}