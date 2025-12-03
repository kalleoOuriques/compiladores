#include "code_generator.hpp"

std::string CodeGenerator::newTemp() {
    return "t" + std::to_string(tempCount++);
}

std::string CodeGenerator::newLabel() {
    return "L" + std::to_string(labelCount++);
}

void CodeGenerator::emit(const std::string &instr) {
    code.push_back(instr);
}

void CodeGenerator::emit(const std::string &dest, const std::string &src) {
    code.push_back(dest + " = " + src);
}

void CodeGenerator::emit(const std::string &dest, const std::string &arg1, const std::string &op, const std::string &arg2) {
    code.push_back(dest + " = " + arg1 + " " + op + " " + arg2);
}

void CodeGenerator::emitLabel(const std::string &label) {
    code.push_back(label + ":");
}

void CodeGenerator::printCode() const {
    std::cout << "\n=== Código Intermediário (TAC) ===\n";
    for (const auto &line : code) {
        std::cout << line << "\n";
    }
}