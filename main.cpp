#include <iostream>

#include "parser.hpp"

#include <stack>
#include <any>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <map>
#include <initializer_list>

typedef std::stack<std::any> stack_t;
typedef std::function<void(stack_t&)> func_t;

extern std::unordered_map<std::string, std::unordered_map<std::type_index, func_t>> basic;

template <typename T>
void say(stack_t& stack) {
    while (!stack.empty()) {
        auto& val = stack.top();
        if (std::type_index(val.type()) != typeid(T)) {
            basic["say"].at(std::type_index(val.type()))(stack);
            return;
        }
        std::cout << std::any_cast<T>(val);
        stack.pop();
    }
    std::cout << std::endl;
}

template <typename T>
void prod(stack_t& stack) {
    auto b = std::any_cast<T>(stack.top());
    stack.pop();
    auto a = std::any_cast<T>(stack.top());
    stack.pop();
    stack.push(a*b);
}

template <typename T>
void dif(stack_t& stack) {
    auto b = std::any_cast<T>(stack.top());
    stack.pop();
    auto a = std::any_cast<T>(stack.top());
    stack.pop();
    stack.push(a-b);
}

#define FUNC_VOID(_name) {typeid(void), [](stack_t& stack){ basic[_name].at(std::type_index(stack.top().type()))(stack); }}
#define FUNC(type, name) {typeid(type), name<type>}
#define FUNC_BASIC_TYPES(name) FUNC(int,name), FUNC(double,name), FUNC(char,name)

std::unordered_map<std::string, std::unordered_map<std::type_index, func_t>> basic {
    {"say", { FUNC_VOID("say"), FUNC_BASIC_TYPES(say), FUNC(std::string,say) }},
    {"*", { FUNC_VOID("*"), FUNC(int,prod), FUNC(double,prod)  }},
    {"-", { FUNC_VOID("-"), FUNC(int,dif), FUNC(double,dif) }}
};

std::unordered_map<std::type_index, std::any> typed_any {
    {typeid(int), int{}},
    {typeid(double), double{}},
    {typeid(char), char{}},
    {typeid(std::string), std::string()}
};

typedef void (*f_t) (stack_t&);

void func_ret (stack_t&) { }

func_t func_return = func_ret;

struct Function {
    Function(){}
    std::string name;
    stack_t* stack;
    std::vector<func_t> cmd;
    std::unordered_map<std::string, std::any> vars;
    std::vector<std::any*> args;

    func_t run_on_stack() {
        return [self=this](stack_t& stack) {
            for (auto& arg : self->args) {
                if (stack.empty()) {
                    std::cout << "\e[31m\e[1mError!\e[0m Too few args for the function \'" << self->name << "\'" << " (expected " << self->args.size() << ")" << std::endl;
                    exit(EXIT_FAILURE);
                }
                *arg = stack.top();
                stack.pop();
            }
            for (auto& c : self->cmd) {
                if (c.target<f_t>() != nullptr && *c.target<f_t>() == func_ret) break;
                c(stack);
            }
        };
    }

    template <typename ...Args>
    std::any operator() (const Args&... a) {
        stack = new stack_t;

        int n = 0;
        for (auto& arg : (std::initializer_list<std::any>{a...})) {
            if (n == args.size()) {
                std::cout << "\e[31m\e[1mError!\e[0m Too many args for the function \'" << name << "\'" << " (expected " << args.size() << ")" << std::endl;
                exit(EXIT_FAILURE);
            }
            *(args[n]) = arg;
            ++n;
        }

        for (auto& c : cmd) {
            if (c.target<f_t>() != nullptr && *c.target<f_t>() == func_ret) break;
            c(*stack);
        }

        std::any result;
        if (!stack->empty()) result = stack->top();
        delete stack;
        return result;
    }

    std::any run(std::vector<std::pair<std::string, std::any>> _args) {
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
            if (c.target<f_t>() != nullptr && *c.target<f_t>() == func_ret) break;
            c(*stack);
        }

        std::any result;
        if (!stack->empty()) result = stack->top();
        delete stack;
        return result;
    }
};

int main() {

    std::vector<std::string> script;
    ll::parse(script, "script.ll");

    /*-----------------------------------------------------------*/

    std::unordered_map<std::string, std::any> vars; // адрес не должен меняться

    std::unordered_map<std::string, Function> funcs;

    for (auto& c : script) {
        std::cout << c << std::endl;
    }

    for (auto cmd = script.begin(); cmd != script.end(); ++cmd) {
        while (cmd != script.end() && *cmd != ";") ++cmd; auto end = cmd; --cmd;

        if (*cmd == "func")
        {
            auto& func = funcs.emplace(*--cmd, Function{}).first->second;
            func.name = *cmd;

            if (cmd != script.begin() && *--cmd != ";") {
                do {
                    func.args.push_back(&func.vars.emplace(*cmd, std::any{}).first->second);
                    std::cout << "arg: " << *cmd << std::endl;
                } while (cmd != script.begin() && *--cmd != ";");
                func.args.shrink_to_fit();
            }

            cmd = end;
            std::type_index last_type = typeid(void);

            while (*++cmd != "end") {
                std::cout << "cmd: " << *cmd << std::endl;
                if (cmd->starts_with('\"')) {
                    last_type = typeid(std::string);
                    cmd->erase(0,1);
                    cmd->pop_back();
                    func.cmd.push_back([value=*cmd](stack_t& stack){ stack.push(value); });
                }
                else if (*cmd == "return") {
                    func.cmd.emplace_back(func_ret);
                }
                else if (*cmd == ";") {
                    if (*(cmd-1) != "return") {
                        last_type = typeid(void);
                        func.cmd.push_back([](stack_t& stack){ while (!stack.empty()) stack.pop(); });
                    }
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
                        if (*(cmd-1) != ";") {
                            func.cmd.push_back([var](stack_t& stack){
                                *var = stack.top();
                                stack.pop();
                            });
                            *var = typed_any[std::type_index(last_type)];
                        }
                        ++cmd;
                    }
                    else if (auto b = basic.find(*cmd); b != basic.end()) {
                        try {
                            func.cmd.push_back(b->second.at(std::type_index(last_type)));
                        }
                        catch (std::out_of_range) {
                            std::cout << "\e[1m\e[31mError!\e[0m\' " << *cmd << "\' was not declared " << "for \'" << last_type.name() << "\' type\e[0m" << ((last_type.name() == "v") ? " (not initialized)!" : "!" ) << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (auto f = funcs.find(*cmd); f != funcs.end()) {
                        func.cmd.push_back(f->second.run_on_stack());
                    }
                    else if (auto v = func.vars.find(*cmd); v != func.vars.end()) {
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
    std::cout << std::any_cast<double>(funcs["sqr"].run({{"x", 10.0}})) << std::endl;
    std::cout << std::any_cast<int>(funcs["dif"].run({{"a", 3}, {"b", 10}})) << std::endl;

    stack_t stack;
    stack.push(12);
    funcs["sqr"].run_on_stack()(stack);
    std::cout << std::any_cast<int>(stack.top()) << std::endl;

    std::cout << std::any_cast<int>(funcs["dif"](8,2)) << std::endl;

    return 0;
}
