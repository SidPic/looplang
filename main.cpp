#include <iostream>

#include "parser.hpp"

#include <stack>
#include <any>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <map>

typedef std::stack<std::any> stack_t;
typedef std::function<void(stack_t&)> func_t;

template <typename T>
void say(stack_t& stack) {
    while (!stack.empty()) {
        std::cout << std::any_cast<T>(stack.top());
        stack.pop();
    }
    std::cout << std::endl;
}

template <typename T>
void prod(stack_t& stack) {
    auto a = std::any_cast<T>(stack.top());
    stack.pop();
    auto b = std::any_cast<T>(stack.top());
    stack.pop();
    stack.push(a*b);
}

#define FUNC(type, name) {typeid(type), name<type>}
#define FUNC_BASIC_TYPES(name) FUNC(int,name), FUNC(double,name), FUNC(char,name)

std::unordered_map<std::string, std::unordered_map<std::type_index, func_t>> basic {
    {"say", { FUNC_BASIC_TYPES(say), FUNC(std::string,say) }},
    {"*", { FUNC(int,prod), FUNC(double,prod) }}
};

int main() {

    std::vector<std::string> script;
    ll::parse(script, "script.ll");

    /*-----------------------------------------------------------*/

    std::unordered_map<std::string, std::any> vars; // адрес не должен меняться

    struct Function {
        Function(){}
        std::string name;
        stack_t* stack;
        std::vector<func_t> cmd;
        std::unordered_map<std::string, std::any> vars;

        void run(std::vector<std::pair<std::string, std::any>> _args) {
            stack = new stack_t;

            for (auto& v : _args) {
                if (auto var = vars.find(v.first); var != vars.end()) {
                    var->second = v.second;
                } else {
                    std::cout << "\e[1m\e[31mError!\e[0m The function '" << name << "' has no the parameter \'" << v.first << "\'\e[0m";
                    exit(EXIT_FAILURE);
                }
            }

            for (auto& c : cmd) {
                c(*stack);
            }

            delete stack;
        }
    };

    std::unordered_map<std::string, Function> funcs;

    for (auto cmd = script.begin(); cmd != script.end(); ++cmd) {
        while (cmd != script.end() && *cmd != ";") ++cmd; auto end = cmd; --cmd;

        if (*cmd == "func")
        {
            auto& func = funcs.emplace(*--cmd, Function{}).first->second;
                std::cout << "func: " << *cmd << std::endl;
            func.name = *cmd;

            if (cmd != script.begin()) {
                --cmd;
                while (cmd != script.begin() && *cmd != ";") {
                    std::cout << "arg: " << *cmd << std::endl;
                    func.vars.emplace(*cmd, std::any{});
                    --cmd;
                }
            }

            cmd = end;
            std::type_index last_type = typeid(int);

            while (*++cmd != "end") {
                std::cout << "cmd: " << *cmd << std::endl;
                if (cmd->starts_with('\"')) {
                    last_type = typeid(std::string);
                    func.cmd.push_back([value=*cmd](stack_t& stack){ stack.push(value); });
                }
                else if (*cmd == ";") {
                    func.cmd.push_back([](stack_t& stack){ while (!stack.empty()) stack.pop(); });
                }
                else if (std::isdigit((*cmd)[0])) {
                    if (cmd->contains(".")) {
                        last_type = typeid(double);
                        func.cmd.push_back([value=std::stod(*cmd)](stack_t& stack){ stack.push(value); });
                    } else {
                        last_type = typeid(int);
                        func.cmd.push_back([value=std::stoi(*cmd)](stack_t& stack){ stack.push(value); });
                    }
                }
                else {
                    if (cmd[1] == "var") {
                        std::any* var = &func.vars.emplace(*cmd, std::any{}).first->second;
                        if (*(cmd-1) != ";") func.cmd.push_back([var](stack_t& stack){
                            *var = stack.top();
                            stack.pop();
                        });
                        ++cmd;
                    }
                    else if (auto b = basic.find(*cmd); b != basic.end()) {
                        try {
                            func.cmd.push_back(b->second.at(std::type_index(last_type)));
                        }
                        catch (std::out_of_range) {
                            std::cout << "\e[1m\e[31mError!\e[0m\' " << *cmd << "\' was not declared " << "for \'" << last_type.name() << "\' type\e[0m" << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (auto v = func.vars.find(*cmd); v != func.vars.end()) {
                        std::cout << v->first << ", " << v->second.type().name() << std::endl;
                        last_type = std::type_index(v->second.type());
                        func.cmd.push_back([value=&v->second](stack_t& stack){ stack.push(*value); });
                    }
                    else if (auto g = vars.find(*cmd); g != vars.end()) {
                        last_type = std::type_index(g->second.type());
                        func.cmd.push_back([value=&g->second](stack_t& stack){ stack.push(*value); });
                    }
                    else {
                        std::cout << "\e[1m\e[31mError!\e[0m\' " << *cmd << "\' was not declared\e[0m" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            } ++cmd;
        }
    }

    funcs["hello"].run({});
    funcs["sqr"].run({{"x", 10.0}});

    return 0;
}
