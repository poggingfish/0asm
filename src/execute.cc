#include "oasm.hh"
#include <vector>
#include <stack>
#include <set>
#include <iostream>
#include <unordered_map>
class Type{
    public:
    ArgType sel;
    std::optional<int> inttype;
    std::optional<std::string> strtype;
    Type(std::string val){
        this->strtype = val;
        this->sel = String;
    }
    Type(int val){
        this->inttype = val;
        this->sel = Int;
    }
};
class Stack{
    public:
    std::stack<Type> stack;
    Type pop(){
        if (stack.empty()){
            std::cout << "[RUNTIME ERROR] Stack empty.\n";
            exit(1);
        }
        Type t = stack.top();
        stack.pop();
        return t;
    }

    void push(Instruction inst){
        switch (inst.selected){
            case Int:
                stack.push(Type(inst.int_arg.value()));
                break;
            case String:
                stack.push(Type(inst.string_arg.value()));
                break;
            case Math:
                std::cout << "[RUNTIME ERROR] You cant push Math to the stack." << std::endl;
                exit(1);
                break;
            case None:
                std::cout << "[RUNTIME ERROR] You cant push None to the stack." << std::endl;
                exit(1);
                break;
        }
    }
    void print(){
        Type i = pop();
        switch (i.sel){
            case Int:
                std::cout << i.inttype.value();
                break;
            case String:
                std::cout << i.strtype.value();
                break;
            case Math:
                std::cout << "Math Object";
                break;
            case None:
                std::cout << "None";
                break;
        }
    }
    void plus(){
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel){
            std::cout << "[RUNTIME ERROR] You cant add values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel){
            case Int:
                stack.push(*new Type(val1.inttype.value() + val2.inttype.value()));
                break;
            case String:
                stack.push(*new Type(val1.strtype.value() + val2.strtype.value()));
                break;
            default:
                std::cout << "[RUNTIME ERROR] Addition on invalid type." << std::endl;
                exit(1);
        }
    }
    void minus(){
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel){
            std::cout << " [RUNTIME ERROR] You cant subtract values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel){
            case Int:
                stack.push(*new Type(val1.inttype.value() - val2.inttype.value()));
                break;
            default:
                std::cout << "[RUNTIME ERROR] Subtraction on invalid type." << std::endl;
                exit(1);
        }
    }
    void multiply(){
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel){
            std::cout << "[RUNTIME ERROR] You cant multiply values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel){
            case Int:
                stack.push(*new Type(val1.inttype.value() * val2.inttype.value()));
                break;
            default:
                std::cout << "[RUNTIME ERROR] Multiplication on invalid type." << std::endl;
                exit(1);
        }
    }
    void divide(){
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel){
            std::cout << "[RUNTIME ERROR] You cant divide values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel){
            case Int:
                stack.push(*new Type(val1.inttype.value() / val2.inttype.value()));
                break;
            default:
                std::cout << "[RUNTIME ERROR] Divison on invalid type." << std::endl;
                exit(1);
        }
    }
    bool eq(){
        Type val1 = pop();
        Type val2 = pop();

        if (val1.sel != val2.sel){
            return false;
        }
        switch (val1.sel){
            case Int:
                return val1.inttype.value() == val2.inttype.value();
                break;
            case String:
                return val1.strtype.value() == val2.strtype.value();
                break;
            case None:
                return true;
                break;
            case Math:
                return true;
                break;
        }
        return false;
    }
    bool gt(){
        Type val1 = pop();
        Type val2 = pop();

        if (val1.sel != val2.sel){
            std::cout << "[RUNTIME ERROR] You cant do a greater than on two values that are of different types." << std::endl;
            exit(1);
        }
        switch (val1.sel){
            case Int:
                return val1.inttype.value() > val2.inttype.value();
                break;
            default:
                std::cout << "[RUNTIME ERROR] Greater than only works on ints." << std::endl;
                exit(1);
        }
        return false;
    }
    Stack(){}
};
void execute(std::vector<Instruction> bytecode){
    Stack stack;
    int *labels = (int*)malloc(sizeof(int)*1000000); // Max of 1,000,000
    Type *mem = (Type*)malloc(sizeof(Type)*1000000); // Max of 1,000,000 memory locations.
    for (int i = 0; i < (int)bytecode.size(); i++){
        switch(bytecode[i].inst){
            case 3:
                if (bytecode[i].selected != Int){
                    std::cout << "[RUNTIME ERROR] Inst 3 expects a int." << std::endl;
                    exit(1);
                }
                labels[bytecode[i].int_arg.value()] = i;
                break;
            default: break;
        }
    }
    for (int i = 0; i < (int)bytecode.size(); i++){
        if (bytecode[i].selected == Math){
            switch(bytecode[i].mathOp){
                case Plus:
                    stack.plus();
                    break;
                case Minus:
                    stack.minus();
                    break;
                case Multiply:
                    stack.multiply();
                    break;
                case Divide:
                    stack.divide();
                    break;
                default:
                    std::cout << "[RUNTIME ERROR] Something is not implemented in execute.cc" << std::endl;
                    exit(1);
            }
            continue;
        }
        switch(bytecode[i].inst){
            case 0:
                stack.push(bytecode[i]);
                break;
            case 1:
                stack.print();
                break;
            case 2:
            {
                Type c = stack.pop();
                if (c.sel != Int){
                    std::cout << "[RUNTIME ERROR] Inst 2 expects a int." << std::endl;
                    exit(1);
                }
                std::cout << (char)c.inttype.value();
                break;
            }
            case 4:
                if (bytecode[i].selected != Int){
                    std::cout << "[RUNTIME ERROR] Inst 4 expects a int." << std::endl;
                    exit(1);
                }
                i = labels[bytecode[i].int_arg.value()];
                break;
            case 5:
            {
                if (bytecode[i].selected != Int){
                    std::cout << "[RUNTIME ERROR] Inst 5 expects a int." << std::endl;
                    exit(1);
                }
                Type m = stack.pop();
                mem[bytecode[i].int_arg.value()] = m;
                break;
            }
            case 6:
                if (bytecode[i].selected != Int){
                    std::cout << "[RUNTIME ERROR] Inst 6 expects a int." << std::endl;
                    exit(1);
                }
                stack.stack.push(mem[bytecode[i].int_arg.value()]);
                break;
            case 7:
            {
                Type i1 = stack.pop();
                Type i2 = stack.pop();
                stack.stack.push(i1);
                stack.stack.push(i2);
                break;
            }
            case 8:
                if (stack.eq()) stack.stack.push(*new Type(0)); else stack.stack.push(*new Type(1));
                break;
            case 9:
            {
                if (bytecode[i].selected != Int){
                    std::cout << "[RUNTIME ERROR] Inst 9 expects a int." << std::endl;
                    exit(1);
                }
                Type v = stack.pop();
                if (v.sel != Int){
                    std::cout << "[RUNTIME ERROR] Stack does not contain an integer on Inst 9";
                }
                if (v.inttype.value() == 1){
                    i = labels[bytecode[i].int_arg.value()];
                }
                break;
            }
            case 10:
            {
                if (bytecode[i].selected != Int){
                    std::cout << "[RUNTIME ERROR] Inst 10 expects a int." << std::endl;
                    exit(1);
                }
                Type v = stack.pop();
                if (v.sel != Int){
                    std::cout << "[RUNTIME ERROR] Stack does not contain an integer on Inst 10";
                }
                if (v.inttype.value() == 0){
                    i = labels[bytecode[i].int_arg.value()];
                }
                break;
            }
            case 11:
            {
                stack.stack.push(stack.stack.top());
            }
            case 12:
            {
                if (stack.gt()) stack.stack.push(*new Type(0)); else stack.stack.push(*new Type(1));
                break;
            }
        }
    }
}