#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

const int WIDTH = 101; // 画布宽度
const int HEIGHT = 103; // 画布高度
const int SECOND = 100; // 模拟时间


// 机器人结构体
struct Robot {
    int x, y;    // 位置
    int vx, vy;  // 速度
};

// 读取输入文件并返回机器人向量
std::vector<Robot> readInput(const std::string& filename) {
    std::vector<Robot> robots;
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // 解析格式: p=x,y v=dx,dy
        Robot robot;
        size_t pPos = line.find("p=");
        size_t vPos = line.find("v=");
        size_t commaPos1 = line.find(",", pPos);
        size_t spacePos = line.find(" ", pPos);
        size_t commaPos2 = line.find(",", vPos);
        
        // 提取位置坐标
        robot.x = std::stoi(line.substr(pPos + 2, commaPos1 - pPos - 2));
        robot.y = std::stoi(line.substr(commaPos1 + 1, spacePos - commaPos1 - 1));
        
        // 提取速度
        robot.vx = std::stoi(line.substr(vPos + 2, commaPos2 - vPos - 2));
        robot.vy = std::stoi(line.substr(commaPos2 + 1));
        
        robots.push_back(robot);
    }
    
    return robots;
}

/* Part 1 */
/**
 * 画布中机器人按照各自的速度移动
 * 将画布划分为 4 个象限，计算每个象限内的机器人数量, 忽略位于中间区域的机器人(即 x=WIDTH/2 或 y=HEIGHT/2 的机器人)
 * 返回 安全系数 = 象限内机器人数量的乘积
 */
int Part1(const std::vector<Robot>& robots) {
    int* quadrantCount = new int[4]();
    for (const Robot& robot : robots) {
        // 计算SECOND秒后的位置
        int newX = (robot.x + robot.vx * SECOND) % WIDTH;
        int newY = (robot.y + robot.vy * SECOND) % HEIGHT;
        // 处理负坐标
        if (newX < 0) newX += WIDTH;
        if (newY < 0) newY += HEIGHT;
        // 根据位置确定象限 使对应象限计数器加1 忽略中间区域的机器人
        if (newX < WIDTH / 2 && newY < HEIGHT / 2) {
            quadrantCount[0]++; // 第一象限
        } else if (newX > WIDTH / 2 && newY < HEIGHT / 2) {
            quadrantCount[1]++; // 第二象限
        } else if (newX < WIDTH / 2 && newY > HEIGHT / 2) {
            quadrantCount[2]++; // 第三象限
        } else if (newX > WIDTH / 2 && newY > HEIGHT / 2) {
            quadrantCount[3]++; // 第四象限
        }
    }
    // 输出各象限内机器人数量
    std::cout << "---Part1---" << std::endl;
    std::cout << "象限内机器人数量: " << std::endl;
    std::cout << "第一象限: " << quadrantCount[0] << std::endl;
    std::cout << "第二象限: " << quadrantCount[1] << std::endl;
    std::cout << "第三象限: " << quadrantCount[2] << std::endl;
    std::cout << "第四象限: " << quadrantCount[3] << std::endl;
    // 计算安全系数
    int safetyFactor = quadrantCount[0] * quadrantCount[1] * quadrantCount[2] * quadrantCount[3];
    std::cout << "安全系数: " << safetyFactor << std::endl;
    return safetyFactor;
}

/* Part 2 */
/** 
 * 画布中机器人按照各自的速度移动
 * 大部分机器人会在某个时刻形成圣诞树形状
 * 计算形成圣诞树形状的时刻，并返回该时刻
 * 题目没有给出具体的圣诞树形状定义，因此这里假设圣诞树必然会形成3*3的机器人集群作为判断条件
 */
int Part2(const std::vector<Robot>& robots) {
    for (int t = 0; t < 10000; ++t) {
        std::vector<std::vector<bool>> canvas(HEIGHT, std::vector<bool>(WIDTH, false));
        for (const Robot& robot : robots) {
            int newX = (robot.x + robot.vx * t) % WIDTH;
            int newY = (robot.y + robot.vy * t) % HEIGHT;
            if (newX < 0) newX += WIDTH;
            if (newY < 0) newY += HEIGHT;
            if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
                canvas[newY][newX] = true;
            }
        }
        
        // 检查是否形成3*3的集群
        for (int y = 1; y < HEIGHT - 1; ++y) {
            for (int x = 1; x < WIDTH - 1; ++x) {
                if (canvas[y][x] && canvas[y-1][x] && canvas[y+1][x] &&
                    canvas[y][x-1] && canvas[y][x+1] &&
                    canvas[y-1][x-1] && canvas[y-1][x+1] &&
                    canvas[y+1][x-1] && canvas[y+1][x+1]) {
                    // 输出画布
                    std::cout << "---Part2---" << std::endl;
                    std::cout << "形成圣诞树形状的时刻: " << t << std::endl;
                    for (int i = 0; i < HEIGHT; ++i) {
                        for (int j = 0; j < WIDTH; ++j) {
                            std::cout << (canvas[i][j] ? '#' : '.');
                        }
                        std::cout << std::endl;
                    }
                    return t; // 返回形成圣诞树形状的时刻
                }
            }
        }
    }
    return -1; // 如果没有形成圣诞树形状，返回-1
}

int main() {
    std::string filename = "input.txt";
    std::vector<Robot> robots = readInput(filename);    

    int result1 = Part1(robots);
    int result2 = Part2(robots);

    std::cout << "Part 1: " << result1 << std::endl;
    std::cout << "Part 2: " << result2 << std::endl;

    return 0;
}