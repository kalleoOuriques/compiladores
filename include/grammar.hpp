#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <map>
#include <vector>
#include <set>

class Grammar {
public:
    std::map<std::string, std::vector<std::vector<std::string>>> rules;

    Grammar();
    void buildLL1Table();
    std::map<std::pair<std::string, std::string>, std::vector<std::string>> ll1table;

private:
    std::set<std::string> terminals;
    std::set<std::string> nonTerminals;
};

#endif
