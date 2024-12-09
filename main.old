#include <iostream>

#include "parser.hpp"

#include <stack>
#include <any>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <map>
#include <initializer_list>
#include <memory>

//std::variant<std::any,std::any*> variable

struct Function;

typedef std::stack<std::any> stack_t;
typedef std::function<void(stack_t&)> func_t;

extern std::unordered_map<std::string, std::unordered_map<std::type_index, func_t>> basic;

template <typename T>
void say(stack_t& stack) {
    //~ std::cout << "[say]"; ///DEBUG
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
    //~ std::cout << "[*]"; ///DEBUG
    auto b = std::any_cast<T>(stack.top());
    stack.pop();
    auto a = std::any_cast<T>(stack.top());
    stack.pop();
    //~ std::cout << a << " * " << b; ///DEBUG
    stack.push(a*b);
}

template <typename T>
void dif(stack_t& stack) {
    //~ std::cout << "[-]"; ///DEBUG
    auto b = std::any_cast<T>(stack.top());
    stack.pop();
    auto a = std::any_cast<T>(stack.top());
    stack.pop();
    //~ std::cout << a << " - " << b; ///DEBUG
    stack.push(a-b);
}

template <typename T>
void eq(stack_t& stack) {
    //~ std::cout << "[eq] ";   ///DEBUG
    auto b = std::any_cast<T>(stack.top());
    stack.pop();
    auto a = std::any_cast<T>(stack.top());
    stack.pop();
    //~ std::cout << a << " = " << b; ///DEBUG
    stack.push(a == b);
}

template <typename T>
void lt(stack_t& stack) {
    //~ std::cout << "[lt] ";   ///DEBUG
    auto b = std::any_cast<T>(stack.top());
    stack.pop();
    auto a = std::any_cast<T>(stack.top());
    stack.pop();
    //~ std::cout << a << " < " << b; ///DEBUG
    stack.push(a < b);
}

template <typename T>
void inc(stack_t& stack) {
    //~ std::cout << "[inc]";
    ++std::any_cast<T&>(*std::any_cast<std::any*>(stack.top()));
}

#define FUNC_VOID(_name) {typeid(void), [](stack_t& stack){ basic[_name].at(std::type_index(stack.top().type()))(stack); }}
#define FUNC(type, name) {typeid(type), name<type>}
#define FUNC_BASIC_TYPES(name) FUNC(int,name), FUNC(double,name), FUNC(char,name)

std::unordered_map<std::string, std::unordered_map<std::type_index, func_t>> basic {
    {"say", { FUNC_VOID("say"), FUNC_BASIC_TYPES(say), FUNC(std::string,say) }},
    {"*", { FUNC_VOID("*"), FUNC(int,prod), FUNC(double,prod)  }},
    {"-", { FUNC_VOID("-"), FUNC(int,dif), FUNC(double,dif) }},
    {"<", { FUNC_VOID("<"), FUNC(int,lt), FUNC(double,lt) }},
    {"=", { FUNC_VOID("="), FUNC_BASIC_TYPES(eq), FUNC(std::string,eq) } },
    {"inc", { FUNC_VOID("inc"), FUNC(int,inc), FUNC(double,inc) }}
};

std::unordered_map<std::type_index, std::any> typed_any {
    {typeid(int), int()},
    {typeid(double), double()},
    {typeid(char), char()},
    {typeid(std::string), std::string()}
};

struct Function {
    Function(){}
    std::string name;
    stack_t* stack;
    std::vector<func_t> cmd;
    std::vector<func_t>::iterator cmd_iter;
    std::unordered_map<std::string, std::any> vars; // may be optimized (erased nahren)
    std::vector<std::any*> args;

    func_t run_on_stack() {
        return [this](stack_t& stack) {
            //~ std::cout << "[" << name << "]";
            for (auto& arg : args) {
                if (stack.empty()) {
                    std::cout << "\e[31m\e[1mError!\e[0m Too few args for the function \'" << name << "\'" << " (expected " << args.size() << ")" << std::endl;
                    exit(EXIT_FAILURE);
                }
                *arg = stack.top();
                stack.pop();
            }
            for (cmd_iter = cmd.begin(); cmd_iter != cmd.end(); ++cmd_iter) {
                //~ std::cout << " { ";
                (*cmd_iter)(stack);
                //~ std::cout << " } " << std::endl;
            }
        };
    }

    template <typename ...Args>
    std::any operator() (const Args&... a) {
        stack = new stack_t;
        //~ std::cout << "[" << name << "]";
        int n = 0;
        for (auto& arg : (std::initializer_list<std::any>{a...})) {
            if (n == args.size()) {
                std::cout << "\e[31m\e[1mError!\e[0m Too many args for the function \'" << name << "\'" << " (expected " << args.size() << ")" << std::endl;
                exit(EXIT_FAILURE);
            }
            *(args[n]) = arg;
            ++n;
        }
        // if n < args.size() -> too few args

        for (cmd_iter = cmd.begin(); cmd_iter != cmd.end(); ++cmd_iter) {
            //~ std::cout << " { ";
            (*cmd_iter)(*stack);
            //~ std::cout << " } " << std::endl;
        }

        std::any result;
        if (!stack->empty()) result = stack->top();
        delete stack;
        return result;
    }

    std::any run(std::vector<std::pair<std::string, std::any>> _args) {
        stack = new stack_t;
        //~ std::cout << "[" << name << "]";
        for (auto& v : _args) {
            if (auto var = vars.find(v.first); var != vars.end()) {
                var->second = v.second;
            } else {
                std::cout << "\e[1m\e[31mError!\e[0m The function '" << name << "' has no the parameter \'" << v.first << "\'\e[0m";
                exit(EXIT_FAILURE);
            }
        }

        for (cmd_iter = cmd.begin(); cmd_iter != cmd.end(); ++cmd_iter) {
            //~ std::cout << " { ";
            (*cmd_iter)(*stack);
            //~ std::cout << " } " << std::endl;
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
            std::stack<size_t> if_l, else_l, loop_l, loop_l1; //labels
            std::stack<std::vector<size_t>> elif_l, elif_l1;
            func.name = *cmd;
            std::cout << "func: " << func.name << std::endl;

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
                    func.cmd.push_back([value=*cmd](stack_t& stack){ stack.push(value); /*std::cout << "[\"" << value << "\"]"*/; });
                }
                else if (*cmd == "loop") {
                    int n = 1;
                    auto pos = cmd;
                    while (*--cmd != ";") ++n; cmd = pos;
                    loop_l1.push(func.cmd.size() - n);
                    func.cmd.emplace_back();
                    loop_l.push(func.cmd.size()-1);
                }
                else if (*cmd == "endl") {
                    func.cmd.push_back([foo=&func,loop=loop_l1.top()](stack_t& stack){ /*std::cout << "[endl]",*/ foo->cmd_iter = foo->cmd.begin() + loop; });
                    func.cmd[loop_l.top()] = [foo=&func,endl=func.cmd.size()-1](stack_t& stack){
                        //~ std::cout << "[loop]";
                        if (!std::any_cast<bool>(stack.top())) foo->cmd_iter = foo->cmd.begin() + endl/*, std::cout << " false"; else std::cout << " true"*/;
                        stack.pop();
                    };
                    loop_l1.pop();
                    loop_l.pop();
                }
                else if (*cmd == "if") {
                    elif_l.push({}); elif_l1.push({});
                    func.cmd.emplace_back();
                    if_l.push(func.cmd.size()-1);
                }
                else if (*cmd == "else") {
                    func.cmd.emplace_back();
                    else_l.push(func.cmd.size()-1);
                }
                else if (*cmd == "elif") {
                    int n = 1;
                    auto pos = cmd;
                    while (*--cmd != ";") ++n; cmd = pos;
                    elif_l1.top().push_back(func.cmd.emplace(func.cmd.begin() + func.cmd.size() - n) - func.cmd.begin());

                    func.cmd.emplace_back();
                    elif_l.top().push_back(func.cmd.size()-1);
                }
                else if (*cmd == "endif") {
                    if (!elif_l.top().empty()) {
                        func.cmd[if_l.top()] = [foo=&func,endif=elif_l1.top()[0]](stack_t& stack){
                            //~ std::cout << "[if_elif]";
                            if (!std::any_cast<bool>(stack.top())) foo->cmd_iter = foo->cmd.begin() + endif/*, std::cout << " false"; else std::cout << " true"*/;
                            stack.pop();
                        };
                        int n = 0;
                        for (auto elif : elif_l.top()) {
                            func.cmd[elif_l1.top()[n]] = [foo=&func,endif=func.cmd.size()-1](stack_t& stack){
                                //~ std::cout << "[elif-endif]";
                                foo->cmd_iter = foo->cmd.begin() + endif;
                            };

                            if (n+1 < elif_l.top().size()) {
                                func.cmd[elif] = [foo=&func,endif=elif_l1.top()[n+1]](stack_t& stack) {
                                    //~ std::cout << "[elif]";
                                    if (!std::any_cast<bool>(stack.top())) foo->cmd_iter = foo->cmd.begin() + endif/*, std::cout << " false"; else std::cout << " true"*/;
                                    stack.pop();
                                };
                            }
                            else if (else_l.size() == if_l.size()) {
                                func.cmd[elif] = [foo=&func,endif=else_l.top()](stack_t& stack) {
                                    //~ std::cout << "[elif-else]";
                                    if (!std::any_cast<bool>(stack.top())) foo->cmd_iter = foo->cmd.begin() + endif/*, std::cout << " false"; else std::cout << " true"*/;
                                    stack.pop();
                                };
                                func.cmd[else_l.top()] = [foo=&func,endif=func.cmd.size()-1](stack_t& stack) {
                                    //~ std::cout << "[else_elif]";
                                    foo->cmd_iter = foo->cmd.begin() + endif;
                                };
                                else_l.pop();
                            }
                            else {
                                func.cmd[elif] = [foo=&func,endif=func.cmd.size()-1](stack_t& stack){
                                    //~ std::cout << "[elif-last]";
                                    if (!std::any_cast<bool>(stack.top())) foo->cmd_iter = foo->cmd.begin() + endif/*, std::cout << " false"; else std::cout << " true"*/;
                                    stack.pop();
                                };
                            }
                            ++n;
                        }
                    }
                    else if (else_l.size() !=  if_l.size()) {
                        func.cmd[if_l.top()] = [foo=&func,endif=func.cmd.size()-1](stack_t& stack){
                            //~ std::cout << "[if]";
                            if (!std::any_cast<bool>(stack.top())) foo->cmd_iter = foo->cmd.begin() + endif/*, std::cout << " false"; else std::cout << " true"*/;
                            stack.pop();
                        };
                    } else {
                        func.cmd[if_l.top()] = [foo=&func,endif=else_l.top()](stack_t& stack){
                            //~ std::cout << "[if_else]";
                            if (!std::any_cast<bool>(stack.top())) foo->cmd_iter = foo->cmd.begin() + endif/*, std::cout << " false"; else std::cout << " true"*/;
                            stack.pop();
                        };
                        func.cmd[else_l.top()] = [foo=&func,endif=func.cmd.size()-1](stack_t& stack) {
                            //~ std::cout << "[else]";
                            foo->cmd_iter = foo->cmd.begin() + endif;
                        };
                        else_l.pop();
                    }
                    if_l.pop();
                    elif_l.pop();
                    elif_l1.pop();
                }
                else if (*cmd == "return") {
                    func.cmd.emplace_back([foo=&func](stack_t& stack){ foo->cmd_iter = foo->cmd.end()-1; /*std::cout << "[return]";*/ });
                }
                else if (*cmd == ";") {
                    if (*(cmd-1) != "return" && *(cmd-1) != "endif") {
                        last_type = typeid(void);
                        func.cmd.push_back([](stack_t& stack){ while (!stack.empty()) stack.pop(); /*std::cout << "[;]";*/ });
                    }
                }
                else if (std::isdigit((*cmd)[0])) {
                    if (cmd->contains(".")) {
                        last_type = typeid(double);
                        func.cmd.push_back([value=std::stod(*cmd)](stack_t& stack){ stack.push(value); /*std::cout << "[" << value << "]";*/ });
                    } else {
                        last_type = typeid(int);
                        func.cmd.push_back([value=std::stoi(*cmd)](stack_t& stack){ stack.push(value); /*std::cout << "[" << value << "]";*/ });
                    }
                }
                else {
                    if (cmd[1] == "var") {
                        std::any* var = &func.vars.emplace(*cmd, std::any{}).first->second;
                        if (*(cmd-1) != ";") {
                            func.cmd.push_back([var](stack_t& stack){
                                //~ std::cout << "[var set]";
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
                            std::cout << "\e[1m\e[31mError!\e[0m\' " << *cmd << "\' was not declared " << "for type \'" << ((last_type.name() == "v") ? "\' (not initialized)" : "\'" ) << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (auto f = funcs.find(*cmd); f != funcs.end()) {
                        func.cmd.push_back(f->second.run_on_stack());
                    }
                    else if (auto v = func.vars.find(*cmd); v != func.vars.end()) {
                        last_type = std::type_index(v->second.type());
                        if (cmd[1] == "inc")
                            func.cmd.push_back([value=&v->second/*DEBUG*/,name=v->first](stack_t& stack){ stack.push(value);/* std::cout << "[&" << name << "]"; */});
                        else
                            func.cmd.push_back([value=&v->second/*DEBUG*/,name=v->first](stack_t& stack){ stack.push(*value); /*std::cout << "[" << name << "]";*/ });
                    }
                    else if (auto g = vars.find(*cmd); g != vars.end()) {
                        last_type = std::type_index(g->second.type());
                        if (cmd[1] == "inc")
                            func.cmd.push_back([value=&v->second/*DEBUG*/,name=v->first](stack_t& stack){ stack.push(value);/* std::cout << "[&" << name << "]"; */});
                        else
                            func.cmd.push_back([value=&g->second/*DEBUG*/,name=v->first](stack_t& stack){ stack.push(*value);  /*std::cout << "[" << name << "]"; */});
                    }
                    else {
                        std::cout << "\e[1m\e[31mError!\e[0m\' " << *cmd << "\' was not declared" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            } ++cmd;
            func.cmd.shrink_to_fit();
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
