module;

#include <memory>
#include <utility>
#include <string>

export module parser.expressions;

import token;
import parser.token_cursor;
import parser.evaluation;
import error;
import ast.node;
import ast.expressions;

template<typename T>
using ptr = std::unique_ptr<T>;

export class ExpressionParser {
private:
    TokenCursor& cursor;

    ptr<Expr> primary()
    {
        auto tk = cursor.advance();

        if (tk.type == TokenType::INT_LITERAL)
            return std::make_unique<LiteralExpr>(Evaluation(std::stoll(tk.src, 0)));
        else if (tk.type == TokenType::FLOAT_LITERAL)
            return std::make_unique<LiteralExpr>(Evaluation(std::stod(tk.src, 0)));
        else if (tk.type == TokenType::BOOL_LITERAL)
            return std::make_unique<LiteralExpr>(Evaluation(tk.src == "true"));
        else if (tk.type == TokenType::IDENTIFYER)
            return std::make_unique<VarExpr>(tk.src);
        else if (tk.type == TokenType::L_PAREN)
        {
            auto res = equality();
            cursor.consume(TokenType::R_PAREN, "Token de fechamento de parenteses ausente");
            return res;
        }

        error::report("Token inesperado: '" + tk.src + "' do tipo " + std::to_string(static_cast<int>(tk.type)));
        return nullptr;
    }

    ptr<Expr> unary()
    {
        Token tk = cursor.peek();
        if (tk.type == TokenType::MINUS || tk.type == TokenType::NOT)
        {
            cursor.advance();
            auto newType = tk.type == TokenType::MINUS ? UnaryExprType::MINUS : UnaryExprType::NOT;
            return std::make_unique<UnaryExpr>(newType, unary());
        }
            
        return primary();
    }

    ptr<Expr> factor()
    {
        auto left = unary();

        Token tk = cursor.peek();
        while (tk.type == TokenType::STAR || tk.type == TokenType::SLASH || tk.type == TokenType::MOD) 
        {
            cursor.advance();
            auto newType = 
                    tk.type == TokenType::STAR ? 
                    BinaryExprType::MUL : 
                    tk.type == TokenType::SLASH ? BinaryExprType::DIV : BinaryExprType::MOD;
            
            left = std::make_unique<BinaryExpr>(std::move(left), newType, unary());
            tk = cursor.peek();
        }

        return left;
    }

    ptr<Expr> term()
    {
        auto left = factor();

        Token tk = cursor.peek();
        while (tk.type == TokenType::PLUS || tk.type == TokenType::MINUS) 
        {
            cursor.advance();
            left = std::make_unique<BinaryExpr>(std::move(left), tk.type == TokenType::PLUS ? BinaryExprType::ADD : BinaryExprType::SUB, factor());
            tk = cursor.peek();
        }

        return left;
    }

    ptr<Expr> comparison()
    {
        auto left = term();
        TokenType type = cursor.peek().type;

        if (type == TokenType::GRATER_THEN || type == TokenType::GREATER_OR_EQUALS || 
            type == TokenType::LESSER_THEN || type == TokenType::LESSER_OR_EQUALS) 
        {
            auto tk = cursor.advance(); 
            
            BinaryExprType op;
            if (tk.type == TokenType::GRATER_THEN) op = BinaryExprType::GREATER;
            else if (tk.type == TokenType::GREATER_OR_EQUALS) op = BinaryExprType::GREATER_EQ;
            else if (tk.type == TokenType::LESSER_THEN) op = BinaryExprType::LESSER;
            else op = BinaryExprType::LESSER_EQ;

            return std::make_unique<BinaryExpr>(std::move(left), op, term());
        }

        return left;
    }

    ptr<Expr> equality() 
    {
        auto left = comparison();

        if (!cursor.check(TokenType::EQ_EQ) && !cursor.check(TokenType::NOT_EQ)) return left;

        const Token& tk = cursor.advance();
        const BinaryExprType tp = 
            (tk.type == TokenType::EQ_EQ) ? BinaryExprType::EQUALS : BinaryExprType::UNEQUALS;

        auto right = comparison();
        if (!left || !right) return nullptr;

        auto result = std::make_unique<BinaryExpr>(std::move(left), tp, std::move(right));

        if (cursor.check(TokenType::EQ_EQ) || cursor.check(TokenType::NOT_EQ)) {
            error::report("Comparacoes de igualdade encadeadas exigem parenteses");
            return nullptr;
        }

        return result;
    }

public:
    explicit ExpressionParser(TokenCursor& cursor) : cursor(cursor) { }

    [[nodiscard]] ptr<Expr> parseExpr() { return equality(); }
};
