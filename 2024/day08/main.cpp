#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cmath>

using namespace std;

struct Point {
    int x, y;
    
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// 检查点是否在地图范围内
bool isInBounds(const Point& p, int rows, int cols) {
    return p.x >= 0 && p.x < rows && p.y >= 0 && p.y < cols;
}

// 计算两个天线产生的反节点
vector<Point> calculateAntinodes(const Point& antenna1, const Point& antenna2) {
    vector<Point> antinodes;
    
    int dx = antenna2.x - antenna1.x;
    int dy = antenna2.y - antenna1.y;
    
    // 反节点1：从天线1出发，延伸2倍距离
    int antinode1_x = antenna1.x + 2 * dx;
    int antinode1_y = antenna1.y + 2 * dy;
    
    // 反节点2：从天线2出发，延伸2倍距离（反方向）
    int antinode2_x = antenna2.x - 2 * dx;
    int antinode2_y = antenna2.y - 2 * dy;
    
    Point antinode1(antinode1_x, antinode1_y);
    Point antinode2(antinode2_x, antinode2_y);
    antinodes.push_back(antinode1);
    antinodes.push_back(antinode2);
    return antinodes;
}

vector<Point> calculateAntinodesPartTwo(const Point& antenna1, const Point& antenna2, int rows, int cols) {
    vector<Point> antinodes;

    int dx = antenna2.x - antenna1.x;
    int dy = antenna2.y - antenna1.y;

    // 从antenna1开始，向两个方向延伸
    // 正方向
    for (int i = 0;; i++)
    {
      Point p(antenna1.x + i * dx, antenna1.y + i * dy);
      if (!isInBounds(p, rows, cols))
        break;
      antinodes.push_back(p);
    }

    // 负方向
    for (int i = 1;; i++)
    {
      Point p(antenna1.x - i * dx, antenna1.y - i * dy);
      if (!isInBounds(p, rows, cols))
        break;
      antinodes.push_back(p);
    }

    return antinodes;
}

// 找到所有相同频率天线产生的反节点
set<Point> findAntinodesForFrequency(const vector<Point>& antennas, int rows, int cols) {
    set<Point> antinodes;
    
    // 对于每对相同频率的天线，计算它们产生的反节点
    // 只保留在地图范围内的反节点
    
    for (size_t i = 0; i < antennas.size(); i++) {
        for (size_t j = i + 1; j < antennas.size(); j++) {
            vector<Point> nodesPair = calculateAntinodes(antennas[i], antennas[j]);
            for (const Point& node : nodesPair) {
                if (isInBounds(node, rows, cols)) {
                    antinodes.insert(node);
                }
            }
        }
    }
    
    return antinodes;
}

set<Point> findAntinodesForFrequencyPartTwo(const vector<Point>& antennas, int rows, int cols) {
    set<Point> antinodes;

    for (size_t i = 0; i < antennas.size(); i++) {
        for (size_t j = i + 1; j < antennas.size(); j++) {
            vector<Point> nodesPair = calculateAntinodesPartTwo(antennas[i], antennas[j], rows, cols);
            for (const Point& node : nodesPair) {
                if (isInBounds(node, rows, cols)) {
                    antinodes.insert(node);
                }
            }
        }
    }

    return antinodes;
}

int main() {
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cerr << "错误：无法打开 input.txt 文件" << endl;
        return 1;
    }
    
    vector<string> grid;
    string line;
    
    // 读取地图
    while (getline(inputFile, line)) {
        grid.push_back(line);
    }
    inputFile.close();
    
    if (grid.empty()) {
        cerr << "错误：地图为空" << endl;
        return 1;
    }
    
    int rows = grid.size();
    int cols = grid[0].size();
    
    cout << "地图大小: " << rows << " x " << cols << endl;
    
    // 按频率分组收集天线位置
    map<char, vector<Point>> antennasByFreq;
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            char ch = grid[i][j];
            if (ch != '.') {  // 不是空地，就是天线
                antennasByFreq[ch].push_back(Point(i, j));
            }
        }
    }
    
    cout << "找到 " << antennasByFreq.size() << " 种不同频率的天线:" << endl;
    
    // 收集所有反节点
    set<Point> allAntinodes;
    
    for (const auto& pair : antennasByFreq) {
        char freq = pair.first;
        const vector<Point>& antennas = pair.second;
        
        if (antennas.size() < 2) {
            continue;  // 需要至少两个天线才能产生反节点
        }
        
        set<Point> freqAntinodes = findAntinodesForFrequency(antennas, rows, cols);
        cout << "频率 '" << freq << "' 产生 " << freqAntinodes.size() << " 个反节点" << endl;
        
        // 合并到总集合中
        allAntinodes.insert(freqAntinodes.begin(), freqAntinodes.end());
    }
    
    cout << "\n地图范围内总反节点数量: " << allAntinodes.size() << endl;
    
    // 打印带反节点的地图用于调试
    cout << "\n带反节点的地图 (#表示反节点):" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Point current(i, j);
            if (allAntinodes.count(current) && grid[i][j] == '.') {
                cout << '#';
            } else {
                cout << grid[i][j];
            }
        }
        cout << endl;
    }

    /* Part 2 */
    set<Point> allAntinodes2;
    for (const auto& pair : antennasByFreq) {
        char freq = pair.first;
        const vector<Point>& antennas = pair.second;
        if (antennas.size() < 2) {
            continue;  // 需要至少两个天线才能产生反节点
        }
        set<Point> freqAntinodes = findAntinodesForFrequencyPartTwo(antennas, rows, cols);
        // 合并到总集合中
        allAntinodes2.insert(freqAntinodes.begin(), freqAntinodes.end());
    }
    cout << "\nPart 2 - 地图范围内总反节点数量: " << allAntinodes2.size() << endl;
    cout << "\nPart 2 - 带反节点的地图 (#表示反节点):" << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Point current(i, j);
            if (allAntinodes2.count(current) && grid[i][j] == '.') {
                cout << '#';
            } else {
                cout << grid[i][j];
            }
        }
        cout << endl;
    }
    
    return 0;
}