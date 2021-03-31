#include <iostream>
#include <string>
#include <regex>

using namespace std;
//std::regex r("^(\\d+)pal(\\w+)");
std::regex r("(Pal)");

int main() {
    int no = 1;
    std::smatch m;
    for (std::string line; std::getline(std::cin, line);) {
        std::regex_search(line, m, r);
        if (m.size() > 0) {
            //for(auto v: m) std::cout << v << std::endl;
            cout << m.str(1) << endl;
        }
    }
    return 0;
}

