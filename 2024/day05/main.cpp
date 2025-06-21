#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <queue>

using namespace std;

bool isValidUpdate(const vector<int>& queue, const vector<pair<int, int>>& rules) {
    for (const auto& rule : rules) {
        int before = rule.first;
        int after = rule.second;
        auto itBefore = find(queue.begin(), queue.end(), before);
        auto itAfter = find(queue.begin(), queue.end(), after);

        if (itBefore == queue.end() || itAfter == queue.end()) {
            continue;
        }
        if (itBefore > itAfter) {
            return false; 
        }
    }
    return true;
}

void buildGraphFromRules(const vector<int>& queue, 
                        const vector<pair<int, int>>& allRules,
                        map<int, vector<int>>& graph,
                        map<int, int>& inDegree) {
    
    // 第一步：初始化所有队列中的节点
    for (int page : queue) {
        graph[page] = vector<int>();  // 初始化邻接表
        inDegree[page] = 0;          // 初始化入度为0
    }
    
    // 第二步：筛选适用于当前队列的规则
    for (const auto& rule : allRules) {
        int before = rule.first;
        int after = rule.second;
        
        // 只考虑队列中都存在的页面之间的规则
        if (find(queue.begin(), queue.end(), before) != queue.end() &&
            find(queue.begin(), queue.end(), after) != queue.end()) {
            
            // 添加边：before -> after
            graph[before].push_back(after);
            
            // after的入度加1
            inDegree[after]++;
        }
    }
}

void sortQueueByRules(vector<int>& queue, const vector<pair<int, int>>& rules) {
    // 邻接表表示图
    map<int, vector<int>> graph;  // 节点 -> 它指向的节点列表
    map<int, int> inDegree;       // 节点 -> 入度
    buildGraphFromRules(queue, rules, graph, inDegree);

    std::queue<int> zeroInDegreeQueue;

    // 使用拓扑排序来排序队列
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {  // 入度为0
            zeroInDegreeQueue.push(pair.first);
        }
    }

    vector<int> sortedQueue;
    while (!zeroInDegreeQueue.empty()) {
        int current = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        sortedQueue.push_back(current);

        // 遍历当前节点的所有邻接节点
        for (int neighbor : graph[current]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) {
                zeroInDegreeQueue.push(neighbor);
            }
        }
    }

    if (sortedQueue.size() == queue.size()) {
        queue = sortedQueue;  // 如果排序成功，更新原队列
    } else {
        cerr << "错误：无法对队列进行拓扑排序，可能存在循环依赖。" << endl;
    }    
}

int main() {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cerr << "无法打开文件 input.txt" << endl;
        return 1;
    }
    
    vector<pair<int, int>> rules;
    
    vector<vector<int>> updates;
    
    string line;
    bool readingRules = true;
    
    while (getline(file, line)) {
        if (line.empty()) {
            readingRules = false;
            continue;
        }
        
        if (readingRules) {
            stringstream ss(line);
            string rulePart;
            while (getline(ss, rulePart)) {
                size_t pos = rulePart.find('|');
                if (pos != string::npos) {
                    try {
                        int before = stoi(rulePart.substr(0, pos));
                        int after = stoi(rulePart.substr(pos + 1));
                        rules.emplace_back(before, after);
                    } catch (const invalid_argument& e) {
                        cerr << "规则格式错误: " << rulePart << endl;
                    }
                } else {
                    cerr << "规则格式错误: " << rulePart << endl;
                }
            }
            
        } else {
            stringstream ss(line);
            string pagePart;
            vector<int> queue;
            while (getline(ss, pagePart, ',')) {
                int pageNumber;
                try {
                    pageNumber = stoi(pagePart);
                    queue.push_back(pageNumber);
                } catch (const invalid_argument& e) {
                    cerr << "页面号格式错误: " << pagePart << endl;
                }
            }
            if (!queue.empty()) {
                updates.push_back(queue);
            } else {
                cerr << "更新队列为空或格式错误: " << line << endl;
            }
        }
    }
    file.close();
    
    cout << "规则数量: " << rules.size() << endl;
    cout << "更新队列数量: " << updates.size() << endl;

    /* Part 1 */
    vector<vector<int>> validUpdates;
    vector<vector<int>> invalidUpdates;

    for (const auto& queue : updates) {
        if (isValidUpdate(queue, rules)) {
            validUpdates.push_back(queue);
        } else {
            invalidUpdates.push_back(queue);
        }
    }
    cout << "符合规则的更新队列数量: " << validUpdates.size() << endl;
    cout << "不符合规则的更新队列数量: " << invalidUpdates.size() << endl;
    
    int result = 0;
    for (const auto& queue : validUpdates) {
        if (queue.size() % 2 == 1) {
            int midIndex = queue.size() / 2;
            result += queue[midIndex];
        } else {
            int midIndex1 = queue.size() / 2 - 1;
            int midIndex2 = queue.size() / 2;
            result += (queue[midIndex1] + queue[midIndex2]) / 2;
        }
    }
    
    cout << "Part 1 结果: " << result << endl;

    /* Part 2 */
    for (auto& queue : invalidUpdates) {
        sortQueueByRules(queue, rules);
    }    

    int invalidResult = 0;
    for (const auto& queue : invalidUpdates) {
        if (queue.size() % 2 == 1) {
            int midIndex = queue.size() / 2;
            invalidResult += queue[midIndex];

        } else {
            int midIndex1 = queue.size() / 2 - 1;
            int midIndex2 = queue.size() / 2;
            invalidResult += (queue[midIndex1] + queue[midIndex2]) / 2;
        }
    }


    cout << "Part 2 结果: " << invalidResult << endl;
    
    return 0;
}