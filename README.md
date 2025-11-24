# ConvCC-2025-2 Compiler

Compilador completo para a linguagem ConvCC-2025-2 com:
- **Analisador Léxico** (Tarefa 1)
- **Analisador Sintático LL(1)** (Tarefa 2)

## 📋 Requisitos

- **Sistema**: Linux (Ubuntu 22.04+)
- **Compilador**: g++ 11.4.0 ou superior
- **C++ Standard**: C++17
- **Bibliotecas**: STL apenas (sem dependências externas)

## 🚀 Como Compilar

```bash
cd /home/kalleoubuntu/Documents/compilador
make clean
make
```

## ▶️ Como Executar

```bash
./compiler <arquivo.convcc>
```

### Executar todos os testes automaticamente:

```bash
make test
```

### Executar testes individuais:

```bash
# Teste correto (deve passar)
./compiler test/test_correct.convcc

# Testes com erro (devem falhar)
./compiler test/test_syntax_error.convcc
./compiler test/test_lexical_error.convcc
```

## 🧪 Resultados Esperados

### ✅ test_correct.convcc (240 linhas)

Programa sintaticamente e lexicamente correto.

**Saída:**
```
Programa sintaticamente correto!

Tabela de símbolos:
[Lista de identificadores com ocorrências (linha, coluna)]
```

### ❌ test_syntax_error.convcc (220 linhas)

Erro sintático na linha 88: falta `;` após declaração de variável.

**Saída:**
```
Erro sintático: esperado 'SEMICOLON' mas encontrado 'IDENT' na linha 89, coluna 1
```

### ❌ test_lexical_error.convcc (240 linhas)

Erro léxico na linha 90: caractere inválido `@` no identificador.

**Saída:**
```
Erro léxico: Unexpected char: @ na linha 90, coluna 10
```

## 📝 Estrutura do Código

```
compilador/
├── include/          # Headers (.hpp)
├── src/              # Implementações (.cpp)
├── test/             # Arquivos de teste (.convcc)
├── Makefile          # Build system
└── README.md         # Este arquivo
```

## 🔤 Tokens Reconhecidos

- **Identificadores**: `[a-zA-Z_][a-zA-Z0-9_]*`
- **Constantes**: inteiros, floats, strings
- **Palavras-chave**: `int`, `float`, `string`, `def`, `for`, `if`, `else`, `return`, `break`, `print`, `read`, `new`, `null`
- **Operadores**: `+`, `-`, `*`, `/`, `%`, `<`, `>`, `<=`, `>=`, `==`, `!=`, `=`
- **Delimitadores**: `(`, `)`, `{`, `}`, `[`, `]`, `,`, `;`

## 🌳 Gramática LL(1)

A gramática foi transformada para LL(1) removendo:
- Recursão à esquerda nas expressões
- Ambiguidades
- Necessidade de fatoração à esquerda

Exemplo de transformação:
```
# Antes (com recursão à esquerda)
EXPR → EXPR + TERM | TERM

# Depois (LL(1))
EXPR → TERM EXPR'
EXPR' → + TERM EXPR' | ε
```

## 🛠️ Construtos Suportados

- Declarações: `int x;`, `float y = 3.14;`
- Arrays: `arr = new int[10];`, `arr[0] = 5;`
- Expressões: `x = (a + b) * c - d / 2;`
- Condicionais: `if (x > 10) { } else { }`
- Loops: `for (int i = 0; i < 10; i = i + 1) { }`
- Funções: `def soma(int a, int b) { return a + b; }`
- Comandos: `print(x);`, `read(y);`, `return expr;`, `break;`

## 🔍 Detecção de Erros

### Erros Léxicos
- Caracteres inválidos
- Strings não terminadas
- Símbolos desconhecidos

### Erros Sintáticos
- Tokens inesperados
- Falta de delimitadores (`;`, `)`, `}`, etc.)
- Estrutura inválida

O compilador **para imediatamente** ao encontrar um erro e exibe:
- Tipo do erro (léxico ou sintático)
- Linha e coluna
- Descrição clara do problema

## 📊 Implementação Técnica

### Analisador Léxico
- Implementado com **DFA** (Autômatos Finitos Determinísticos)
- Leitura caractere por caractere
- Tabela de símbolos com todas as ocorrências

### Analisador Sintático
- Parser **LL(1)** com pilha
- Tabela LL(1) com ~300 entradas
- Construída manualmente em `grammar.cpp`

---

**Status**: ✅ Completo - Tarefas 1 e 2 implementadas e testadas.
