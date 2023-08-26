#include "oasm.hh"
#include <vector>
#include <stack>
#include <set>
#include <iostream>
#include <unordered_map>

#include <unistd.h>
#include <termios.h>

char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

class Type
{
public:
    ArgType sel;
    std::optional<int> inttype;
    std::optional<std::string> strtype;
    std::optional<void*> anytype;
    Type(std::string val)
    {
        this->strtype = val;
        this->sel = String;
    }
    Type(int val)
    {
        this->inttype = val;
        this->sel = Int;
    }
    Type(void* val)
    {
        this->anytype = val;
        this->sel = AnyType;
    }
    Type() {}
};
class Stack
{
public:
    std::stack<Type> stack;
    Type pop()
    {
        if (stack.empty())
        {
            std::cout << "[RUNTIME ERROR] Stack empty.\n";
            exit(1);
        }
        Type t = stack.top();
        stack.pop();
        return t;
    }

    void push(Instruction inst)
    {
        switch (inst.selected)
        {
        case Int:
            stack.push(Type(inst.int_arg.value()));
            break;
        case String:
            stack.push(Type(inst.string_arg.value()));
            break;
        case AnyType:
            std::cout << "[RUNTIME ERROR] You cant just push an AnyType to the stack. RTFD!" << std::endl;
            exit(1);
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
    void print()
    {
        Type i = pop();
        switch (i.sel)
        {
        case Int:
            std::cout << i.inttype.value();
            break;
        case String:
            std::cout << i.strtype.value();
            break;
        case Math:
            std::cout << "Math Object";
            break;
        case AnyType:
            std::cout << "Any";
            break;
        case None:
            std::cout << "None";
            break;
        }
    }
    void plus()
    {
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel)
        {
            std::cout << "[RUNTIME ERROR] You cant add values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel)
        {
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
    void minus()
    {
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel)
        {
            std::cout << " [RUNTIME ERROR] You cant subtract values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel)
        {
        case Int:
            stack.push(*new Type(val1.inttype.value() - val2.inttype.value()));
            break;
        default:
            std::cout << "[RUNTIME ERROR] Subtraction on invalid type." << std::endl;
            exit(1);
        }
    }
    void multiply()
    {
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel)
        {
            std::cout << "[RUNTIME ERROR] You cant multiply values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel)
        {
        case Int:
            stack.push(*new Type(val1.inttype.value() * val2.inttype.value()));
            break;
        default:
            std::cout << "[RUNTIME ERROR] Multiplication on invalid type." << std::endl;
            exit(1);
        }
    }
    void divide()
    {
        Type val1 = pop();
        Type val2 = pop();
        if (val1.sel != val2.sel)
        {
            std::cout << "[RUNTIME ERROR] You cant divide values of two different types." << std::endl;
            exit(1);
        }
        switch (val1.sel)
        {
        case Int:
            stack.push(*new Type(val1.inttype.value() / val2.inttype.value()));
            break;
        default:
            std::cout << "[RUNTIME ERROR] Divison on invalid type." << std::endl;
            exit(1);
        }
    }
    bool eq()
    {
        Type val1 = pop();
        Type val2 = pop();

        if (val1.sel != val2.sel)
        {
            return false;
        }
        switch (val1.sel)
        {
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
    bool gt()
    {
        Type val1 = pop();
        Type val2 = pop();

        if (val1.sel != val2.sel)
        {
            std::cout << "[RUNTIME ERROR] You cant do a greater than on two values that are of different types." << std::endl;
            exit(1);
        }
        switch (val1.sel)
        {
        case Int:
            return val1.inttype.value() > val2.inttype.value();
            break;
        default:
            std::cout << "[RUNTIME ERROR] Greater than only works on ints." << std::endl;
            exit(1);
        }
        return false;
    }
    Stack() {}
};
void execute(std::vector<Instruction> bytecode)
{
    Stack stack;
    std::stack<int> call_stack;
    std::unordered_map<int, int> labels = {};
    std::unordered_map<int, Type> mem = {};
    std::unordered_map<std::string, int> strmem = {};
    std::deque<int> unallocated;
    for (int ptr = 0; ptr < 1000; ptr++)
    {
        unallocated.push_front(ptr);
    }
    for (int i = 0; i < (int)bytecode.size(); i++)
    {
        switch (bytecode[i].inst)
        {
        case 3:
            if (bytecode[i].selected != Int)
            {
                std::cout << "[RUNTIME ERROR] Inst 3 expects a int." << std::endl;
                exit(1);
            }
            labels[bytecode[i].int_arg.value()] = i;
            break;
        default:
            break;
        }
    }
    for (int i = 0; i < (int)bytecode.size(); i++)
    {
        switch (bytecode[i].inst)
        {
        case 0:
            stack.push(bytecode[i]);
            break;
        case 1:
            stack.print();
            break;
        case 2:
        {
            Type c = stack.pop();
            if (c.sel != Int)
            {
                std::cout << "[RUNTIME ERROR] Inst 2 expects a int." << std::endl;
                exit(1);
            }
            std::cout << (char)c.inttype.value();
            break;
        }
        case 4:
            if (bytecode[i].selected != Int)
            {
                std::cout << "[RUNTIME ERROR] Inst 4 expects a int." << std::endl;
                exit(1);
            }
            i = labels[bytecode[i].int_arg.value()];
            break;
        case 5:
        {
            if (bytecode[i].selected != Int && bytecode[i].selected != String)
            {
                std::cout << "[RUNTIME ERROR] Inst 5 expects a int or string." << std::endl;
                exit(1);
            }
            Type m = stack.pop();
            if (bytecode[i].selected == Int)
            {
                mem[bytecode[i].int_arg.value()] = m;
            }
            else
            {
                mem[strmem[bytecode[i].string_arg.value()]] = m;
            }
            break;
        }
        case 6:
            if (bytecode[i].selected != Int && bytecode[i].selected != String)
            {
                std::cout << "[RUNTIME ERROR] Inst 6 expects a int or string." << std::endl;
                exit(1);
            }
            if (bytecode[i].selected == Int)
            {
                stack.stack.push(mem[bytecode[i].int_arg.value()]);
            }
            else
            {
                stack.stack.push(mem[strmem[bytecode[i].string_arg.value()]]);
            }
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
            if (stack.eq())
                stack.stack.push(*new Type(1));
            else
                stack.stack.push(*new Type(0));
            break;
        case 9:
        {
            if (bytecode[i].selected != Int)
            {
                std::cout << "[RUNTIME ERROR] Inst 9 expects a int." << std::endl;
                exit(1);
            }
            Type v = stack.pop();
            if (v.sel != Int)
            {
                std::cout << "[RUNTIME ERROR] Stack does not contain an integer on Inst 9";
            }
            if (v.inttype.value() == 1)
            {
                i = labels[bytecode[i].int_arg.value()];
            }
            break;
        }
        case 10:
        {
            if (bytecode[i].selected != Int)
            {
                std::cout << "[RUNTIME ERROR] Inst 10 expects a int." << std::endl;
                exit(1);
            }
            Type v = stack.pop();
            if (v.sel != Int)
            {
                std::cout << "[RUNTIME ERROR] Stack does not contain an integer on Inst 10";
                exit(1);
            }
            if (v.inttype.value() == 0)
            {
                i = labels[bytecode[i].int_arg.value()];
            }
            break;
        }
        case 11:
        {
            stack.stack.push(stack.stack.top());
            break;
        }
        case 12:
        {
            if (stack.gt())
                stack.stack.push(*new Type(0));
            else
                stack.stack.push(*new Type(1));
            break;
        }
        case 13:
        {
            Type offset = stack.pop();
            if (offset.sel != Int)
            {
                std::cout << "[RUNTIME ERROR] i13 expects an integer offset.\n";
                exit(1);
            }
            if (i + 1 >= (int)bytecode.size())
            {
                std::cout << "[RUNTIME ERROR] i13 expects a instruction to exist after it.\n";
                exit(1);
            }
            call_stack.push(i + 1 + offset.inttype.value());
            break;
        }
        case 14:
            if (call_stack.size() == 0)
            {
                i = labels[0];
            }
            else
            {
                i = call_stack.top();
                call_stack.pop();
            }
            break;
        case 15:
        {
            if (bytecode[i].selected != String)
            {
                printf("[RUNTIME ERROR] Expected a string name for inst 15!\n");
                exit(1);
            }
            if (unallocated.empty())
            {
                printf("[RUNTIME ERROR] Out of memory!\n");
                exit(1);
            }
            std::string name = bytecode[i].string_arg.value();
            int loc = unallocated.back();
            unallocated.pop_back();
            strmem[name] = loc;
            break;
        }
        case 16:
            if (bytecode[i].selected != String)
            {
                printf("[RUNTIME ERROR] Expected a string name for inst 15!\n");
                exit(1);
            }
            unallocated.push_back(strmem[bytecode[i].string_arg.value()]);
            strmem.erase(bytecode[i].string_arg.value());
            break;
        case 17:
            stack.plus();
            break;
        case 18:
            stack.minus();
            break;
        case 19:
            stack.multiply();
            break;
        case 20:
            stack.divide();
            break;
        case 21:
            stack.stack.push(Type(getch()));
            break;
        case 22:
        {
            Type file = stack.pop();
            if (file.sel != AnyType) {
                std::cout << "i22 expected AnyType for fd." << std::endl;
                exit(1);
            }
            fflush((FILE*)file.anytype.value());
            break;
        }
        case 23:
        {
            Type type = stack.pop();
            Type mode = stack.pop();
            if (type.sel != Int){
                std::cout << "i23 expects an int." << std::endl;
                exit(1);
            }
            if (mode.sel != String){
                std::cout << "i23 expects a string." << std::endl;
                exit(1);
            }
            FILE *fd = fdopen(type.inttype.value(), mode.strtype.value().c_str());
            stack.stack.push(Type((void*)fd));
            break;
        }
        case 24:
        {
            Type character = stack.pop();
            Type file = stack.pop();
            if (file.sel != AnyType) {
                std::cout << "i24 expected AnyType for file." << std::endl;
                exit(1);
            }
            if (character.sel != Int) {
                std::cout << "i24 expected int for char." << std::endl;
                exit(1);
            }
            fputc(character.inttype.value(),(FILE*)file.anytype.value());
            break;
        }
        case 25:
            Type str = stack.pop();
            Type file = stack.pop();
            if (file.sel != AnyType) {
                std::cout << "i25 expected AnyType for file." << std::endl;
                exit(1);
            }
            if (str.sel != String) {
                std::cout << "i25 expected string." << std::endl;
                exit(1);
            }
            fputs(str.strtype.value().c_str(),(FILE*)file.anytype.value());
            break;
        }
    }
}