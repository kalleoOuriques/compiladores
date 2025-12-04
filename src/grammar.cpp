#include "grammar.hpp"

/*
 * GRAMÁTICA LL(1) COMPLETA PARA ConvCC-2025-2
 *
 * A gramática foi transformada para LL(1) removendo:
 * - Recursão à esquerda nas expressões
 * - Ambiguidades
 * - Fatoração à esquerda onde necessário
 *
 * Estrutura:
 * PROGRAM -> DECL_LIST
 *
 * DECL_LIST -> DECL DECL_LIST | ε
 *
 * DECL -> TYPE_SPEC IDENT DECL_TAIL
 *       | KW_DEF IDENT LPAREN PARAM_LIST RPAREN BLOCK
 *       | STMT
 *
 * DECL_TAIL -> SEMICOLON           // declaração simples: int x;
 *            | ASSIGN EXPR SEMICOLON  // declaração com init: int x = 5;
 *            | LBRACKET EXPR RBRACKET SEMICOLON  // array: int arr[10];
 *
 * TYPE_SPEC -> KW_INT | KW_FLOAT | KW_STRING
 *
 * STMT -> KW_IF LPAREN EXPR RPAREN BLOCK ELSE_PART
 *       | KW_FOR LPAREN FOR_INIT SEMICOLON EXPR SEMICOLON FOR_UPDATE RPAREN BLOCK
 *       | KW_RETURN RETURN_EXPR SEMICOLON
 *       | KW_BREAK SEMICOLON
 *       | KW_PRINT LPAREN EXPR RPAREN SEMICOLON
 *       | KW_READ LPAREN IDENT RPAREN SEMICOLON
 *       | IDENT ASSIGN_OR_CALL
 *       | BLOCK
 *
 * ASSIGN_OR_CALL -> LBRACKET EXPR RBRACKET ASSIGN EXPR SEMICOLON  // arr[i] = expr;
 *                 | ASSIGN EXPR SEMICOLON                         // x = expr;
 *                 | LPAREN ARG_LIST RPAREN SEMICOLON             // func();
 *
 * ELSE_PART -> KW_ELSE BLOCK | ε
 *
 * FOR_INIT -> TYPE_SPEC IDENT ASSIGN EXPR | IDENT ASSIGN EXPR | ε
 * FOR_UPDATE -> IDENT ASSIGN EXPR | ε
 *
 * RETURN_EXPR -> EXPR | ε
 *
 * BLOCK -> LBRACE STMT_LIST RBRACE
 * STMT_LIST -> STMT STMT_LIST | ε
 *
 * EXPR -> REL_EXPR
 * REL_EXPR -> ADD_EXPR REL_EXPR'
 * REL_EXPR' -> REL_OP ADD_EXPR REL_EXPR' | ε
 * REL_OP -> LT | GT | LE | GE | EQ | NEQ
 *
 * ADD_EXPR -> MULT_EXPR ADD_EXPR'
 * ADD_EXPR' -> ADD_OP MULT_EXPR ADD_EXPR' | ε
 * ADD_OP -> PLUS | MINUS
 *
 * MULT_EXPR -> UNARY_EXPR MULT_EXPR'
 * MULT_EXPR' -> MULT_OP UNARY_EXPR MULT_EXPR' | ε
 * MULT_OP -> STAR | SLASH | MOD
 *
 * UNARY_EXPR -> MINUS UNARY_EXPR | PRIMARY
 *
 * PRIMARY -> INT_CONST
 *          | FLOAT_CONST
 *          | STRING_CONST
 *          | KW_NULL
 *          | IDENT PRIMARY_TAIL
 *          | KW_NEW TYPE_SPEC LBRACKET EXPR RBRACKET
 *          | LPAREN EXPR RPAREN
 *
 * PRIMARY_TAIL -> LBRACKET EXPR RBRACKET  // array access
 *               | LPAREN ARG_LIST RPAREN  // function call
 *               | ε                       // just identifier
 *
 * PARAM_LIST -> TYPE_SPEC IDENT PARAM_LIST' | ε
 * PARAM_LIST' -> COMMA TYPE_SPEC IDENT PARAM_LIST' | ε
 *
 * ARG_LIST -> EXPR ARG_LIST' | ε
 * ARG_LIST' -> COMMA EXPR ARG_LIST' | ε
 */

Grammar::Grammar()
{
    buildLL1Table();
}

void Grammar::buildLL1Table()
{
    // Além dos terminais e não-terminais, inserimos "Marcadores Semânticos" (ex: #BUILD_ADD, #MARK_DECL) nas regras de produção.

    // ======== PROGRAM ========
    // PROGRAM -> DECL_LIST
    ll1table[{"PROGRAM", "KW_INT"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_FLOAT"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_STRING"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_DEF"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_IF"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_FOR"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_RETURN"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_BREAK"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_PRINT"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "KW_READ"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "IDENT"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "LBRACE"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};
    ll1table[{"PROGRAM", "END_OF_FILE"}] = {"#MARK_PROG", "DECL_LIST", "#BUILD_PROG"};

    // ======== DECL_LIST ========
    // DECL_LIST -> DECL DECL_LIST
    ll1table[{"DECL_LIST", "KW_INT"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_FLOAT"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_STRING"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_DEF"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_IF"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_FOR"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_RETURN"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_BREAK"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_PRINT"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "KW_READ"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "IDENT"}] = {"DECL", "DECL_LIST"};
    ll1table[{"DECL_LIST", "LBRACE"}] = {"DECL", "DECL_LIST"};
    // DECL_LIST -> ε
    ll1table[{"DECL_LIST", "END_OF_FILE"}] = {};
    ll1table[{"DECL_LIST", "RBRACE"}] = {};

    // ======== DECL ========
    // DECL -> TYPE_SPEC IDENT DECL_TAIL
    ll1table[{"DECL", "KW_INT"}] = {"#MARK_DECL", "TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_VAR", "DECL_TAIL", "#BUILD_VARDECL"};
    ll1table[{"DECL", "KW_FLOAT"}] = {"#MARK_DECL", "TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_VAR", "DECL_TAIL", "#BUILD_VARDECL"};
    ll1table[{"DECL", "KW_STRING"}] = {"#MARK_DECL", "TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_VAR", "DECL_TAIL", "#BUILD_VARDECL"};
    // DECL -> KW_DEF IDENT LPAREN PARAM_LIST RPAREN BLOCK
    ll1table[{"DECL", "KW_DEF"}] = {"KW_DEF", "IDENT", "#BUILD_FUNC_ID", "LPAREN", "#MARK_PARAMS", "PARAM_LIST", "RPAREN", "BLOCK", "#BUILD_FUNC"};
    // DECL -> STMT
    ll1table[{"DECL", "KW_IF"}] = {"STMT"};
    ll1table[{"DECL", "KW_FOR"}] = {"STMT"};
    ll1table[{"DECL", "KW_RETURN"}] = {"STMT"};
    ll1table[{"DECL", "KW_BREAK"}] = {"STMT"};
    ll1table[{"DECL", "KW_PRINT"}] = {"STMT"};
    ll1table[{"DECL", "KW_READ"}] = {"STMT"};
    ll1table[{"DECL", "IDENT"}] = {"STMT"};
    ll1table[{"DECL", "LBRACE"}] = {"STMT"};

    // ======== DECL_TAIL ========
    // DECL_TAIL -> SEMICOLON
    ll1table[{"DECL_TAIL", "SEMICOLON"}] = {"SEMICOLON"};
    // DECL_TAIL -> ASSIGN EXPR SEMICOLON
    ll1table[{"DECL_TAIL", "ASSIGN"}] = {"ASSIGN", "EXPR", "SEMICOLON"};
    // DECL_TAIL -> LBRACKET EXPR RBRACKET SEMICOLON
    ll1table[{"DECL_TAIL", "LBRACKET"}] = {"LBRACKET", "EXPR", "RBRACKET", "SEMICOLON"};

    // ======== TYPE_SPEC ========
    ll1table[{"TYPE_SPEC", "KW_INT"}] = {"KW_INT"};
    ll1table[{"TYPE_SPEC", "KW_FLOAT"}] = {"KW_FLOAT"};
    ll1table[{"TYPE_SPEC", "KW_STRING"}] = {"KW_STRING"};

    // ======== STMT ========
    // STMT -> KW_IF LPAREN EXPR RPAREN BLOCK ELSE_PART
    ll1table[{"STMT", "KW_IF"}] = {"KW_IF", "LPAREN", "EXPR", "RPAREN", "BLOCK", "ELSE_PART"};
    // STMT -> KW_FOR LPAREN FOR_INIT SEMICOLON EXPR SEMICOLON FOR_UPDATE RPAREN BLOCK
    ll1table[{"STMT", "KW_FOR"}] = {"KW_FOR", "LPAREN", "#MARK_FOR_INIT", "FOR_INIT", "#BUILD_FOR_INIT", "SEMICOLON", "EXPR", "SEMICOLON", "#MARK_FOR_UPDATE", "FOR_UPDATE", "#BUILD_FOR_UPDATE", "RPAREN", "BLOCK", "#BUILD_FOR"};
    // STMT -> KW_RETURN RETURN_EXPR SEMICOLON
    ll1table[{"STMT", "KW_RETURN"}] = {"KW_RETURN", "RETURN_EXPR", "#BUILD_RETURN", "SEMICOLON"};
    // STMT -> KW_BREAK SEMICOLON
    ll1table[{"STMT", "KW_BREAK"}] = {"KW_BREAK", "#BUILD_BREAK", "SEMICOLON"};
    // STMT -> KW_PRINT LPAREN EXPR RPAREN SEMICOLON
    ll1table[{"STMT", "KW_PRINT"}] = {"KW_PRINT", "LPAREN", "EXPR", "RPAREN", "#BUILD_PRINT", "SEMICOLON"};
    // STMT -> KW_READ LPAREN IDENT RPAREN SEMICOLON
    ll1table[{"STMT", "KW_READ"}] = {"KW_READ", "LPAREN", "IDENT", "RPAREN", "SEMICOLON"};
    // STMT -> IDENT ASSIGN_OR_CALL
    ll1table[{"STMT", "IDENT"}] = {"IDENT", "#BUILD_VAR", "ASSIGN_OR_CALL"};
    // STMT -> BLOCK
    ll1table[{"STMT", "LBRACE"}] = {"BLOCK"};

    // ======== ASSIGN_OR_CALL ========
    // ASSIGN_OR_CALL -> LBRACKET EXPR RBRACKET ASSIGN EXPR SEMICOLON
    ll1table[{"ASSIGN_OR_CALL", "LBRACKET"}] = {"LBRACKET", "EXPR", "RBRACKET", "ASSIGN", "EXPR", "SEMICOLON", "#BUILD_ARRAY_ASSIGN"};
    // ASSIGN_OR_CALL -> ASSIGN EXPR SEMICOLON
    ll1table[{"ASSIGN_OR_CALL", "ASSIGN"}] = {"ASSIGN", "EXPR", "#BUILD_ASSIGN", "SEMICOLON"};
    // ASSIGN_OR_CALL -> LPAREN ARG_LIST RPAREN SEMICOLON
    ll1table[{"ASSIGN_OR_CALL", "LPAREN"}] = {"LPAREN", "ARG_LIST", "RPAREN", "SEMICOLON"};

    // ======== ELSE_PART ========
    // ELSE_PART -> KW_ELSE BLOCK
    ll1table[{"ELSE_PART", "KW_ELSE"}] = {"KW_ELSE", "BLOCK"};
    // ELSE_PART -> ε
    ll1table[{"ELSE_PART", "KW_INT"}] = {};
    ll1table[{"ELSE_PART", "KW_FLOAT"}] = {};
    ll1table[{"ELSE_PART", "KW_STRING"}] = {};
    ll1table[{"ELSE_PART", "KW_DEF"}] = {};
    ll1table[{"ELSE_PART", "KW_IF"}] = {};
    ll1table[{"ELSE_PART", "KW_FOR"}] = {};
    ll1table[{"ELSE_PART", "KW_RETURN"}] = {};
    ll1table[{"ELSE_PART", "KW_BREAK"}] = {};
    ll1table[{"ELSE_PART", "KW_PRINT"}] = {};
    ll1table[{"ELSE_PART", "KW_READ"}] = {};
    ll1table[{"ELSE_PART", "IDENT"}] = {};
    ll1table[{"ELSE_PART", "LBRACE"}] = {};
    ll1table[{"ELSE_PART", "RBRACE"}] = {};
    ll1table[{"ELSE_PART", "END_OF_FILE"}] = {};

    // ======== FOR_INIT ========
    // FOR_INIT -> TYPE_SPEC IDENT ASSIGN EXPR
    ll1table[{"FOR_INIT", "KW_INT"}] = {"#MARK_DECL", "TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_VAR", "ASSIGN", "EXPR", "#BUILD_VARDECL"};
    ll1table[{"FOR_INIT", "KW_FLOAT"}] = {"#MARK_DECL", "TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_VAR", "ASSIGN", "EXPR", "#BUILD_VARDECL"};
    ll1table[{"FOR_INIT", "KW_STRING"}] = {"#MARK_DECL", "TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_VAR", "ASSIGN", "EXPR", "#BUILD_VARDECL"};
    // FOR_INIT -> IDENT ASSIGN EXPR
    ll1table[{"FOR_INIT", "IDENT"}] = {"IDENT", "#BUILD_VAR", "ASSIGN", "EXPR", "#BUILD_ASSIGN"};
    // FOR_INIT -> ε
    ll1table[{"FOR_INIT", "SEMICOLON"}] = {};

    // ======== FOR_UPDATE ========
    // FOR_UPDATE -> IDENT ASSIGN EXPR
    ll1table[{"FOR_UPDATE", "IDENT"}] = {"IDENT", "#BUILD_VAR", "ASSIGN", "EXPR", "#BUILD_ASSIGN"};
    // FOR_UPDATE -> ε
    ll1table[{"FOR_UPDATE", "RPAREN"}] = {};

    // ======== RETURN_EXPR ========
    // RETURN_EXPR -> EXPR
    ll1table[{"RETURN_EXPR", "INT_CONST"}] = {"EXPR"};
    ll1table[{"RETURN_EXPR", "FLOAT_CONST"}] = {"EXPR"};
    ll1table[{"RETURN_EXPR", "STRING_CONST"}] = {"EXPR"};
    ll1table[{"RETURN_EXPR", "KW_NULL"}] = {"EXPR"};
    ll1table[{"RETURN_EXPR", "IDENT"}] = {"EXPR"};
    ll1table[{"RETURN_EXPR", "KW_NEW"}] = {"EXPR"};
    ll1table[{"RETURN_EXPR", "LPAREN"}] = {"EXPR"};
    ll1table[{"RETURN_EXPR", "MINUS"}] = {"EXPR"};
    // RETURN_EXPR -> ε
    ll1table[{"RETURN_EXPR", "SEMICOLON"}] = {};

    // ======== BLOCK ========
    ll1table[{"BLOCK", "LBRACE"}] = {"LBRACE", "#MARK_BLOCK", "STMT_LIST", "RBRACE", "#BUILD_BLOCK"};

    // ======== STMT_LIST ========
    // STMT_LIST -> DECL STMT_LIST (permite declarações e statements dentro de blocos)
    ll1table[{"STMT_LIST", "KW_INT"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_FLOAT"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_STRING"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_IF"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_FOR"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_RETURN"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_BREAK"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_PRINT"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "KW_READ"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "IDENT"}] = {"DECL", "STMT_LIST"};
    ll1table[{"STMT_LIST", "LBRACE"}] = {"DECL", "STMT_LIST"};
    // STMT_LIST -> ε
    ll1table[{"STMT_LIST", "RBRACE"}] = {};

    // ======== EXPR ========
    ll1table[{"EXPR", "INT_CONST"}] = {"REL_EXPR"};
    ll1table[{"EXPR", "FLOAT_CONST"}] = {"REL_EXPR"};
    ll1table[{"EXPR", "STRING_CONST"}] = {"REL_EXPR"};
    ll1table[{"EXPR", "KW_NULL"}] = {"REL_EXPR"};
    ll1table[{"EXPR", "IDENT"}] = {"REL_EXPR"};
    ll1table[{"EXPR", "KW_NEW"}] = {"REL_EXPR"};
    ll1table[{"EXPR", "LPAREN"}] = {"REL_EXPR"};
    ll1table[{"EXPR", "MINUS"}] = {"REL_EXPR"};

    // ======== REL_EXPR ========
    ll1table[{"REL_EXPR", "INT_CONST"}] = {"ADD_EXPR", "REL_EXPR'"};
    ll1table[{"REL_EXPR", "FLOAT_CONST"}] = {"ADD_EXPR", "REL_EXPR'"};
    ll1table[{"REL_EXPR", "STRING_CONST"}] = {"ADD_EXPR", "REL_EXPR'"};
    ll1table[{"REL_EXPR", "KW_NULL"}] = {"ADD_EXPR", "REL_EXPR'"};
    ll1table[{"REL_EXPR", "IDENT"}] = {"ADD_EXPR", "REL_EXPR'"};
    ll1table[{"REL_EXPR", "KW_NEW"}] = {"ADD_EXPR", "REL_EXPR'"};
    ll1table[{"REL_EXPR", "LPAREN"}] = {"ADD_EXPR", "REL_EXPR'"};
    ll1table[{"REL_EXPR", "MINUS"}] = {"ADD_EXPR", "REL_EXPR'"};

    // ======== REL_EXPR' ========
    // REL_EXPR' -> REL_OP ADD_EXPR REL_EXPR'
    ll1table[{"REL_EXPR'", "LT"}] = {"REL_OP", "ADD_EXPR", "#BUILD_LT", "REL_EXPR'"};
    ll1table[{"REL_EXPR'", "GT"}] = {"REL_OP", "ADD_EXPR", "#BUILD_GT", "REL_EXPR'"};
    ll1table[{"REL_EXPR'", "LE"}] = {"REL_OP", "ADD_EXPR", "#BUILD_LE", "REL_EXPR'"};
    ll1table[{"REL_EXPR'", "GE"}] = {"REL_OP", "ADD_EXPR", "#BUILD_GE", "REL_EXPR'"};
    ll1table[{"REL_EXPR'", "EQ"}] = {"REL_OP", "ADD_EXPR", "#BUILD_EQ", "REL_EXPR'"};
    ll1table[{"REL_EXPR'", "NEQ"}] = {"REL_OP", "ADD_EXPR", "#BUILD_NEQ", "REL_EXPR'"};
    // REL_EXPR' -> ε
    ll1table[{"REL_EXPR'", "SEMICOLON"}] = {};
    ll1table[{"REL_EXPR'", "RPAREN"}] = {};
    ll1table[{"REL_EXPR'", "RBRACKET"}] = {};
    ll1table[{"REL_EXPR'", "COMMA"}] = {};

    // ======== REL_OP ========
    ll1table[{"REL_OP", "LT"}] = {"LT"};
    ll1table[{"REL_OP", "GT"}] = {"GT"};
    ll1table[{"REL_OP", "LE"}] = {"LE"};
    ll1table[{"REL_OP", "GE"}] = {"GE"};
    ll1table[{"REL_OP", "EQ"}] = {"EQ"};
    ll1table[{"REL_OP", "NEQ"}] = {"NEQ"};

    // ======== ADD_EXPR ========
    ll1table[{"ADD_EXPR", "INT_CONST"}] = {"MULT_EXPR", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR", "FLOAT_CONST"}] = {"MULT_EXPR", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR", "STRING_CONST"}] = {"MULT_EXPR", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR", "KW_NULL"}] = {"MULT_EXPR", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR", "IDENT"}] = {"MULT_EXPR", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR", "KW_NEW"}] = {"MULT_EXPR", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR", "LPAREN"}] = {"MULT_EXPR", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR", "MINUS"}] = {"MULT_EXPR", "ADD_EXPR'"};

    // ======== ADD_EXPR' ========
    // ADD_EXPR' -> ADD_OP MULT_EXPR ADD_EXPR'
    ll1table[{"ADD_EXPR'", "PLUS"}] = {"ADD_OP", "MULT_EXPR", "#BUILD_ADD", "ADD_EXPR'"};
    ll1table[{"ADD_EXPR'", "MINUS"}] = {"ADD_OP", "MULT_EXPR", "#BUILD_SUB", "ADD_EXPR'"};
    // ADD_EXPR' -> ε
    ll1table[{"ADD_EXPR'", "LT"}] = {};
    ll1table[{"ADD_EXPR'", "GT"}] = {};
    ll1table[{"ADD_EXPR'", "LE"}] = {};
    ll1table[{"ADD_EXPR'", "GE"}] = {};
    ll1table[{"ADD_EXPR'", "EQ"}] = {};
    ll1table[{"ADD_EXPR'", "NEQ"}] = {};
    ll1table[{"ADD_EXPR'", "SEMICOLON"}] = {};
    ll1table[{"ADD_EXPR'", "RPAREN"}] = {};
    ll1table[{"ADD_EXPR'", "RBRACKET"}] = {};
    ll1table[{"ADD_EXPR'", "COMMA"}] = {};

    // ======== ADD_OP ========
    ll1table[{"ADD_OP", "PLUS"}] = {"PLUS"};
    ll1table[{"ADD_OP", "MINUS"}] = {"MINUS"};

    // ======== MULT_EXPR ========
    ll1table[{"MULT_EXPR", "INT_CONST"}] = {"UNARY_EXPR", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR", "FLOAT_CONST"}] = {"UNARY_EXPR", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR", "STRING_CONST"}] = {"UNARY_EXPR", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR", "KW_NULL"}] = {"UNARY_EXPR", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR", "IDENT"}] = {"UNARY_EXPR", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR", "KW_NEW"}] = {"UNARY_EXPR", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR", "LPAREN"}] = {"UNARY_EXPR", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR", "MINUS"}] = {"UNARY_EXPR", "MULT_EXPR'"};

    // ======== MULT_EXPR' ========
    // MULT_EXPR' -> MULT_OP UNARY_EXPR MULT_EXPR'
    ll1table[{"MULT_EXPR'", "STAR"}] = {"MULT_OP", "UNARY_EXPR", "#BUILD_MUL", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR'", "SLASH"}] = {"MULT_OP", "UNARY_EXPR", "#BUILD_DIV", "MULT_EXPR'"};
    ll1table[{"MULT_EXPR'", "MOD"}] = {"MULT_OP", "UNARY_EXPR", "#BUILD_MOD", "MULT_EXPR'"};
    // MULT_EXPR' -> ε
    ll1table[{"MULT_EXPR'", "PLUS"}] = {};
    ll1table[{"MULT_EXPR'", "MINUS"}] = {};
    ll1table[{"MULT_EXPR'", "LT"}] = {};
    ll1table[{"MULT_EXPR'", "GT"}] = {};
    ll1table[{"MULT_EXPR'", "LE"}] = {};
    ll1table[{"MULT_EXPR'", "GE"}] = {};
    ll1table[{"MULT_EXPR'", "EQ"}] = {};
    ll1table[{"MULT_EXPR'", "NEQ"}] = {};
    ll1table[{"MULT_EXPR'", "SEMICOLON"}] = {};
    ll1table[{"MULT_EXPR'", "RPAREN"}] = {};
    ll1table[{"MULT_EXPR'", "RBRACKET"}] = {};
    ll1table[{"MULT_EXPR'", "COMMA"}] = {};

    // ======== MULT_OP ========
    ll1table[{"MULT_OP", "STAR"}] = {"STAR"};
    ll1table[{"MULT_OP", "SLASH"}] = {"SLASH"};
    ll1table[{"MULT_OP", "MOD"}] = {"MOD"};

    // ======== UNARY_EXPR ========
    // UNARY_EXPR -> MINUS UNARY_EXPR
    ll1table[{"UNARY_EXPR", "MINUS"}] = {"MINUS", "UNARY_EXPR", "#BUILD_NEG"};
    // UNARY_EXPR -> PRIMARY
    ll1table[{"UNARY_EXPR", "INT_CONST"}] = {"PRIMARY"};
    ll1table[{"UNARY_EXPR", "FLOAT_CONST"}] = {"PRIMARY"};
    ll1table[{"UNARY_EXPR", "STRING_CONST"}] = {"PRIMARY"};
    ll1table[{"UNARY_EXPR", "KW_NULL"}] = {"PRIMARY"};
    ll1table[{"UNARY_EXPR", "IDENT"}] = {"PRIMARY"};
    ll1table[{"UNARY_EXPR", "KW_NEW"}] = {"PRIMARY"};
    ll1table[{"UNARY_EXPR", "LPAREN"}] = {"PRIMARY"};

    // ======== PRIMARY ========
    ll1table[{"PRIMARY", "INT_CONST"}] = {"INT_CONST", "#BUILD_INT"};
    ll1table[{"PRIMARY", "FLOAT_CONST"}] = {"FLOAT_CONST", "#BUILD_FLOAT"};
    ll1table[{"PRIMARY", "STRING_CONST"}] = {"STRING_CONST", "#BUILD_STRING"};
    ll1table[{"PRIMARY", "KW_NULL"}] = {"KW_NULL"};
    // PRIMARY -> IDENT PRIMARY_TAIL
    ll1table[{"PRIMARY", "IDENT"}] = {"IDENT", "#BUILD_VAR", "PRIMARY_TAIL"};
    // PRIMARY -> KW_NEW TYPE_SPEC LBRACKET EXPR RBRACKET
    ll1table[{"PRIMARY", "KW_NEW"}] = {"KW_NEW", "TYPE_SPEC", "LBRACKET", "EXPR", "RBRACKET"};
    // PRIMARY -> LPAREN EXPR RPAREN
    ll1table[{"PRIMARY", "LPAREN"}] = {"LPAREN", "EXPR", "RPAREN"};

    // ======== PRIMARY_TAIL ========
    // PRIMARY_TAIL -> LBRACKET EXPR RBRACKET
    ll1table[{"PRIMARY_TAIL", "LBRACKET"}] = {"LBRACKET", "EXPR", "RBRACKET", "#BUILD_ARRAY_ACCESS"};
    // PRIMARY_TAIL -> LPAREN ARG_LIST RPAREN
    ll1table[{"PRIMARY_TAIL", "LPAREN"}] = {"LPAREN", "#MARK_ARGS", "ARG_LIST", "RPAREN", "#BUILD_CALL"};
    // PRIMARY_TAIL -> ε
    ll1table[{"PRIMARY_TAIL", "STAR"}] = {};
    ll1table[{"PRIMARY_TAIL", "SLASH"}] = {};
    ll1table[{"PRIMARY_TAIL", "MOD"}] = {};
    ll1table[{"PRIMARY_TAIL", "PLUS"}] = {};
    ll1table[{"PRIMARY_TAIL", "MINUS"}] = {};
    ll1table[{"PRIMARY_TAIL", "LT"}] = {};
    ll1table[{"PRIMARY_TAIL", "GT"}] = {};
    ll1table[{"PRIMARY_TAIL", "LE"}] = {};
    ll1table[{"PRIMARY_TAIL", "GE"}] = {};
    ll1table[{"PRIMARY_TAIL", "EQ"}] = {};
    ll1table[{"PRIMARY_TAIL", "NEQ"}] = {};
    ll1table[{"PRIMARY_TAIL", "SEMICOLON"}] = {};
    ll1table[{"PRIMARY_TAIL", "RPAREN"}] = {};
    ll1table[{"PRIMARY_TAIL", "RBRACKET"}] = {};
    ll1table[{"PRIMARY_TAIL", "COMMA"}] = {};

    // ======== PARAM_LIST ========
    // PARAM_LIST -> TYPE_SPEC #BUILD_TYPE IDENT #BUILD_PARAM PARAM_LIST'
    ll1table[{"PARAM_LIST", "KW_INT"}] = {"TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_PARAM", "PARAM_LIST'"};
    ll1table[{"PARAM_LIST", "KW_FLOAT"}] = {"TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_PARAM", "PARAM_LIST'"};
    ll1table[{"PARAM_LIST", "KW_STRING"}] = {"TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_PARAM", "PARAM_LIST'"};
    // PARAM_LIST -> ε
    ll1table[{"PARAM_LIST", "RPAREN"}] = {};

    // ======== PARAM_LIST' ========
    // PARAM_LIST' -> COMMA TYPE_SPEC #BUILD_TYPE IDENT #BUILD_PARAM PARAM_LIST'
    ll1table[{"PARAM_LIST'", "COMMA"}] = {"COMMA", "TYPE_SPEC", "#BUILD_TYPE", "IDENT", "#BUILD_PARAM", "PARAM_LIST'"};
    // PARAM_LIST' -> ε
    ll1table[{"PARAM_LIST'", "RPAREN"}] = {};

    // ======== ARG_LIST ========
    // ARG_LIST -> EXPR ARG_LIST'
    ll1table[{"ARG_LIST", "INT_CONST"}] = {"EXPR", "ARG_LIST'"};
    ll1table[{"ARG_LIST", "FLOAT_CONST"}] = {"EXPR", "ARG_LIST'"};
    ll1table[{"ARG_LIST", "STRING_CONST"}] = {"EXPR", "ARG_LIST'"};
    ll1table[{"ARG_LIST", "KW_NULL"}] = {"EXPR", "ARG_LIST'"};
    ll1table[{"ARG_LIST", "IDENT"}] = {"EXPR", "ARG_LIST'"};
    ll1table[{"ARG_LIST", "KW_NEW"}] = {"EXPR", "ARG_LIST'"};
    ll1table[{"ARG_LIST", "LPAREN"}] = {"EXPR", "ARG_LIST'"};
    ll1table[{"ARG_LIST", "MINUS"}] = {"EXPR", "ARG_LIST'"};
    // ARG_LIST -> ε
    ll1table[{"ARG_LIST", "RPAREN"}] = {};

    // ======== ARG_LIST' ========
    // ARG_LIST' -> COMMA EXPR ARG_LIST'
    ll1table[{"ARG_LIST'", "COMMA"}] = {"COMMA", "EXPR", "ARG_LIST'"};
    // ARG_LIST' -> ε
    ll1table[{"ARG_LIST'", "RPAREN"}] = {};
}