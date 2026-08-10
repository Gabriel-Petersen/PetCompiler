#pragma once

#include "../type_system/type_info.h"
#include "../ast/tree_node.h"
#include <unordered_map>

namespace runtime 
{
    struct VarEntry {
        types::TypeInfo type;
        ast::Evaluation value;
        int depth;
    };

    static std::unordered_map<std::string, VarEntry> variablePool;
    static int currentDepth = 0;

    inline void enterScope() { currentDepth++; }
    inline void exitScope() { currentDepth--; }

    inline void define(std::string name, types::TypeInfo type, ast::Evaluation val, bool new_assignment = true) 
    {
        if (!new_assignment)
        {
            auto search = variablePool.find(name);
            if (search == variablePool.end())
            {
                std::cout << "[RUNTIME-ERROR]: Variavel '" << name << "' redefinida sem ser declarada!\n";
                return;
            }
            search->second.type = type;
            search->second.value = val;
            return;
        }
        
        variablePool[name] = {type, val, currentDepth};
        // std::cout << "[DEBUG-VAR]: '" << name << "' definida na profundidade " << currentDepth << "\n";
    }

    inline ast::Evaluation get(std::string name) 
    {
        if (variablePool.find(name) == variablePool.end())
         {
            std::cout << "[RUNTIME-ERROR]: Variavel '" << name << "' lida sem ser declarada!\n";
            return ast::Evaluation();
        }
        
        auto& entry = variablePool[name];
        if (entry.depth > currentDepth) {
            std::cout << "[RUNTIME-WARN]: Variavel '" << name << "' (depth " << entry.depth 
                      << ") acessada de um escopo mais raso (" << currentDepth << ")!\n";
        }
        return entry.value;
    }
}