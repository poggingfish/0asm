#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "oasm.hh"
#include "execute.hh"
#include "parser.hh"

int main(int argc, char *argv[]){
    if (argc != 2){
        std::cout << "Usage: 0asm <file>" << std::endl;
        return 1;
    }
    std::fstream infile;
    std::stringstream buff;
    infile.open(argv[1], std::fstream::in);
    if (!infile.is_open()){
        std::cout << "Invalid file." << std::endl;
        return 1;
    }
    buff << infile.rdbuf();
    std::string contents = buff.str();
    std::vector<Instruction> bytecode = parse(contents);
    execute(bytecode);
}