// std=c++23

#include "parser.hpp"
#include "basic_functions.hpp"

#include <iostream>
#include <unordered_map>

// во время компилляции параллельно добавлению в скрипт push-функций,
// добавлять в стек pop-функции, чтобы потом их вставить в скрипт //
// стек (мапа) описаний массивов                                  // compile
// стек (мапа) описаний структур                                  //    time
// описания переменных                                            //

// разбить по функциям с аргументами

struct VariableInfo {
    VariableInfo(size_t __addr, Type __type): addr(__addr), type(__type) { }
    size_t addr;
    Type type;
};
//                  имя, номер в стеке
std::unordered_map<std::string, VariableInfo> variables;
// виртуальный стек времени компиляции для разметки переменных
Stack vstack;

inline void bf_var(Stack& stack) {
    std::cout << "OK" << std::endl;
    std::cout << "type: " << int(stack.top<Type>()) << std::endl;
    bf_assign[stack.pop<Type>()](stack);
}

struct Expression {
    std::vector<TypedValue> data;
    std::string string_literals;

    Expression& compile(std::vector<std::string>::iterator& s) {

        Type expr_type = INT_TYPE;

        for (std::string word = *s; word != ";"; word = *++s) {

            // NUMBER
            if (std::isdigit(word[0])) {
                if (word.contains(".")) { // double
                    data.emplace_back(std::stod(word), LITERAL_EXT);
                } else {  // integer
                    data.emplace_back(std::stoi(word), LITERAL_EXT);
                }
            }

            // VARIABLE
            else if (auto var = variables.find(word); var != variables.end()) {
                data.emplace_back(var->second.addr, VAR_EXT, var->second.type);
            }

            // KEYWORD VAR
            else if (word == "var") {
                size_t new_variable = vstack.var_end;

                // if unable to register a variable with this name
                if (!variables.try_emplace(s[-1],
                    vstack.push_var(expr_type), expr_type
                ).second) {
                    std::cerr << "\e[31;1m[Error]\e[0m " << s[-1] << " is already declared" << std::endl;
                    throw("variable redeclaring");
                }

                data.emplace_back(new_variable, LITERAL_EXT);

                data.emplace_back(expr_type, LITERAL_EXT);

                data.emplace_back(bf_var, FUNC_EXT);
            }

            //~ else if (word == "print") {

            //~ }
            //else if ( is func ) expr_type = func returning type

            // OTHER WORDS
            else {
                if (word.starts_with('\"')) {
                    word.erase(0,1);
                    word.pop_back();
                }
                else if (s[1] == "var") {
                    continue;
                } else {
                    std::cerr << "\e[31;1m[Error]\e[0m " << word << " is not declared" << std::endl;
                    throw("undeclared name");
                }

                char* new_string = string_literals.data() + string_literals.size();
                string_literals.reserve(string_literals.size() + word.size());
                for (auto ch : word) {
                    string_literals.push_back(ch);
                }
                string_literals.push_back('\0');

                data.emplace_back(new_string, LITERAL_EXT);
            }

            expr_type = data.back().type;
        }

        return *this;
    }

    void run(Stack& stack) {

        for (int i = 0; i < data.size(); i++) {
            if (data[i].extension == VAR_EXT) {
                stack.push_value(data[i]);
            }
            else if (data[i].extension == FUNC_EXT) { // если print, то после каждой ф-ии вставить тип в стек
                std::cout << "function" << std::endl;
                data[i].VAL_FUNC_T(stack); // see stack.top_expr_type
            }
            else if (data[i].extension == LITERAL_EXT) { // if next_func(data, i) is print stack.push(type)
                std::cout << "literal: " << data[i].VAL_STRING_T << " type: " << data[i].type << std::endl;
                stack.push_literal(data[i]);
            }
        }

        stack.end = stack.var_end;
    }
};

#include <variant>

int main() {

    std::vector<std::string> script;
    lpl::parse(script, "test.ll");

    Expression expr;

    for (auto s = script.begin(); s != script.end(); s++) {
        expr.compile(s).run(vstack);
    }

    return 0;
}
