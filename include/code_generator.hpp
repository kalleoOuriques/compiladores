#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/**
 * @brief Gerador de Código Intermediário (TAC - Three Address Code).
 * * Esta classe é responsável por linearizar a árvore sintática.
 * Utiliza o conceito de "Temporários Virtuais" (t0, t1, t2...) infinitos para
 * armazenar resultados intermediários de expressões, abstraindo a complexidade
 * de alocação de registradores reais da máquina alvo.
 * Também gerencia a criação de Labels únicos (L0, L1...) para controle de fluxo.
 */

class CodeGenerator
{
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