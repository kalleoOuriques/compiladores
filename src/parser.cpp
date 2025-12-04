#include "parser.hpp"
#include "utils.hpp"
#include "code_generator.hpp"
#include <iostream>
#include <algorithm>

Parser::Parser(Lexer &lex) : lexer(lex), grammar()
{
    advance();
}

void Parser::advance()
{
    previous = current;
    current = lexer.nextToken();
    // Detectar erro léxico imediatamente
    if (current.type == TokenType::ERROR)
    {
        std::cerr << "Erro léxico: " << current.lexeme
                  << " na linha " << current.line
                  << " coluna " << current.column << "\n";
        exit(1);
    }
}

void Parser::parse()
{
    std::stack<std::string> st;
    st.push("PROGRAM");

    while (!st.empty())
    {
        std::string top = st.top();
        st.pop();

        // Ignorar produções vazias (ε)
        if (top == "")
            continue;

        // --- Processamento de Ações Semânticas (SDT) ---
        // Se o símbolo no topo da pilha começar com '#', ele é um Marcador de Ação.
        // Esses marcadores não consomem tokens da entrada. Eles apenas disparam
        // a função `performAction` para manipular a pilha semântica (semanticStack)
        // e construir os nós da AST correspondentes à regra gramatical recém-processada.
        if (top.length() > 0 && top[0] == '#')
        {
            performAction(top);
            continue;
        }

        // Verificar se é um terminal
        if (isTerminal(top))
        {
            // Comparar terminal esperado com token atual
            if (matchTerminal(top, current.type))
            {
                if (current.type == TokenType::IDENT)
                {
                    auto node = new VarAccess(current.lexeme);
                    node->line = current.line;
                    semanticStack.push(node);
                }
                advance();
                continue;
            }

            // Erro: terminal esperado não corresponde ao token atual
            std::cerr << "Erro sintático: esperado '" << top
                      << "' mas encontrado '" << current.lexeme
                      << "' na linha " << current.line
                      << ", coluna " << current.column << "\n";
            exit(1);
        }

        // É um não-terminal: consultar tabela LL(1)
        std::string tokName = tokenTypeToString(current.type);

        auto key = std::make_pair(top, tokName);

        if (grammar.ll1table.count(key) == 0)
        {
            std::cerr << "Erro sintático: não há produção para (" << top
                      << ", " << tokName << ")\n";
            std::cerr << "Token inesperado '" << current.lexeme
                      << "' na linha " << current.line
                      << ", coluna " << current.column << "\n";
            exit(1);
        }

        // Empilhar produção na ordem reversa
        auto production = grammar.ll1table[key];
        for (int i = (int)production.size() - 1; i >= 0; --i)
        {
            st.push(production[i]);
        }
    }

    std::cout << "Programa sintaticamente correto!\n";
    if (!semanticStack.empty())
    {
        // O topo da pilha deve ser a raiz da AST completa
        root.reset(semanticStack.top());
        semanticStack.pop();

        if (!semanticStack.empty())
        {
            std::cerr << "Aviso: Árvore incompleta/fragmentada. Sobraram " << semanticStack.size() << " nós na pilha.\n";
        }

        std::cout << "Árvore AST gerada (raiz):\n";
        root->print();

        std::cout << "\nIniciando geração de código intermediário...\n";

        root->genCode(gen);
        gen.printCode();
    }
}

/**
 * @brief Executa uma ação semântica baseada em um marcador da gramática.
 * * Esta função é chamada pelo Parser sempre que um token de ação (iniciado por '#')
 * é encontrado no topo da pilha de análise sintática. Ela é responsável por:
 * 1. Interpretar o comando (ex: "#BUILD_ASSIGN", "#BUILD_IF").
 * 2. Desempilhar os nós necessários da pilha semântica (`semanticStack`).
 * 3. Construir o nó da AST correspondente com os dados coletados.
 * 4. Empilhar o novo nó resultante de volta na pilha semântica.
 * * @param action O identificador da ação semântica (ex: "#BUILD_VARDECL").
 */
void Parser::performAction(const std::string &action)
{
    // Esta função implementa a SDD.
    // Características L-Atribuídas:
    // 1. Atributos Sintetizados: Construídos de baixo para cima usando a `semanticStack`
    //    (ex: #BUILD_ADD desempilha filhos para criar o pai).
    // 2. Atributos Herdados: Informações fluem da esquerda para a direita via variáveis
    //    auxiliares como `lastType` e `tempParams`.
    //    Exemplo: Em `int x;`, o tipo 'int' é lido (#BUILD_TYPE), salvo em `lastType`
    //    e herdado pela ação posterior (#BUILD_VARDECL) para criar o nó.

    if (action == "#BUILD_INT")
    {
        int val = std::stoi(previous.lexeme);
        auto node = new IntLiteral(val);
        node->line = previous.line;
        semanticStack.push(node);
    }
    else if (action == "#BUILD_FLOAT")
    {
        float val = std::stof(previous.lexeme);
        auto node = new FloatLiteral(val);
        node->line = previous.line;
        semanticStack.push(node);
    }
    else if (action == "#BUILD_STRING")
    {
        std::string s = previous.lexeme;
        if (s.length() >= 2 && s.front() == '"' && s.back() == '"')
        {
            s = s.substr(1, s.length() - 2);
        }
        auto node = new StringLiteral(s);
        node->line = previous.line;
        semanticStack.push(node);
    }
    else if (action == "#BUILD_VAR")
    {
        // semanticStack.push(new VarAccess(previous.lexeme));
        // Agora o IDENT é empilhado automaticamente no parse()
    }
    else if (action == "#BUILD_ADD" || action == "#BUILD_SUB" ||
             action == "#BUILD_MUL" || action == "#BUILD_DIV" ||
             action == "#BUILD_MOD" || action == "#BUILD_LT" ||
             action == "#BUILD_GT" || action == "#BUILD_LE" ||
             action == "#BUILD_GE" || action == "#BUILD_EQ" ||
             action == "#BUILD_NEQ")
    {

        if (semanticStack.size() < 2)
        {
            std::cerr << "Erro semântico: operandos insuficientes para " << action << "\n";
            exit(1);
        }

        ASTNode *rightNode = semanticStack.top();
        semanticStack.pop();
        ASTNode *leftNode = semanticStack.top();
        semanticStack.pop();

        std::string op;
        if (action == "#BUILD_ADD")
            op = "+";
        else if (action == "#BUILD_SUB")
            op = "-";
        else if (action == "#BUILD_MUL")
            op = "*";
        else if (action == "#BUILD_DIV")
            op = "/";
        else if (action == "#BUILD_MOD")
            op = "%";
        else if (action == "#BUILD_LT")
            op = "<";
        else if (action == "#BUILD_GT")
            op = ">";
        else if (action == "#BUILD_LE")
            op = "<=";
        else if (action == "#BUILD_GE")
            op = ">=";
        else if (action == "#BUILD_EQ")
            op = "==";
        else if (action == "#BUILD_NEQ")
            op = "!=";

        ExprNode *leftExpr = dynamic_cast<ExprNode *>(leftNode);
        ExprNode *rightExpr = dynamic_cast<ExprNode *>(rightNode);

        if (!leftExpr || !rightExpr)
        {
            std::cerr << "Erro semântico: operandos inválidos para operação binária\n";
            exit(1);
        }

        auto binExpr = new BinaryExpr(
            std::unique_ptr<ExprNode>(leftExpr),
            op,
            std::unique_ptr<ExprNode>(rightExpr));
        binExpr->line = leftExpr->line;
        semanticStack.push(binExpr);
    }
    else if (action == "#MARK_BLOCK" || action == "#MARK_PROG")
    {
        semanticStack.push(nullptr);
    }
    else if (action == "#BUILD_BLOCK")
    {
        auto block = std::make_unique<BlockNode>();
        std::vector<std::unique_ptr<ASTNode>> stmts;

        while (!semanticStack.empty() && semanticStack.top() != nullptr)
        {
            stmts.push_back(std::unique_ptr<ASTNode>(semanticStack.top()));
            semanticStack.pop();
        }

        if (!semanticStack.empty())
        {
            semanticStack.pop(); // Remove nullptr marker
        }

        std::reverse(stmts.begin(), stmts.end());
        if (!stmts.empty())
        {
            block->line = stmts[0]->line;
        }
        else
        {
            block->line = previous.line;
        }
        for (auto &stmt : stmts)
        {
            block->addStatement(std::move(stmt));
        }

        semanticStack.push(block.release());
    }
    else if (action == "#BUILD_PROG")
    {
        auto prog = std::make_unique<ProgramNode>();
        std::vector<std::unique_ptr<ASTNode>> globals;

        while (!semanticStack.empty() && semanticStack.top() != nullptr)
        {
            globals.push_back(std::unique_ptr<ASTNode>(semanticStack.top()));
            semanticStack.pop();
        }

        if (!semanticStack.empty())
        {
            semanticStack.pop(); // Remove nullptr marker
        }

        std::reverse(globals.begin(), globals.end());
        if (!globals.empty())
        {
            prog->line = globals[0]->line;
        }
        for (auto &node : globals)
        {
            prog->addGlobal(std::move(node));
        }

        semanticStack.push(prog.release());
    }
    else if (action == "#BUILD_RETURN")
    {
        ASTNode *expr = nullptr;
        if (!semanticStack.empty())
        {
            ExprNode *possibleExpr = dynamic_cast<ExprNode *>(semanticStack.top());
            if (possibleExpr)
            {
                semanticStack.pop();
                expr = possibleExpr;
            }
        }
        auto retNode = new ReturnNode(std::unique_ptr<ExprNode>(dynamic_cast<ExprNode *>(expr)));
        retNode->line = previous.line;
        semanticStack.push(retNode);
    }
    else if (action == "#BUILD_PRINT")
    {
        if (semanticStack.empty())
        {
            std::cerr << "Erro semântico: Expressão para print não encontrada\n";
            exit(1);
        }
        ExprNode *expr = dynamic_cast<ExprNode *>(semanticStack.top());
        if (!expr)
        {
            std::cerr << "Erro semântico: Operando inválido para print\n";
            exit(1);
        }
        semanticStack.pop();
        auto printNode = new PrintStmt(std::unique_ptr<ExprNode>(expr));
        printNode->line = previous.line;
        semanticStack.push(printNode);
    }
    else if (action == "#BUILD_BREAK")
    {
        auto breakNode = new BreakStmt();
        breakNode->line = previous.line;
        semanticStack.push(breakNode);
    }
    else if (action == "#MARK_FOR_INIT" || action == "#MARK_FOR_UPDATE")
    {
        semanticStack.push(nullptr);
    }
    else if (action == "#BUILD_FOR_INIT" || action == "#BUILD_FOR_UPDATE")
    {
        ASTNode *node = nullptr;
        if (!semanticStack.empty() && semanticStack.top() != nullptr)
        {
            node = semanticStack.top();
            semanticStack.pop();
        }
        if (!semanticStack.empty())
            semanticStack.pop();

        semanticStack.push(node);
    }
    else if (action == "#BUILD_FOR")
    {
        if (semanticStack.size() < 4)
        {
            std::cerr << "Erro semântico: Pilha insuficiente para #BUILD_FOR\n";
            exit(1);
        }

        ASTNode *bodyNode = semanticStack.top();
        semanticStack.pop();
        ASTNode *updateNode = semanticStack.top();
        semanticStack.pop();
        ASTNode *condNode = semanticStack.top();
        semanticStack.pop();
        ASTNode *initNode = semanticStack.top();
        semanticStack.pop();

        BlockNode *block = dynamic_cast<BlockNode *>(bodyNode);
        StmtNode *update = dynamic_cast<StmtNode *>(updateNode);
        ExprNode *cond = dynamic_cast<ExprNode *>(condNode);
        StmtNode *init = dynamic_cast<StmtNode *>(initNode);

        if (!block)
        {
            std::cerr << "Erro semântico: Corpo do for inválido\n";
            exit(1);
        }

        auto forNode = new ForStmt(
            std::unique_ptr<StmtNode>(init),
            std::unique_ptr<ExprNode>(cond),
            std::unique_ptr<StmtNode>(update),
            std::unique_ptr<StmtNode>(block));
        if (init)
            forNode->line = init->line;
        else if (block)
            forNode->line = block->line;
        semanticStack.push(forNode);
    }
    else if (action == "#MARK_ARGS")
    {
        semanticStack.push(nullptr);
    }
    else if (action == "#BUILD_CALL")
    {
        std::vector<ASTNode *> args;
        while (!semanticStack.empty() && semanticStack.top() != nullptr)
        {
            args.push_back(semanticStack.top());
            semanticStack.pop();
        }
        if (!semanticStack.empty())
            semanticStack.pop(); // Remove nullptr marker

        if (semanticStack.empty())
        {
            std::cerr << "Erro semântico: Nome da função não encontrado na pilha para #BUILD_CALL\n";
            exit(1);
        }

        VarAccess *funcNameNode = dynamic_cast<VarAccess *>(semanticStack.top());
        if (!funcNameNode)
        {
            std::cerr << "Erro semântico: Esperado identificador de função, encontrado outro nó.\n";
            exit(1);
        }
        semanticStack.pop(); // Pop the VarAccess

        auto callNode = std::make_unique<FuncCallNode>(funcNameNode->name);
        callNode->line = funcNameNode->line;
        delete funcNameNode; // We only needed the name

        std::reverse(args.begin(), args.end());
        for (auto *arg : args)
        {
            callNode->addArg(std::unique_ptr<ASTNode>(arg));
        }
        semanticStack.push(callNode.release());
    }
    else if (action == "#MARK_PARAMS")
    {
        semanticStack.push(nullptr);
        tempParams.clear();
    }
    else if (action == "#BUILD_PARAM")
    {
        if (semanticStack.empty())
        {
            std::cerr << "Erro semântico: Identificador do parâmetro não encontrado\n";
            exit(1);
        }
        VarAccess *varNode = dynamic_cast<VarAccess *>(semanticStack.top());
        if (!varNode)
        {
            std::cerr << "Erro semântico: Esperado identificador para parâmetro\n";
            exit(1);
        }
        std::string paramName = varNode->name;
        int paramLine = varNode->line;
        semanticStack.pop();
        delete varNode;

        auto param = new VarDeclNode(lastType, paramName);
        param->line = paramLine;
        tempParams.push_back(param);
    }
    else if (action == "#BUILD_FUNC")
    {
        if (semanticStack.size() < 2)
        {
            std::cerr << "Erro semântico: Pilha insuficiente para #BUILD_FUNC\n";
            exit(1);
        }

        ASTNode *bodyNode = semanticStack.top();
        semanticStack.pop();

        BlockNode *block = dynamic_cast<BlockNode *>(bodyNode);
        if (!block)
        {
            std::cerr << "Erro semântico: Corpo da função inválido\n";
            exit(1);
        }

        // Remove #MARK_PARAMS
        if (!semanticStack.empty() && semanticStack.top() == nullptr)
        {
            semanticStack.pop();
        }
        else
        {
            // Limpeza de segurança: remove nós residuais da pilha até encontrar o marcador de parâmetros (nullptr), garantindo a consistência do estado.
            if (!semanticStack.empty())
            {
                while (!semanticStack.empty() && semanticStack.top() != nullptr)
                {
                    // Clean up unexpected garbage
                    delete semanticStack.top();
                    semanticStack.pop();
                }
                if (!semanticStack.empty())
                    semanticStack.pop(); // Pop nullptr
            }
        }

        if (semanticStack.empty())
        {
            std::cerr << "Erro semântico: Nome da função não encontrado\n";
            exit(1);
        }

        VarAccess *funcNameNode = dynamic_cast<VarAccess *>(semanticStack.top());
        if (!funcNameNode)
        {
            std::cerr << "Erro semântico: Esperado nome da função\n";
            exit(1);
        }
        std::string funcName = funcNameNode->name;
        int funcLine = funcNameNode->line;
        semanticStack.pop();
        delete funcNameNode;

        auto func = std::make_unique<FuncDefNode>(funcName, std::unique_ptr<BlockNode>(block));
        func->line = funcLine;
        for (auto *param : tempParams)
        {
            func->addParameter(std::unique_ptr<VarDeclNode>(param));
        }
        tempParams.clear();
        semanticStack.push(func.release());
    }
    else if (action == "#BUILD_TYPE")
    {
        lastType = previous.lexeme;
    }
    else if (action == "#MARK_DECL")
    {
        semanticStack.push(nullptr);
    }
    else if (action == "#BUILD_VARDECL")
    {
        std::vector<ASTNode *> nodes;
        while (!semanticStack.empty() && semanticStack.top() != nullptr)
        {
            nodes.push_back(semanticStack.top());
            semanticStack.pop();
        }
        if (!semanticStack.empty())
            semanticStack.pop(); // Remove nullptr marker

        if (nodes.size() == 2)
        {
            // Init, Name
            ExprNode *initExpr = dynamic_cast<ExprNode *>(nodes[0]);
            VarAccess *nameNode = dynamic_cast<VarAccess *>(nodes[1]);

            if (initExpr && nameNode)
            {
                auto decl = new VarDeclNode(lastType, nameNode->name, std::unique_ptr<ExprNode>(initExpr));
                decl->line = nameNode->line;
                semanticStack.push(decl);
            }
            delete nameNode;
        }
        else if (nodes.size() == 1)
        {
            VarAccess *nameNode = dynamic_cast<VarAccess *>(nodes[0]);

            if (nameNode)
            {
                auto decl = new VarDeclNode(lastType, nameNode->name);
                decl->line = nameNode->line;
                semanticStack.push(decl);
            }
            delete nameNode;
        }
    }
    else if (action == "#BUILD_ASSIGN")
    {
        if (semanticStack.size() < 2)
        {
            std::cerr << "Erro semântico: operandos insuficientes para #BUILD_ASSIGN\n";
            exit(1);
        }
        ASTNode *valNode = semanticStack.top();
        semanticStack.pop();
        ASTNode *varNode = semanticStack.top();
        semanticStack.pop();

        ExprNode *valExpr = dynamic_cast<ExprNode *>(valNode);
        VarAccess *varAccess = dynamic_cast<VarAccess *>(varNode);

        if (valExpr && varAccess)
        {
            auto assign = new AssignNode(varAccess->name, std::unique_ptr<ExprNode>(valExpr));
            assign->line = varAccess->line;
            semanticStack.push(assign);
        }
        delete varAccess;
    }
    else if (action == "#BUILD_NEG")
    {
        if (semanticStack.empty())
        {
            std::cerr << "Erro semântico: operando insuficiente para #BUILD_NEG\n";
            exit(1);
        }
        ASTNode *node = semanticStack.top();
        semanticStack.pop();
        ExprNode *expr = dynamic_cast<ExprNode *>(node);
        if (!expr)
        {
            std::cerr << "Erro semântico: operando inválido para #BUILD_NEG\n";
            exit(1);
        }
        auto negExpr = new BinaryExpr(
            std::make_unique<IntLiteral>(0),
            "-",
            std::unique_ptr<ExprNode>(expr));
        negExpr->line = expr->line;
        semanticStack.push(negExpr);
    }
    else if (action == "#BUILD_ARRAY_ACCESS")
    {
        if (semanticStack.size() < 2)
        {
            std::cerr << "Erro semântico: Pilha insuficiente para #BUILD_ARRAY_ACCESS\n";
            exit(1);
        }
        ExprNode *indexExpr = dynamic_cast<ExprNode *>(semanticStack.top());
        semanticStack.pop();
        VarAccess *varNode = dynamic_cast<VarAccess *>(semanticStack.top());
        semanticStack.pop();

        if (!indexExpr || !varNode)
        {
            std::cerr << "Erro semântico: Operandos inválidos para acesso a array\n";
            exit(1);
        }

        std::string name = varNode->name;
        int varLine = varNode->line;
        delete varNode; // We only need the name
        auto arrayAccess = new ArrayAccessNode(name, std::unique_ptr<ExprNode>(indexExpr));
        arrayAccess->line = varLine;
        semanticStack.push(arrayAccess);
    }
    else if (action == "#BUILD_ARRAY_ASSIGN")
    {
        if (semanticStack.size() < 3)
        {
            std::cerr << "Erro semântico: Pilha insuficiente para #BUILD_ARRAY_ASSIGN\n";
            exit(1);
        }
        ExprNode *valExpr = dynamic_cast<ExprNode *>(semanticStack.top());
        semanticStack.pop();
        ExprNode *indexExpr = dynamic_cast<ExprNode *>(semanticStack.top());
        semanticStack.pop();
        VarAccess *varNode = dynamic_cast<VarAccess *>(semanticStack.top());
        semanticStack.pop();

        if (!valExpr || !indexExpr || !varNode)
        {
            std::cerr << "Erro semântico: Operandos inválidos para atribuição de array\n";
            exit(1);
        }

        std::string name = varNode->name;
        int varLine = varNode->line;
        delete varNode;
        auto arrayAssign = new ArrayAssignNode(name, std::unique_ptr<ExprNode>(indexExpr), std::unique_ptr<ExprNode>(valExpr));
        arrayAssign->line = varLine;
        semanticStack.push(arrayAssign);
    }
}

bool Parser::isTerminal(const std::string &symbol)
{
    // Todos os terminais são tokens ou símbolos especiais
    if (symbol == "IDENT" || symbol == "INT_CONST" || symbol == "FLOAT_CONST" ||
        symbol == "STRING_CONST" || symbol == "END_OF_FILE")
    {
        return true;
    }

    // Palavras-chave
    if (symbol.rfind("KW_", 0) == 0)
    {
        return true;
    }

    // Operadores e símbolos
    if (symbol == "PLUS" || symbol == "MINUS" || symbol == "STAR" ||
        symbol == "SLASH" || symbol == "MOD" ||
        symbol == "LT" || symbol == "GT" || symbol == "LE" ||
        symbol == "GE" || symbol == "EQ" || symbol == "NEQ" ||
        symbol == "ASSIGN" || symbol == "COMMA" || symbol == "SEMICOLON" ||
        symbol == "LPAREN" || symbol == "RPAREN" ||
        symbol == "LBRACE" || symbol == "RBRACE" ||
        symbol == "LBRACKET" || symbol == "RBRACKET")
    {
        return true;
    }

    return false;
}

bool Parser::matchTerminal(const std::string &terminal, TokenType type)
{
    if (terminal == "IDENT")
        return type == TokenType::IDENT;
    if (terminal == "INT_CONST")
        return type == TokenType::INT_CONST;
    if (terminal == "FLOAT_CONST")
        return type == TokenType::FLOAT_CONST;
    if (terminal == "STRING_CONST")
        return type == TokenType::STRING_CONST;
    if (terminal == "END_OF_FILE")
        return type == TokenType::END_OF_FILE;

    // Palavras-chave
    if (terminal == "KW_INT")
        return type == TokenType::KW_INT;
    if (terminal == "KW_FLOAT")
        return type == TokenType::KW_FLOAT;
    if (terminal == "KW_STRING")
        return type == TokenType::KW_STRING;
    if (terminal == "KW_DEF")
        return type == TokenType::KW_DEF;
    if (terminal == "KW_FOR")
        return type == TokenType::KW_FOR;
    if (terminal == "KW_IF")
        return type == TokenType::KW_IF;
    if (terminal == "KW_ELSE")
        return type == TokenType::KW_ELSE;
    if (terminal == "KW_PRINT")
        return type == TokenType::KW_PRINT;
    if (terminal == "KW_READ")
        return type == TokenType::KW_READ;
    if (terminal == "KW_RETURN")
        return type == TokenType::KW_RETURN;
    if (terminal == "KW_BREAK")
        return type == TokenType::KW_BREAK;
    if (terminal == "KW_NEW")
        return type == TokenType::KW_NEW;
    if (terminal == "KW_NULL")
        return type == TokenType::KW_NULL;

    // Operadores e símbolos
    if (terminal == "PLUS")
        return type == TokenType::PLUS;
    if (terminal == "MINUS")
        return type == TokenType::MINUS;
    if (terminal == "STAR")
        return type == TokenType::STAR;
    if (terminal == "SLASH")
        return type == TokenType::SLASH;
    if (terminal == "MOD")
        return type == TokenType::MOD;
    if (terminal == "LT")
        return type == TokenType::LT;
    if (terminal == "GT")
        return type == TokenType::GT;
    if (terminal == "LE")
        return type == TokenType::LE;
    if (terminal == "GE")
        return type == TokenType::GE;
    if (terminal == "EQ")
        return type == TokenType::EQ;
    if (terminal == "NEQ")
        return type == TokenType::NEQ;
    if (terminal == "ASSIGN")
        return type == TokenType::ASSIGN;
    if (terminal == "COMMA")
        return type == TokenType::COMMA;
    if (terminal == "SEMICOLON")
        return type == TokenType::SEMICOLON;
    if (terminal == "LPAREN")
        return type == TokenType::LPAREN;
    if (terminal == "RPAREN")
        return type == TokenType::RPAREN;
    if (terminal == "LBRACE")
        return type == TokenType::LBRACE;
    if (terminal == "RBRACE")
        return type == TokenType::RBRACE;
    if (terminal == "LBRACKET")
        return type == TokenType::LBRACKET;
    if (terminal == "RBRACKET")
        return type == TokenType::RBRACKET;

    return false;
}
