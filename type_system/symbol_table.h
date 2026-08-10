#pragma once // file symbol_table.h

#include "type_info.h"
#include "../compile_error/compile_error.h"
#include <unordered_map>
#include <memory>

namespace types
{
    class SymbolTable
    {
        std::shared_ptr<SymbolTable> parent;
        std::unordered_map<std::string, TypeInfo> symbols;

    public:
        SymbolTable(std::shared_ptr<SymbolTable> parent) : parent(std::move(parent)) { }
        SymbolTable() : parent(nullptr) { }

        void define(std::string name, TypeInfo type)
        {
            if (symbols.count(name))
                error::report("Variable of name \'" + name + "\' already defined in this scope");
            else
                symbols[name] = type;
        }

        TypeInfo* lookup(std::string name)
        {
            if (symbols.count(name))
                return &symbols[name];
            
            return parent == nullptr ? nullptr : parent->lookup(name);
        }
    };
}