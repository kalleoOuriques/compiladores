#include "symbol_table.hpp"
#include <iostream>

void SymbolTable::addOccurrence(const std::string &name, int line, int col) {
    if (!exists(name)) {
        table[name] = SymbolEntry{name, {}, ""};
    }
    table[name].occurrences.push_back({line, col});
}

bool SymbolTable::exists(const std::string &name) const {
    return table.find(name) != table.end();
}

const SymbolEntry& SymbolTable::get(const std::string &name) const {
    return table.at(name);
}

void SymbolTable::print() const {
    for (const auto &entry : table) {
        std::cout << entry.first << " occurs at: ";
        for (const auto &p : entry.second.occurrences) {
            std::cout << "(" << p.first << "," << p.second << ") ";
        }
        std::cout << "\n";
    }
}
