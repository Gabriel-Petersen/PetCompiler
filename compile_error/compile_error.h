#pragma once
#include <string>
#include <vector>
#include <iostream>

namespace error
{
    struct CompileError {
        std::string message;
    };

    inline static std::vector<CompileError> errorLog;

    inline void report(const std::string& msg) {
        errorLog.push_back({msg});
    }

    inline bool hasErrors() {
        return !errorLog.empty();
    }

    inline void dump() {
        for (const auto& err : errorLog) {
            std::clog << "[Erro]: " << err.message << std::endl;
        }
    }

    inline void clear() {
        errorLog.clear();
    }
}