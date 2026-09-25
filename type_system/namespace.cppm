module;

#include <string>
#include <vector>

export module types.nameSpace;

namespace {
    std::vector<std::string> split(std::string s, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        tokens.push_back(s);

        return tokens;
    }
};

export class Namespace {
private:
    std::vector<std::string> pieces;
public:
    const std::string fullName;

    explicit Namespace(const std::string& namespaceStr) : fullName(namespaceStr) {
        if (namespaceStr.empty()) return;
        pieces = split(namespaceStr, ".");
    }

    [[nodiscard]] bool isInside(const Namespace& o) const {
        if (o.pieces.size() > pieces.size()) return false;
        
        for (std::size_t i = 0; i < o.pieces.size(); ++i) {
            if (pieces[i] != o.pieces[i]) return false;
        }
        
        return true;
    }

    bool operator==(const Namespace& o) const { return fullName == o.fullName; }
    bool operator!=(const Namespace& o) const { return fullName != o.fullName; }
};