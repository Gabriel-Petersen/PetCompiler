module;

#include "stl.h"

export module interpreter.runtime;

import parser.evaluation;
import types.info;

struct RuntimeVariable
{
    TypeInfo type;
    Evaluation value;
    std::size_t depth;
};

namespace
{
    std::unordered_map<std::string, RuntimeVariable> variablePool;
    std::size_t currentDepth = 0;
}

export namespace runtime
{
    void enterScope() { ++currentDepth; }

    void exitScope() {
        if (currentDepth > 0) --currentDepth;
    }

    [[nodiscard]] std::size_t getCurrentDepth() {
        return currentDepth;
    }

    void define(std::string name, TypeInfo type, Evaluation value) {
        variablePool.insert_or_assign(
            std::move(name),RuntimeVariable{std::move(type), std::move(value), currentDepth}
        );
    }

    bool assign(const std::string& name, Evaluation value)
    {
        const auto found = variablePool.find(name);

        if (found == variablePool.end())
        {
            std::cout << "[RUNTIME-ERROR]: Variavel '" << name << "' redefinida sem ser declarada!\n";
            return false;
        }

        found->second.value = std::move(value);
        return true;
    }

    [[nodiscard]] Evaluation get(const std::string& name)
    {
        const auto found = variablePool.find(name);

        if (found == variablePool.end())
        {
            std::cout << "[RUNTIME-ERROR]: Variavel '" << name << "' lida sem ser declarada!\n";
            return Evaluation();
        }

        const RuntimeVariable& entry = found->second;

        if (entry.depth > currentDepth) {
            std::cout << "[RUNTIME-WARN]: Variavel '" << name << "' (depth " << entry.depth 
                      << ") acessada de um escopo mais raso (" << currentDepth << ")!\n";
        }

        return entry.value;
    }

    [[nodiscard]] const RuntimeVariable* inspect(const std::string& name)
    {
        const auto found = variablePool.find(name);

        if (found == variablePool.end())
            return nullptr;

        return &found->second;
    }

    [[nodiscard]] const std::unordered_map<std::string, RuntimeVariable>& inspectAll() {
        return variablePool;
    }

    void clear() {
        variablePool.clear();
        currentDepth = 0;
    }
}