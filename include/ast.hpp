#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "symbol_table.hpp"
#include "code_generator.hpp"

class ASTNode;
class ExprNode;
class StmtNode;

/**
 * @brief Definição da hierarquia de classes da Árvore Sintática Abstrata (AST).
 *
 * Este arquivo contém as classes que representam a estrutura hierárquica do código fonte
 * após a análise sintática. É o componente central para a Análise Semântica (Fase 3)
 * e servirá de base para a Geração de Código Intermediário (Fase 4).
 *
 * --- ESTRUTURA ---
 * A hierarquia baseia-se na classe abstrata `ASTNode`, ramificando-se em:
 * - `ExprNode`: Nós que representam expressões e retornam valores (Literais, Operações Binárias, Acesso a Variáveis/Arrays).
 * - `StmtNode`: Nós que representam comandos/instruções (If, For, While, Atribuições, Blocos).
 * - Estruturas de Alto Nível: `ProgramNode` e `FuncDefNode`.
 *
 * --- FUNCIONALIDADES IMPLEMENTADAS NOS NÓS ---
 *
 * 1. Visualização da Árvore (Método `print`):
 * - Percorre a árvore recursivamente imprimindo a estrutura indentada.
 *
 * 2. Análise Semântica (Método `checkType`):
 * - Cada nó é responsável por validar a si mesmo.
 * - Validação de Tipos: Garante que operações aritméticas e atribuições sejam compatíveis (ex: não somar string com int).
 * - Gerenciamento de Escopo: Interage com a `SymbolTable` para registrar variáveis (`VarDeclNode`) e verificar existência (`VarAccess`).
 * - Atualização de Tabela: Preenche os tipos das variáveis na tabela de símbolos para a saída final.
 * - Validação de Contexto: Usa o parâmetro `insideLoop` para impedir comandos como `break` fora de laços de repetição.
 *
 * 3. Gestão de Erros:
 * - Utiliza a flag estática `ASTNode::hasSemanticError` para sinalizar falhas sem interromper
 * imediatamente a análise, permitindo reportar múltiplos erros antes de abortar a compilação.
 * * 4. Arrays e Ponteiros:
 * - `ArrayAccessNode` e `ArrayAssignNode` tratam a indexação, permitindo semanticamente
 * que variáveis escalares (int/float) sejam tratadas como arrays, conforme permitido pela gramática.
 */

class ASTNode
{
public:
  int line = 0;
  inline static bool hasSemanticError = false;
  virtual ~ASTNode() = default;
  virtual void print(int level = 0) const = 0;
  virtual std::string checkType(SymbolTable &symtab, bool insideLoop = false)
  {
    (void)symtab;
    (void)insideLoop;
    return "";
  }

  virtual std::string genCode(CodeGenerator &gen, std::string loopExit = "")
  {
    (void)gen;
    (void)loopExit;
    return "";
  }

protected:
  void printIndent(int level) const
  {
    for (int i = 0; i < level; ++i)
      std::cout << "  ";
  }
};

class ExprNode : public ASTNode
{
public:
  std::string type;
};

class IntLiteral : public ExprNode
{
public:
  int value;
  IntLiteral(int val) : value(val) { type = "int"; }
  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "IntLiteral: " << value << "\n";
  }
  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)symtab;
    (void)insideLoop;
    return "int";
  }
  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)gen;
    (void)loopExit;
    return std::to_string(value);
  }
};

class FloatLiteral : public ExprNode
{
public:
  float value;
  FloatLiteral(float val) : value(val) { type = "float"; }
  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "FloatLiteral: " << value << "\n";
  }
  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)symtab;
    (void)insideLoop;
    return "float";
  }
  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)gen;
    (void)loopExit;
    return std::to_string(value);
  }
};

class StringLiteral : public ExprNode
{
public:
  std::string value;
  StringLiteral(std::string val) : value(val) { type = "string"; }
  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "StringLiteral: " << value << "\n";
  }
  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)symtab;
    (void)insideLoop;
    return "string";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)gen;
    (void)loopExit;
    // Retorna a string entre aspas para ser usada no código intermediário
    return "\"" + value + "\"";
  }
};

class FuncCallNode : public ExprNode
{
public:
  std::string name;
  std::vector<std::unique_ptr<ASTNode>> args;

  FuncCallNode(std::string n) : name(n) {}

  void addArg(std::unique_ptr<ASTNode> arg)
  {
    args.push_back(std::move(arg));
  }

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "FuncCall: " << name << "\n";
    for (const auto &arg : args)
    {
      arg->print(level + 1);
    }
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    SymbolEntry *entry = symtab.lookup(name);
    if (!entry)
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Função '" << name << "' não declarada na linha " << line << ".\n";
      return "ERROR";
    }
    return entry->type;
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    std::vector<std::string> argAddrs;
    for (const auto &arg : args)
    {
      argAddrs.push_back(arg->genCode(gen, loopExit));
    }

    for (const auto &addr : argAddrs)
    {
      gen.emit("param " + addr);
    }

    std::string t = gen.newTemp();
    gen.emit(t, "call " + name + ", " + std::to_string(args.size()));
    return t;
  }
};

class VarAccess : public ExprNode
{
public:
  std::string name;
  VarAccess(std::string n) : name(n) {}
  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "VarAccess: " << name << "\n";
  }
  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    SymbolEntry *entry = symtab.lookup(name);
    if (!entry)
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Variável '" << name << "' não declarada na linha " << line << ".\n";
      return "ERROR";
    }
    if (entry->type.empty())
    {
      return "ERROR";
    }
    return entry->type;
  }
  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)gen;
    (void)loopExit;
    return name;
  }
};

class BinaryExpr : public ExprNode
{
public:
  std::unique_ptr<ExprNode> left;
  std::unique_ptr<ExprNode> right;
  std::string op;

  BinaryExpr(std::unique_ptr<ExprNode> l, std::string o, std::unique_ptr<ExprNode> r)
      : left(std::move(l)), right(std::move(r)), op(o) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "BinaryExpr: " << op << "\n";
    if (left)
      left->print(level + 1);
    if (right)
      right->print(level + 1);
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    std::string leftType = left->checkType(symtab, insideLoop);
    std::string rightType = right->checkType(symtab, insideLoop);

    if (leftType == "ERROR" || rightType == "ERROR")
      return "ERROR";

    if (leftType == rightType)
    {
      return leftType;
    }

    hasSemanticError = true;
    std::cerr << "Erro semântico: Tipos incompatíveis (" << leftType << " " << op << " " << rightType << ") na linha " << line << ".\n";
    return "ERROR";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    std::string t1 = left->genCode(gen, loopExit);
    std::string t2 = right->genCode(gen, loopExit);

    std::string temp = gen.newTemp();
    // t0 = t1 + t2
    gen.emit(temp, t1, op, t2);

    return temp;
  }
};

class StmtNode : public ASTNode
{
};

class BlockNode : public StmtNode
{
public:
  std::vector<std::unique_ptr<ASTNode>> statements;

  void addStatement(std::unique_ptr<ASTNode> stmt)
  {
    statements.push_back(std::move(stmt));
  }

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "{\n";
    for (const auto &stmt : statements)
    {
      stmt->print(level + 1);
    }
    printIndent(level);
    std::cout << "}\n";
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    symtab.enterScope();
    for (const auto &stmt : statements)
    {
      stmt->checkType(symtab, insideLoop);
    }
    symtab.exitScope();
    return "";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    for (const auto &stmt : statements)
    {
      if (stmt)
        stmt->genCode(gen, loopExit);
    }
    return "";
  }
};

class VarDeclNode : public StmtNode
{
public:
  std::string typeName;
  std::string varName;
  std::unique_ptr<ExprNode> initializer;

  VarDeclNode(std::string type, std::string name, std::unique_ptr<ExprNode> init = nullptr)
      : typeName(type), varName(name), initializer(std::move(init)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "VarDecl: " << typeName << " " << varName << "\n";
    if (initializer)
    {
      initializer->print(level + 1);
    }
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    SymbolEntry *entry = symtab.lookup(varName);
    if (entry)
    {
      if (entry->type.empty())
      {
        entry->type = typeName;
        if (initializer)
        {
          initializer->checkType(symtab, insideLoop);
        }
        return "void";
      }
      else
      {
        hasSemanticError = true;
        std::cerr << "Erro semântico: Variável '" << varName << "' já declarada na linha " << line << ".\n";
        return "ERROR";
      }
    }

    symtab.addOccurrence(varName, 0, 0);
    entry = symtab.lookup(varName);
    if (entry)
    {
      entry->type = typeName;
    }
    if (initializer)
    {
      initializer->checkType(symtab, insideLoop);
    }
    return "void";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    if (initializer)
    {
      std::string valAddr = initializer->genCode(gen, loopExit);
      // x = val
      gen.emit(varName, valAddr);
    }
    return varName;
  }
};

class AssignNode : public StmtNode
{
public:
  std::string varName;
  std::unique_ptr<ExprNode> value;

  AssignNode(std::string name, std::unique_ptr<ExprNode> val)
      : varName(name), value(std::move(val)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "Assign: " << varName << "\n";
    if (value)
      value->print(level + 1);
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    std::string exprType = value->checkType(symtab, insideLoop);
    SymbolEntry *entry = symtab.lookup(varName);
    if (!entry)
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Variável '" << varName << "' não declarada na linha " << line << ".\n";
      return "ERROR";
    }

    bool typesMatch = (entry->type == exprType);

    if (!typesMatch && exprType != "ERROR")
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Atribuição inválida. Variável '" << varName
                << "' é do tipo " << entry->type << " mas recebeu " << exprType << " na linha " << line << ".\n";
      return "ERROR";
    }
    return entry->type;
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    std::string valAddr = value->genCode(gen, loopExit);
    gen.emit(varName, valAddr);
    return varName;
  }
};
class IfStmt : public StmtNode
{
public:
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> thenBranch;
  std::unique_ptr<StmtNode> elseBranch;

  IfStmt(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> thenB, std::unique_ptr<StmtNode> elseB = nullptr)
      : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "IfStmt\n";
    printIndent(level + 1);
    std::cout << "Condition:\n";
    if (condition)
      condition->print(level + 2);
    printIndent(level + 1);
    std::cout << "Then:\n";
    if (thenBranch)
      thenBranch->print(level + 2);
    if (elseBranch)
    {
      printIndent(level + 1);
      std::cout << "Else:\n";
      elseBranch->print(level + 2);
    }
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    if (condition)
      condition->checkType(symtab, insideLoop);
    if (thenBranch)
      thenBranch->checkType(symtab, insideLoop);
    if (elseBranch)
      elseBranch->checkType(symtab, insideLoop);
    return "";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    std::string condAddr = condition->genCode(gen, loopExit);

    std::string labelElse = gen.newLabel();
    std::string labelEnd = gen.newLabel();

    // ifFalse cond goto L_Else
    gen.emit("ifFalse " + condAddr + " goto " + labelElse);

    if (thenBranch)
      thenBranch->genCode(gen, loopExit);
    gen.emit("goto " + labelEnd);

    gen.emitLabel(labelElse);
    if (elseBranch)
      elseBranch->genCode(gen, loopExit);

    gen.emitLabel(labelEnd);
    return "";
  }
};

class ForStmt : public StmtNode
{
public:
  std::unique_ptr<StmtNode> init;
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> update;
  std::unique_ptr<StmtNode> body;

  ForStmt(std::unique_ptr<StmtNode> i, std::unique_ptr<ExprNode> c, std::unique_ptr<StmtNode> u, std::unique_ptr<StmtNode> b)
      : init(std::move(i)), condition(std::move(c)), update(std::move(u)), body(std::move(b)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "ForStmt\n";
    printIndent(level + 1);
    std::cout << "Init:\n";
    if (init)
      init->print(level + 2);
    printIndent(level + 1);
    std::cout << "Condition:\n";
    if (condition)
      condition->print(level + 2);
    printIndent(level + 1);
    std::cout << "Update:\n";
    if (update)
      update->print(level + 2);
    printIndent(level + 1);
    std::cout << "Body:\n";
    if (body)
      body->print(level + 2);
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    symtab.enterScope(); // For loop has its own scope for init variables
    if (init)
      init->checkType(symtab, false);
    if (condition)
      condition->checkType(symtab, true);
    if (update)
      update->checkType(symtab, true);
    if (body)
      body->checkType(symtab, true); // Pass true for insideLoop
    symtab.exitScope();
    return "";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)loopExit; // O for cria um novo contexto de loop

    // Estrutura do Loop em TAC:
    // 1. Inicialização (ex: i = 0)
    // 2. Label de Início (L_start) <- Ponto de retorno
    // 3. Teste de Condição. Se Falso, pula para L_end (Break implícito)
    // 4. Corpo do Loop (passamos L_end para permitir 'break' internos)
    // 5. Atualização (ex: i = i + 1)
    // 6. Goto incondicional para L_start
    // 7. Label de Fim (L_end)

    if (init)
      init->genCode(gen, "");

    std::string labelStart = gen.newLabel();
    std::string labelEnd = gen.newLabel(); // Este é o label para break

    gen.emitLabel(labelStart);

    if (condition)
    {
      std::string condAddr = condition->genCode(gen, "");
      gen.emit("ifFalse " + condAddr + " goto " + labelEnd);
    }

    // O corpo recebe labelEnd para lidar com break
    if (body)
      body->genCode(gen, labelEnd);

    if (update)
      update->genCode(gen, "");

    gen.emit("goto " + labelStart);
    gen.emitLabel(labelEnd);

    return "";
  }
};

class WhileStmt : public StmtNode
{
public:
  std::unique_ptr<ExprNode> condition;
  std::unique_ptr<StmtNode> body;

  WhileStmt(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> b)
      : condition(std::move(cond)), body(std::move(b)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "WhileStmt\n";
    printIndent(level + 1);
    std::cout << "Condition:\n";
    if (condition)
      condition->print(level + 2);
    printIndent(level + 1);
    std::cout << "Body:\n";
    if (body)
      body->print(level + 2);
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    if (condition)
      condition->checkType(symtab, true); // Condition is inside loop context? Maybe not strictly, but body is.
    if (body)
      body->checkType(symtab, true); // Pass true for insideLoop
    return "";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)loopExit; // While cria novo contexto
    std::string labelStart = gen.newLabel();
    std::string labelEnd = gen.newLabel();

    gen.emitLabel(labelStart);

    if (condition)
    {
      std::string condAddr = condition->genCode(gen, "");
      gen.emit("ifFalse " + condAddr + " goto " + labelEnd);
    }

    if (body)
      body->genCode(gen, labelEnd);

    gen.emit("goto " + labelStart);
    gen.emitLabel(labelEnd);
    return "";
  }
};

class ReturnNode : public StmtNode
{
public:
  std::unique_ptr<ExprNode> value;
  std::string inferredType = "void";

  ReturnNode(std::unique_ptr<ExprNode> val = nullptr) : value(std::move(val)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "Return\n";
    if (value)
    {
      value->print(level + 1);
    }
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    if (value)
    {
      inferredType = value->checkType(symtab, insideLoop);
      return inferredType;
    }
    return "void";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)loopExit;
    if (value)
    {
      std::string valAddr = value->genCode(gen, "");
      gen.emit("return " + valAddr);
    }
    else
    {
      gen.emit("return");
    }
    return "";
  }
};

class PrintStmt : public StmtNode
{
public:
  std::unique_ptr<ExprNode> expression;

  PrintStmt(std::unique_ptr<ExprNode> expr) : expression(std::move(expr)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "PrintStmt\n";
    if (expression)
    {
      expression->print(level + 1);
    }
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    if (expression)
    {
      return expression->checkType(symtab, insideLoop);
    }
    return "void";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    if (expression)
    {
      std::string val = expression->genCode(gen, loopExit);
      gen.emit("print " + val);
    }
    return "";
  }
};

class ReadStmt : public StmtNode
{
public:
  std::string varName;

  ReadStmt(std::string name) : varName(name) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "ReadStmt: " << varName << "\n";
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    SymbolEntry *entry = symtab.lookup(varName);
    if (!entry)
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Variável '" << varName << "' não declarada na linha " << line << ".\n";
      return "ERROR";
    }
    return entry->type;
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)loopExit;
    gen.emit("read " + varName);
    return "";
  }
};

class BreakStmt : public StmtNode
{
public:
  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "BreakStmt\n";
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)symtab;
    if (!insideLoop)
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: 'break' fora de loop na linha " << line << "\n";
      return "ERROR";
    }
    return "void";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    if (!loopExit.empty())
    {
      gen.emit("goto " + loopExit);
    }
    else
    {
      std::cerr << "Erro GCI: Break encontrado fora de contexto de loop.\n";
    }
    return "";
  }
};

class FuncDefNode : public ASTNode
{
public:
  std::string name;
  std::vector<std::unique_ptr<VarDeclNode>> parameters;
  std::unique_ptr<BlockNode> body;

  FuncDefNode(std::string n, std::unique_ptr<BlockNode> b)
      : name(n), body(std::move(b)) {}

  void addParameter(std::unique_ptr<VarDeclNode> param)
  {
    parameters.push_back(std::move(param));
  }

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "FuncDef: " << name << "\n";
    printIndent(level + 1);
    std::cout << "Params:\n";
    for (const auto &param : parameters)
    {
      param->print(level + 2);
    }
    if (body)
    {
      body->print(level + 1);
    }
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    // 1. Add function to current scope (Global/Parent)
    std::string returnType = "int"; // Default assumption
    symtab.addOccurrence(name, 0, 0);
    SymbolEntry *entry = symtab.lookup(name);
    if (entry)
      entry->type = returnType;

    symtab.enterScope();
    for (const auto &param : parameters)
    {
      param->checkType(symtab, false); // Functions are not loops
    }
    if (body)
    {
      body->checkType(symtab, false); // Functions are not loops

      // Infer return type from ReturnNode (top-level only for now)
      for (const auto &stmt : body->statements)
      {
        if (auto retNode = dynamic_cast<ReturnNode *>(stmt.get()))
        {
          if (retNode->inferredType != "ERROR" && retNode->inferredType != "void")
          {
            returnType = retNode->inferredType;
          }
        }
      }
    }
    symtab.exitScope();

    // Update function type in symbol table
    if (entry)
      entry->type = returnType;

    return "";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)loopExit;
    gen.emitLabel(name);

    if (body)
      body->genCode(gen, "");

    return "";
  }
};

class ProgramNode : public ASTNode
{
public:
  std::vector<std::unique_ptr<ASTNode>> globals;

  void addGlobal(std::unique_ptr<ASTNode> node)
  {
    globals.push_back(std::move(node));
  }

  void print(int level = 0) const override
  {
    std::cout << "ProgramNode\n";
    for (const auto &node : globals)
    {
      node->print(level + 1);
    }
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    (void)insideLoop;
    // Do not create a new scope for ProgramNode, so globals are in Scope 0
    for (const auto &node : globals)
    {
      node->checkType(symtab, false);
    }
    return "";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    (void)loopExit;
    for (const auto &node : globals)
    {
      if (node)
        node->genCode(gen, "");
    }
    return "";
  }
};

class ArrayAccessNode : public ExprNode
{
public:
  std::string name;
  std::unique_ptr<ExprNode> index;

  ArrayAccessNode(std::string n, std::unique_ptr<ExprNode> idx)
      : name(n), index(std::move(idx)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "ArrayAccess: " << name << "\n";
    if (index)
      index->print(level + 1);
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    std::string indexType = index->checkType(symtab, insideLoop);
    if (indexType != "int")
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Índice de array deve ser inteiro na linha " << line << ".\n";
      return "ERROR";
    }

    SymbolEntry *entry = symtab.lookup(name);
    if (!entry)
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Array '" << name << "' não declarado na linha " << line << ".\n";
      return "ERROR";
    }
    // Permitimos int, float, string serem indexados (como ponteiros)
    return entry->type;
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    std::string idxAddr = index->genCode(gen, loopExit);
    std::string temp = gen.newTemp();
    // Emite: t0 = arr[i]
    gen.emit(temp, name + "[" + idxAddr + "]");
    return temp;
  }
};

class ArrayAssignNode : public StmtNode
{
public:
  std::string name;
  std::unique_ptr<ExprNode> index;
  std::unique_ptr<ExprNode> value;

  ArrayAssignNode(std::string n, std::unique_ptr<ExprNode> idx, std::unique_ptr<ExprNode> val)
      : name(n), index(std::move(idx)), value(std::move(val)) {}

  void print(int level = 0) const override
  {
    printIndent(level);
    std::cout << "ArrayAssign: " << name << "\n";
    printIndent(level + 1);
    std::cout << "Index:\n";
    if (index)
      index->print(level + 2);
    printIndent(level + 1);
    std::cout << "Value:\n";
    if (value)
      value->print(level + 2);
  }

  std::string checkType(SymbolTable &symtab, bool insideLoop = false) override
  {
    std::string indexType = index->checkType(symtab, insideLoop);
    if (indexType != "int")
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Índice de array deve ser inteiro na linha " << line << ".\n";
      return "ERROR";
    }

    SymbolEntry *entry = symtab.lookup(name);
    if (!entry)
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Array '" << name << "' não declarado na linha " << line << ".\n";
      return "ERROR";
    }

    std::string valType = value->checkType(symtab, insideLoop);

    // Verifica compatibilidade (ex: array int recebe valor int)
    bool typesMatch = (entry->type == valType);

    if (!typesMatch && valType != "ERROR")
    {
      hasSemanticError = true;
      std::cerr << "Erro semântico: Atribuição inválida no array na linha " << line << ".\n";
      return "ERROR";
    }

    return "";
  }

  std::string genCode(CodeGenerator &gen, std::string loopExit = "") override
  {
    std::string idxAddr = index->genCode(gen, loopExit);
    std::string valAddr = value->genCode(gen, loopExit);
    // arr[i] = val
    gen.emit(name + "[" + idxAddr + "]", valAddr);
    return "";
  }
};

#endif
