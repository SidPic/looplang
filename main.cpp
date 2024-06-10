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
    "+", "-", "*", "/",     "+r", "-r", "*r", "/r",
    "#", "%", "^", ".",     "#r", "%r", "^r", ".r",
    "<", ">", "<=", ">=",   "<r", ">r", "<=r", ">=r"
};

const std::map<std::string, unsigned char> priority {
    { "+", 3 }, { "+r", 3 },
    { "-", 3 }, { "-r", 3 },
    { "*", 2 }, { "*r", 2 },
    { "/", 2 }, { "/r", 2 },
    { "%", 1 }, { "%r", 1 },
    { "^", 1 }, { "^r", 1 },
    { "#", 0 }, { "#r", 0 },
    { ".", 0 }, { ".r", 0 },
    { "<", 4 }, { "<r", 4 },
    { ">", 4 }, { ">r", 4 },
    { "<=", 4 }, { "<=r", 4 },
    { ">=", 4 }, { ">=r", 4 }
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
        while (isalnum(ch)) {
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
            bool has_brackets = false;

            while (comma < tok.size() && tok[comma] != ",") {
                if (tok[comma] == "(") has_brackets = true;
                couples.back().push_back(tok[comma++]);
            }

            #undef tok
            size_t begin = couples.size()-1;
            #define tok couples[begin]

            int token = 0;
            std::function<void()> parse_brackets;
            parse_brackets = [&](){
                bool has_infix = false;

                if (has_brackets) couples.emplace_back();

                size_t couple = couples.size()-1;

                for (; token < tok.size(); ++token) {
                    if (has_brackets && tok[token] == "(") {
                        ++token;
                        parse_brackets();
                    }
                    else if (has_brackets && tok[token] == ")") {
                        break;
                    }
                    else {
                        //~ std::cout << couple << " | " << tok[token] << std::endl; /// DEBUG
                        if (isinfix(tok[token])) {
                            has_infix = true;
                            if (has_brackets) {
                                if (token+2 < tok.size() && isinfix(tok[token+2])) {
                                    if (priority.at(tok[token+2]) < priority.at(tok[token])) {
                                        couples[couple].push_back(tok[token] + "r");
                                        continue;
                                    }
                                }
                                if (token+1 < tok.size() && tok[token+1] == "(") {
                                    couples[couple].push_back(tok[token] + "r");
                                    continue;
                                }
                                couples[couple].push_back(tok[token]);
                            }
                        } else if (has_brackets) couples[couple].push_back(tok[token]);
                    }
                }

                if (has_infix) {
                    #undef tok
                    #define tok couples[couple]
                    std::vector<std::string> ptoks;
                    std::vector<bool> is_moved ( tok.size(), false );

                    for (int p = 0; p <= 7; ++p) {
                        for (int n = 0; n < tok.size(); ++n) {
                            if (isinfix(tok[n]) && priority.at(tok[n]) == p)
                            {
                                if (n > 0 && !isinfix(tok[n-1]) && !is_moved[n-1]) {
                                    ptoks.push_back(tok[n-1]);
                                    is_moved[n-1] = true;
                                }
                                if (n+1 < tok.size() && !isinfix(tok[n+1]) && !is_moved[n+1]) {
                                    ptoks.push_back(tok[n+1]);
                                    is_moved[n+1] = true;
                                }
                                ptoks.push_back(tok[n]);
                                is_moved[n] = true;
                            }
                        }
                    }
                    if (!ptoks.empty()) tok = ptoks;
                    #undef tok
                }
            };
            parse_brackets();
            if (has_brackets) couples.erase(couples.begin() + begin);
        }
        couples.erase(couples.begin() + begin);

        couples.emplace_back();
        couples.back().push_back(END_TOK);
    }

    std::vector<std::string> script;
    auto cur = script.begin();

    ///DEBUG
    //~ for (auto& c : couples) {
        //~ for (auto& s : c) {
            //~ std::cout << s << " ";
        //~ }
        //~ std::cout << std::endl;
    //~ }

    for (int i = 0; i < couples.size(); ++i) {
        size_t begin = i;
        while (couples[i][0] != END_TOK) ++i;
        for (std::vector<std::vector<std::string>>::reverse_iterator c = couples.rend()-i; c != couples.rend()-begin; ++c) {
            #define c (*c)

            if (c.empty()) continue;

            bool has_infix = false;
            for (auto& tok : c) if (isinfix(tok)) {
                has_infix = true;
                break;
            }

            if (has_infix) for (auto tok = c.begin(); tok != c.end(); ++tok) {
                if (tok->empty()) continue;
                //~ std::cout << *tok << " "; ///DEBUG
                script.push_back(*tok);
            }
            else for (auto tok = c.rbegin(); tok != c.rend(); ++tok) {
                if (tok->empty()) continue;
                //~ std::cout << *tok << " "; ///DEBUG
                script.push_back(*tok);
            }
            //~ std::cout << std::endl; ///DEBUG
            #undef c
        }
        script.push_back(END_TOK);
    }

    ///DEBUG
    for (auto& cmd : script) {
        std::cout << cmd << std::endl;
    }

    return 0;
}
