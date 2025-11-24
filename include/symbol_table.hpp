#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct SymbolEntry {
    std::string name;
    std::vector<std::pair<int,int>> occurrences;
    std::string type; // será usado na fase semântica futuramente
};

class SymbolTable {
private:
    std::unordered_map<std::string, SymbolEntry> table;

public:
    void addOccurrence(const std::string &name, int line, int col);
    bool exists(const std::string &name) const;
    const SymbolEntry& get(const std::string &name) const;
    void print() const;
};

#endif
