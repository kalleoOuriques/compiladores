#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "lexer.hpp"
#include "parser.hpp"
#include "symbol_table.hpp"

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Uso: ./compiler <arquivo.convcc>\n";
        std::cerr << "Exemplo: ./compiler test/example1.convcc\n";
        return 1;
    }

    std::ifstream f(argv[1]);
    if (!f)
    {
        std::cerr << "Erro: não foi possível abrir o arquivo '" << argv[1] << "'\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();

    std::string sourceCode = buffer.str();

    // Necessário para escrever os resultados nos arquivos de output
    std::stringstream output_buffer;
    std::streambuf *coutBuf = std::cout.rdbuf(output_buffer.rdbuf());

    // Se o arquivo estiver vazio, não há nada para analisar
    if (sourceCode.empty())
    {
        std::cout << "Programa sintaticamente correto!\n";
        std::cout << "\nTabela de símbolos:\n";
        std::cout << "(vazia)\n";
    }
    else
    {
        ASTNode::hasSemanticError = false;

        // Criar tabela de símbolos e analisador léxico
        SymbolTable lexSymtab;
        Lexer lex(sourceCode, lexSymtab);

        // Criar analisador sintático (LL(1))
        Parser parser(lex);

        // Executar análise sintática
        // Qualquer erro léxico ou sintático causará exit(1) dentro dos métodos
        parser.parse();

        // Tabela de símbolos para análise semântica (com escopos corretos)
        SymbolTable semanticSymtab;

        if (parser.root)
        {
            std::cout << "\nÁrvore AST:\n";
            parser.root->print();

            std::string result = parser.root->checkType(semanticSymtab, false);

            if (ASTNode::hasSemanticError)
            {
                std::cout.rdbuf(coutBuf); // Restore cout
                std::cerr << "Compilação falhou devido a erros semânticos.\n";
                return 1;
            }

            if (result != "ERROR")
            {
                std::cout << "\nSucesso: Expressões aritméticas válidas.\n";
                std::cout << "Sucesso: Declaração de escopos válida.\n";
                std::cout << "Sucesso: Comandos break válidos.\n";
            }
        }

        // Se chegou aqui, o programa é sintaticamente correto
        std::cout << "\nTabela de símbolos:\n";
        semanticSymtab.print();
    }

    std::cout.rdbuf(coutBuf);

    // Escreve a saída no arquivo
    fs::path inputPath(argv[1]);
    std::string filename = inputPath.stem().string();

    if (!fs::exists("output"))
    {
        fs::create_directory("output");
    }

    std::string outputPath = "output/" + filename + "-result.txt";
    std::ofstream outFile(outputPath);
    if (!outFile)
    {
        std::cerr << "Erro ao criar arquivo de saída: " << outputPath << "\n";
        return 1;
    }

    outFile << output_buffer.str();
    outFile.close();

    std::cerr << "Compilação concluída. Resultados em " << outputPath << "\n";

    return 0;
}