module;

#include <memory>
#include <unordered_map>

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

    void define(std::string name, TypeInfo type)
    {
        if (symbols.count(name))
            errorR::report("Variable of name \'" + name + "\' already defined in this scope");
        else
            symbols[name] = type;
    }

    std::optional<TypeInfo> lookup(std::string name)
    {
        if (symbols.count(name))
            return symbols[name];
        
        return parent == nullptr ? std::nullopt : parent->lookup(name);
    }
};