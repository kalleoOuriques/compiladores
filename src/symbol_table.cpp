#include "symbol_table.hpp"
#include <iostream>
#include <stdexcept>

SymbolTable::SymbolTable()
{
    enterScope();
}

void SymbolTable::enterScope()
{
    scopes.push_back({});
}

void SymbolTable::exitScope()
{
    if (!scopes.empty())
    {
        scopes.pop_back();
    }
}

void SymbolTable::addOccurrence(const std::string &name, int line, int col)
{
    if (scopes.empty())
    {
        enterScope();
    }
    auto &currentScope = scopes.back();
    if (currentScope.find(name) == currentScope.end())
    {
        currentScope[name] = SymbolEntry{name, {}, ""};
    }
    currentScope[name].occurrences.push_back({line, col});
}

SymbolEntry *SymbolTable::lookup(const std::string &name)
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return &found->second;
        }
    }
    return nullptr;
}

bool SymbolTable::exists(const std::string &name)
{
    return lookup(name) != nullptr;
}

bool SymbolTable::definedInCurrentScope(const std::string &name)
{
    if (scopes.empty())
        return false;
    return scopes.back().find(name) != scopes.back().end();
}

const SymbolEntry &SymbolTable::get(const std::string &name)
{
    SymbolEntry *entry = lookup(name);
    if (!entry)
    {
        throw std::out_of_range("Symbol not found: " + name);
    }
    return *entry;
}

void SymbolTable::print() const
{
    int scopeLevel = 0;
    for (const auto &scope : scopes)
    {
        std::cout << "Scope " << scopeLevel++ << ":\n";
        for (const auto &entry : scope)
        {
            std::cout << "  " << entry.first << " (" << entry.second.type << ") occurs at: ";
            for (const auto &p : entry.second.occurrences)
            {
                std::cout << "(" << p.first << "," << p.second << ") ";
            }
            std::cout << "\n";
        }
    }
}
