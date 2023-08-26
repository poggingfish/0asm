#include <optional>
#include <string>
#ifndef __OASM_HH
#define __OASM_HH
enum ArgType
{
    String,
    Int,
    None,
    AnyType,
    Math
};

class Instruction
{
public:
    int inst;
    ArgType selected;
    std::optional<std::string> string_arg;
    std::optional<int> int_arg;
    Instruction(int inst, std::string arg)
    {
        this->inst = inst;
        this->string_arg = arg;
        this->selected = String;
    }
    Instruction(int inst, int arg)
    {
        this->inst = inst;
        this->int_arg = arg;
        this->selected = Int;
    }
    Instruction(int inst)
    {
        this->inst = inst;
        this->selected = None;
    }
};
#endif