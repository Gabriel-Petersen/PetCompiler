module; 

#include "stl.h"

export module error;

struct CompileError {
        std::string message;
};

std::vector<CompileError>& getLog() {
    static std::vector<CompileError> instance;
    return instance;
}

export namespace error
{
    inline void report(const std::string& msg) {
        getLog().push_back({msg});
    }

    inline bool hasErrors() {
        return !getLog().empty();
    }

    inline void dump() {
        for (const auto& err : getLog()) {
            std::clog << "[Erro]: " << err.message << std::endl;
        }
    }

    inline void clear() {
        getLog().clear();
    }
}