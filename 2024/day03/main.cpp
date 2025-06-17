#include <iostream>
#include <fstream>
#include <string>
#include <regex>

int main() {
    // Read the input file
    std::ifstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input.txt" << std::endl;
        return 1;
    }
    
    std::string content;
    std::string line;
    
    // Read entire file content
    while (std::getline(file, line)) {
        content += line;
    }
    file.close();
      // Regex pattern to match mul(num1,num2), do(), and don't()
    std::regex pattern(R"(mul\((\d+),(\d+)\)|do\(\)|don't\(\))");
    std::sregex_iterator iter(content.begin(), content.end(), pattern);
    std::sregex_iterator end;
    
    long long total = 0;
    bool enabled = true; 
    
    while (iter != end) {
        std::smatch match = *iter;
        std::string matched_text = match[0].str();
        
        if (matched_text == "do()") {
            enabled = true;
        }
        else if (matched_text == "don't()") {
            enabled = false;  
        }
        else if (matched_text.substr(0, 3) == "mul" && enabled) {
            int num1 = std::stoi(match[1].str());
            int num2 = std::stoi(match[2].str());
            total += num1 * num2;
        }
        ++iter;
    }
    
    std::cout << "Total sum: " << total << std::endl;
    
    return 0;
}