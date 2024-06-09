#include <iostream>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <string>
#include <set>
#include <functional>
#include <map>

const std::string END_TOK {";"};
const std::set<std::string> infix {
    "+", "-", "*", "/",
    "#", "%", "^", ".",
    "<", ">", "<=", ">=",
};

const std::map<std::string, unsigned char> priority {
    { "+", 3 },
    { "-", 3 },
    { "*", 2 },
    { "/", 2 },
    { "%", 1 },
    { "^", 1 },
    { "<", 4 },
    { "<=", 4 },
    { ">", 4 },
    { ">=", 4 }
};

bool isinfix(const std::string& token) {
    return infix.contains(token);
}

int read_token(std::string& token, FILE* file) {
    token.clear();
    int ch;
again:
    while (isspace(ch = fgetc(file)));

    if (ch == EOF) {
        return 0;
    }

    if (ch == ';') {
        while ((ch = fgetc(file)) != '\n' && ch != EOF);
        goto again;
    }
    else if (ch == '\"') {
        token.push_back('\"');
        while ((ch = fgetc(file)) != '\"' && ch != EOF) token.push_back(ch);
        token.push_back('\"');
    }
    else if (ispunct(ch)) {
        while (ispunct(ch)) {
            token.push_back(ch);
            ch = fgetc(file);
            if (ch == '(' || ch == ')' || ch == ',' || ch == ';') break;
        }
    }
    else if (isalnum(ch)) {
        while (isalnum(ch) || ch == '#' || ch == '.') {
            token.push_back(ch);
            ch = fgetc(file);
        }
    }

    if (ch != '\n' && ch != '\"') {
        ungetc(ch, file);
    }

    return ch;
}

int main() {
    FILE* file = fopen("script.ll", "r");
    std::string token; int c;
    std::vector<std::string> raw_tokens;

    while (c = read_token(token, file)) {
        if (!token.empty()) raw_tokens.push_back(token);
        if (c == '\n') raw_tokens.push_back(END_TOK);
    }

    std::vector<std::vector<std::string>> couples;

    #define tok raw_tokens

    for (int line = 0; line < tok.size(); ++line) {
        couples.emplace_back();
        couples.back().push_back(tok[line++]);

        couples.emplace_back();
        while (line < tok.size() && tok[line] != ";") couples.back().push_back(tok[line++]);

        #undef tok
        size_t begin = couples.size()-1;
        #define tok couples[begin]

        for (int comma = 0; comma < tok.size(); ++comma) {
            couples.emplace_back();

            while (comma < tok.size() && tok[comma] != ",") couples.back().push_back(tok[comma++]);

            #undef tok
            size_t begin = couples.size()-1;
            #define tok couples[begin]

            int token = 0;
            std::function<void()> parse_brackets;
            parse_brackets = [&](){
                couples.emplace_back();
                size_t couple = couples.size()-1;

                for (; token < tok.size(); ++token) {
                    if (tok[token] == "(") {
                        ++token;
                        parse_brackets();
                    }
                    else if (tok[token] == ")") {
                        break;
                    }
                    else {
                        couples[couple].push_back(tok[token]);
                    }
                } // сделать аналогично для более приоритетных операторов

                #undef tok
                #define tok couples[couple]
                std::vector<std::string> ptoks;
                std::vector<bool> is_moved ( tok.size(), false );

                for (int p = 0; p <= 7; ++p) {
                    for (int n = 0; n < tok.size(); ++n) {
                        if (isinfix(tok[n]) && priority.at(tok[n]) == p)
                        {
                            if (p > 0 && !isinfix(tok[n-1]) && !is_moved[n-1]) {
                                ptoks.push_back(tok[n-1]);
                                is_moved[n-1] = true;
                            }
                            if (p < 7 && !isinfix(tok[n+1]) && !is_moved[n+1]) {
                                ptoks.push_back(tok[n+1]);
                                is_moved[n+1] = true;
                            }
                            ptoks.push_back(tok[n]);
                        }
                    }
                }

                if (!ptoks.empty()) tok = ptoks;
            };
            parse_brackets();
            couples.erase(couples.begin()+begin);
        }
        couples.erase(couples.begin() + begin);

        couples.emplace_back();
        couples.back().push_back(END_TOK);
    }

    for (auto& c : couples) {
        for (auto& s : c) {
            std::cout << s << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
