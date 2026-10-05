module;

#include <memory>
#include <string>
#include <optional>
#include <unordered_map>
#include <utility>

export module types.symbol_table;

import types.info;
import error;

export class SymbolTable
{
    std::shared_ptr<SymbolTable> parent;
    std::unordered_map<std::string, SymbolInfo> symbols;

public:
    explicit SymbolTable(std::shared_ptr<SymbolTable> parent) : parent(std::move(parent)) { }
    SymbolTable() : parent(nullptr) { }

    [[nodiscard]] std::shared_ptr<SymbolTable> getParent() const { 
        return parent;
    }

    bool define(const std::string& name, SymbolInfo type)
    {
        const auto [it, inserted] = symbols.emplace(name, type);
        if (!inserted)
            error::report("Variable of name \'" + name + "\' already defined in this scope");
        return inserted;
    }

    std::optional<SymbolInfo> lookup(const std::string& name) const
    {
        auto found = symbols.find(name);
        if (found != symbols.end()) return found->second;
        
        return parent == nullptr ? std::nullopt : parent->lookup(name);
    }
};
