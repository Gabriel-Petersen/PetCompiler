module; 

#include <string>
#include <vector>
#include <iostream>

export module error;

struct CompileError {
        std::string message;
};

inline std::vector<CompileError>& getLog() {
        static std::vector<CompileError> instance;
        return instance;
}

export namespace errorR
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