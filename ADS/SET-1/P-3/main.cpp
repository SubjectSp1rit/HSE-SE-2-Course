#include <iostream>
#include <string>
#include <vector>

std::string RecursiveInoskeUnpacking(std::string input) {
    std::string answer = "";
    std::string innerStr;
    int num = 0;
    for (int i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (std::isdigit(c)) {
            num = c - '0';
            i += 2;
            int balance = 1;
            while (balance > 0) {
                if (input[i] == '[') ++balance;
                else if (input[i] == ']') {
                    --balance;
                    if (balance == 0) {
                        break;
                    }
                }

                innerStr.push_back(input[i]);
                ++i;
            }

            std::string part = RecursiveInoskeUnpacking(innerStr);
            for (int j = 0; j < num; ++j) {
                answer += part;
            }
            innerStr.clear();
        } else {
            answer += input[i];
        }
    }

    return answer;
}

int main() {
    std::string line;
    std::getline(std::cin, line);

    std::cout << RecursiveInoskeUnpacking(line);

    return 0;
}