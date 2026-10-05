module;

#include <memory>
#include <utility>
#include <string>
#include <optional>

export module analyzer.scope;

import types.symbol_table;
import types.info;
import error;

template<typename T>
using ptr = std::shared_ptr<T>;

export class ScopeManager {
private:
    ptr<SymbolTable> currentScope;
public:
    void enterScope() {
        currentScope = std::make_shared<SymbolTable>(currentScope);
    }

    void exitScope()
    {
        ptr<SymbolTable> ptrParent = currentScope->getParent();
        if (ptrParent == nullptr) {
            error::report("[INTERNAL-ERROR] ScopeManager tried to exit a root-scope");
            return;
        }

        currentScope = ptrParent;
    }

    bool define(const std::string& name, SymbolInfo type) {
        return currentScope->define(name, type);
    }

    std::optional<SymbolInfo> lookup(const std::string& name) {
        return currentScope->lookup(name);
    }
};
