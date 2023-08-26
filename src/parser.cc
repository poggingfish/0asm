#include "parser.hh"
#include <iostream>
#include <sstream>
std::vector<Instruction> parse(std::string contents)
{
    int inst = 0;
    std::vector<Instruction> bytecode;
    bool awaiting_arg = false;
    for (int i = 0; i < (int)contents.length(); i++)
    {
        if (contents[i] == '\n' or contents[i] == ' ')
        {
            continue;
        }
        bool next_empty = false;
        if (i + 1 >= (int)contents.length() || contents[i + 1] == ' ' || contents[i + 1] == '\n')
        {
            next_empty = true;
        }
        if (contents[i] == 'i')
        {
            if (awaiting_arg)
            {
                std::cout << "Reached another instruction before recieving an argument." << std::endl;
                exit(1);
            }
            if (next_empty)
            {
                std::cout << "Instruction token with no integer." << std::endl;
                exit(1);
            }
            std::stringstream istr;
            while (i < (int)contents.length() && (contents[i + 1] != ' ' && contents[i + 1] != '\n'))
            {
                char c = contents[++i];
                if (c == 'a')
                {
                    awaiting_arg = true;
                    continue;
                }
                istr << c;
            }
            inst = std::stoi(istr.str());
            if (!awaiting_arg)
            {
                Instruction *instruction = new Instruction(inst);
                bytecode.push_back(*instruction);
            }
        }
        else if ((contents[i] >= 48 && contents[i] <= 57) || contents[i] == '-')
        {
            if (!awaiting_arg)
            {
                std::cout << "Argument provided before instruction requiring argument." << std::endl;
                exit(1);
            }
            if (contents[i] == '-' and next_empty)
            {
                std::cout << "Integers cannot just be a single minus sign." << std::endl;
                exit(1);
            }
            std::stringstream istr;
            while (i < (int)contents.length() && (contents[i] != ' ' && contents[i] != '\n'))
            {
                istr << contents[i++];
            }
            int arg = std::stoi(istr.str());
            awaiting_arg = false;
            Instruction *instruction = new Instruction(inst, arg);
            bytecode.push_back(*instruction);
        }
        else if (contents[i] == '\'')
        {
            if (!awaiting_arg)
            {
                std::cout << "Argument provided before instruction requiring argument." << std::endl;
                exit(1);
            }
            if (next_empty)
            {
                std::cout << "Unexpected end of file on char token." << std::endl;
                exit(1);
            }
            if (contents[i + 1] == '\'')
            {
                std::cout << "Char with no value." << std::endl;
                exit(1);
            }
            if (contents[++i] == '\\')
            {
                if (contents[++i] == '\\')
                {
                    Instruction *instruction = new Instruction(inst, '\\');
                    bytecode.push_back(*instruction);
                }
                else if (contents[i] == 'n')
                {
                    Instruction *instruction = new Instruction(inst, '\n');
                    bytecode.push_back(*instruction);
                }
                else
                {
                    std::cout << "Invalid escape character." << std::endl;
                    exit(1);
                }
            }
            else
            {
                Instruction *instruction = new Instruction(inst, contents[i]);
                bytecode.push_back(*instruction);
            }
            if (contents[++i] != '\'')
            {
                std::cout << "Unclosed char." << std::endl;
                exit(1);
            }
            awaiting_arg = false;
        }
        else if (contents[i] == '\"')
        {
            if (!awaiting_arg)
            {
                std::cout << "Argument provided before instruction requiring argument." << std::endl;
                exit(1);
            }
            if (next_empty)
            {
                std::cout << "Unexpected end of file on string token." << std::endl;
                exit(1);
            }
            bool reading = true;
            std::stringstream sbuf;
            while (reading)
            {
                char c = contents[++i];
                if (i >= (int)contents.length())
                {
                    std::cout << "Unexpected end of file on string token." << std::endl;
                    exit(1);
                }
                if (c == '\\')
                {
                    if (contents[++i] == 'n')
                    {
                        sbuf << "\n";
                    }
                    else if (contents[i] == '\\')
                    {
                        sbuf << "\\";
                    }
                    else if (contents[i] == '\"')
                    {
                        sbuf << "\"";
                    }
                    else
                    {
                        std::cout << "Invalid escape character." << std::endl;
                        exit(1);
                    }
                }
                else if (c == '"')
                {
                    reading = false;
                }
                else
                {
                    sbuf << c;
                }
            }
            Instruction *instruction = new Instruction(inst, sbuf.str());
            bytecode.push_back(*instruction);
            awaiting_arg = false;
        }
        else if (contents[i] == 'm')
        {
            if (next_empty)
            {
                std::cout << "Unexpected end of file on math token." << std::endl;
                exit(1);
            }
            switch (contents[++i])
            {
            case '+':
            {
                Instruction *instruction = new Instruction(17);
                bytecode.push_back(*instruction);
                break;
            }
            case '-':
            {
                Instruction *instruction = new Instruction(18);
                bytecode.push_back(*instruction);
                break;
            }
            case '*':
            {
                Instruction *instruction = new Instruction(19);
                bytecode.push_back(*instruction);
                break;
            }
            case '/':
            {
                Instruction *instruction = new Instruction(20);
                bytecode.push_back(*instruction);
                break;
            }
            }
        }
        else if (contents[i] == ';')
        {
            while (contents[++i] != '\n')
            {
            }
        }
    }
    if (awaiting_arg)
    {
        std::cout << "Reached EOF without an argument." << std::endl;
        exit(1);
    }
    return bytecode;
}