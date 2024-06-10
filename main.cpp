#include <iostream>

#include "parser.hpp"

int main() {

    std::vector<std::string> script;
    ll::parse(script, "script.ll");

    for (auto& cmd : script) {
        std::cout << cmd << std::endl;
    }

    return 0;
}
