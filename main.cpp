// std=c++23

#include "parser.hpp"
#include "types.hpp"

#include <iostream>
#include <unordered_map>

union Value {
    Value (int _i): i(_i) {}
    Value (double _f): f(_f) {}
    Value (char _c): c(_c) {}
    Value (char* _s): s(_s) {}
    Value (void* _ptr): ptr(_ptr) {}
    Value (func_t _func): func(_func) {}
    Value (size_t _size): size(_size) {}
    Value (Type _type): type(_type) {}
    
    int i;
    double f;
    char c;
    char* s;
    void* ptr;      // for pointers and globals
    func_t func;
    size_t size;    // for const size arrays and structures
    size_t addr;
    Type type;      // value contains a type
};

struct Global {
    Type type;
    Value value;
};

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

void bf_var(Stack& stack) {
    Type type = stack.pop<Type>();
    bf_assign[type](stack);
}

struct Expression {
    std::vector<bool> is_func;
    std::vector<bool> is_var;
    std::vector<Value> data;
    std::string string_literals;
    
    Expression& compile(std::vector<std::string>::iterator& s) {
        
        Type expr_type = INT_TYPE;
       
        for (std::string word = *s; word != ";"; word = *++s) {
            
            // NUMBER
            if (std::isdigit(word[0])) {
                if (word.contains(".")) { // double
                    expr_type = FLOAT_TYPE;
                    data.emplace_back(std::stod(word));
                } else {  // integer
                    expr_type = INT_TYPE;
                    data.emplace_back(std::stoi(word));
                }
                is_var.push_back(false);
                is_func.push_back(false);
            }
            
            // VARIABLE
            else if (auto var = variables.find(word); var != variables.end()) {
                expr_type = var->second.type;

                data.emplace_back(var->second.addr);
                is_var.push_back(true);
                is_func.push_back(false);

                data.emplace_back(var->second.type);
                is_var.push_back(false);
                is_func.push_back(false);
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

                data.emplace_back(new_variable);
                is_var.push_back(false);
                is_func.push_back(false);

                data.emplace_back(expr_type);
                is_var.push_back(false);
                is_func.push_back(false);

                data.emplace_back(bf_var);
                is_var.push_back(false);
                is_func.push_back(true);
            }
            
            // OTHER WORDS
            else {
                if (word.starts_with('\"')) {
                    word.erase(0,1);
                    word.pop_back();
                    expr_type = STRING_TYPE;
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
            
                data.emplace_back(new_string);
                is_var.push_back(false);
                is_func.push_back(false);
            }
        }
        
        return *this;
    }
    
    void run(Stack& stack) {
        
        for (int i = 0; i < data.size(); i++) {
            if (is_var[i])
                stack.push_value(data[i].addr, data[++i].type);
            
        }
        
        stack.end = stack.var_end;
    }
};

enum { KEYWORD, STRING, INTEGER, FLOAT, VARIABLE };

// добавить constexpr для вычисления во время компиляции

int main() {

    std::vector<std::string> script;
    ll::parse(script, "test.ll");

    Expression expr;
    size_t stack_end = 0;

    for (auto s = script.begin(); s != script.end(); s++) {
        expr.compile(s).run(vstack);
    }

    return 0;
}
