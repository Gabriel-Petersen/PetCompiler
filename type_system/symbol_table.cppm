module;

#include "stl.h"

export module types.symbol_table;

import types.info;
import error;

export class SymbolTable
{
    std::shared_ptr<SymbolTable> parent;
    std::unordered_map<std::string, TypeInfo> symbols;

public:
    SymbolTable(std::shared_ptr<SymbolTable> parent) : parent(std::move(parent)) { }
    SymbolTable() : parent(nullptr) { }

    bool define(const std::string& name, TypeInfo type)
    {
        bool sucess = symbols.count(name) == 0;

        if (!sucess)
            error::report("Variable of name \'" + name + "\' already defined in this scope");
        else
            symbols[name] = type;

        return sucess;
    }

    std::optional<TypeInfo> lookup(const std::string& name)
    {
        auto found = symbols.find(name);
        if (found != symbols.end()) return *found;
        
        return parent == nullptr ? std::nullopt : parent->lookup(name);
    }
};
