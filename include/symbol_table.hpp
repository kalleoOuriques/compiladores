#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct SymbolEntry
{
    std::string name;
    std::vector<std::pair<int, int>> occurrences;
    std::string type; // será usado na fase semântica futuramente
};

class SymbolTable
{
private:
    std::vector<std::unordered_map<std::string, SymbolEntry>> scopes;

public:
    SymbolTable();
    void enterScope();
    void exitScope();
    void addOccurrence(const std::string &name, int line, int col);
    SymbolEntry *lookup(const std::string &name);
    bool exists(const std::string &name);
    bool definedInCurrentScope(const std::string &name);
    const SymbolEntry &get(const std::string &name);
    void print() const;
};

#endif
