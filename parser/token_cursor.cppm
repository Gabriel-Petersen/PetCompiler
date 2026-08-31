module;

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

export module parser.token_cursor;

import token;
import error;

export class TokenCursor
{
private:
    std::vector<Token> tokens;
    std::size_t current = 0;

public:
    explicit TokenCursor(std::vector<Token> tokenList) : tokens(std::move(tokenList))
    {
        if (tokens.empty())
            tokens.push_back(
                Token{TokenType::END_OF_FILE, "", TokenMeta{0, 0}}
            );
        
    }

    [[nodiscard]] bool isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
    [[nodiscard]] bool check(TokenType type) const { return peek().type == type; }
    [[nodiscard]] std::size_t position() const { return current; }

    [[nodiscard]] const Token& peek() const
    {
        if (current >= tokens.size())
            return tokens.back();

        return tokens[current];
    }

    [[nodiscard]] const Token& previous() const
    {
        if (current == 0)
            return tokens.back();

        return tokens[current - 1];
    }

    [[nodiscard]] const Token& lookAhead(std::size_t distance = 1) const
    {
        if (distance >= tokens.size() - current)
            return tokens.back();

        return tokens[current + distance];
    }

    const Token& advance()
    {
        if (!isAtEnd())
            ++current;

        return previous();
    }

    bool match(TokenType type)
    {
        if (!check(type))
            return false;

        advance();
        return true;
    }

    const Token* consume(TokenType type, const std::string& errorMessage)
    {
        if (check(type))
            return &advance();

        error::report(errorMessage);
        return nullptr;
    }

    void synchronize()
    {
        while (!isAtEnd() && !check(TokenType::SEMICOLON))
            advance();

        if (check(TokenType::SEMICOLON))
            advance();
    }
};
