#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class CodeGenerator {
private:
    int tempCount = 0;
    int labelCount = 0;
    std::vector<std::string> code;

public:
    std::string newTemp();

    std::string newLabel();

    void emit(const std::string &instr);
    void emit(const std::string &dest, const std::string &src);
    void emit(const std::string &dest, const std::string &arg1, const std::string &op, const std::string &arg2);
    void emitLabel(const std::string &label);
    
    void printCode() const;
};

#endif