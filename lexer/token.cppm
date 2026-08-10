module;

#include <string>
#include <utility>
#include <cstddef>

export module token;

export enum class TokenType { // vou ter que adicionar mais no futuro
    IDENTIFYER, 
    PLUS, MINUS, STAR, SLASH, EQUAL, MOD, GRATER_THEN, LESSER_THEN, NOT,
    IF, ELSE, WHILE, FOR, RETURN,
    TP_VOID, TP_BYTE, TP_CHAR, TP_SMALL, TP_INT, TP_LONG, TP_FLOAT, TP_DOUBLE, // aqui para types
    INT_LITERAL, FLOAT_LITERAL, BOOL_LITERAL, STRING_LITERAL, // aqui para literals
    L_PAREN, R_PAREN, L_BRACES, R_BRACES, SEMICOLON, COMMA, DOT,
    PLUSPLUS, MINUSMINUS, PLUS_EQ, MINUS_EQ, STAR_EQ, SLASH_EQ, MOD_EQ, 
    EQ_EQ, NOT_EQ, GREATER_OR_EQUALS, LESSER_OR_EQUALS,
    UNKNOWN, END_OF_FILE,
    PRINT
};

export using TokenMeta = std::pair<size_t, size_t>;

export struct Token {
    TokenType type;
    std::string src;
    TokenMeta meta;
};
