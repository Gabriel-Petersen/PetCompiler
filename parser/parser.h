#pragma once
#include "../ast/tree_node.h"
#include "../ast/expression_nodes.h"
#include "../lexer/lexer.h"
#include "../compile_error/compile_error.h"
#include "../ast/debug_nodes.h"
#include "../ast/statement_nodes.h"
#include "../ast/ast.h"

namespace parser
{
    using namespace ast;

    class Parser
    {
        AstTree* tree = new AstTree();
        size_t current = 0;
        std::vector<lexer::Token> tokens;

        bool isTypeToken(lexer::TokenType type)
        {
            return type == lexer::TP_BYTE || type == lexer::TP_CHAR ||
                type == lexer::TP_SMALL || type == lexer::TP_INT ||
                type == lexer::TP_LONG || type == lexer::TP_FLOAT ||
                type == lexer::TP_DOUBLE;
        }

        types::TypeInfo parseTypeFromToken(const lexer::Token& tk)
        {
            using namespace types;

            switch (tk.type)
            {
            case lexer::TP_BYTE:
                return TypeInfo{TypeKind::BYTE};
            case lexer::TP_CHAR:
                return TypeInfo{TypeKind::CHAR};
            case lexer::TP_SMALL:
                return TypeInfo{TypeKind::SMALL};
            case lexer::TP_INT:
                return TypeInfo{TypeKind::INT};
            case lexer::TP_LONG:
                return TypeInfo{TypeKind::LONG};
            case lexer::TP_FLOAT:
                return TypeInfo{TypeKind::FLOAT};
            case lexer::TP_DOUBLE:
                return TypeInfo{TypeKind::DOUBLE};
            default:
                error::report("Unexpected type token on declaration: " + tk.src);
                return TypeInfo{TypeKind::INT};
            }
        }

        lexer::Token peek() { return tokens[current]; }
        lexer::Token advance() { current++; return before(); }
        lexer::Token before() 
        { 
            if (current <= 0) 
                return tokens[tokens.size() - 1];  // retorna EOF token se tentar ler o primeiro
            return tokens[current - 1]; 
        } 

        void consumeSemicolon()
        {
            if (isEnd())
            {
                error::report("Semicolon ';' expected");
                return;
            }

            auto tk = peek();
            if (tk.type == lexer::SEMICOLON)
                current++;
            else
                error::report("Semicolon ';' expected");
        }

        bool isEnd() { return current >= tokens.size(); }

        void testPanic()
        {
            while (!isEnd())
            {
                auto tk = peek();
                while (tk.type != lexer::SEMICOLON && tk.type != lexer::END_OF_FILE)
                {
                    advance();
                    if (isEnd())
                        return;
                    tk = peek();
                }

                if (!isEnd())
                    advance();
            }
        }

        BlockStmt* block()
        {
            std::vector<Stmt*> blockStmt;
            while (!isEnd() && peek().type != lexer::END_OF_FILE && peek().type != lexer::R_BRACES)
            {
                blockStmt.push_back(parseStatement());
            }

            if (peek().type == lexer::R_BRACES)
                advance();
            else
                error::report("Expected }");
            return new BlockStmt(blockStmt);
        }

        Stmt* parseStatement()
        {
            auto tk = peek();

            if (isTypeToken(tk.type))
            {
                auto typeToken = advance();
                auto typeInfo = parseTypeFromToken(typeToken);

                if (isEnd() || peek().type != lexer::IDENTIFYER)
                {
                    error::report("Expected identifyer after type declaration");
                    testPanic();
                    return nullptr;
                }

                std::string identifyer = advance().src;
                Expr* initializer = nullptr;

                if (!isEnd() && peek().type == lexer::EQUAL)
                {
                    advance();
                    initializer = expression();
                    if (initializer == nullptr)
                        testPanic();
                }

                consumeSemicolon();
                return new VarDeclStmt(typeInfo, identifyer, initializer);
            }
            else if (tk.type == lexer::IDENTIFYER && tokens[current + 1].type == lexer::EQUAL)
            {
                std::string name = advance().src; 
                advance();
                
                Expr* val = expression();
                if (val == nullptr)
                    testPanic();
                    
                consumeSemicolon();
                return new AssignmentStmt(name, val);
            }
            else if (tk.type == lexer::L_BRACES) 
            {
                advance();
                return block();
            }
            else if (tk.type == lexer::PRINT)
            {
                advance();
                auto expr = expression();
                if (expr == nullptr)
                    testPanic();
                consumeSemicolon();
                return new PrintStmt(expr);
            }
            else if (tk.type == lexer::IF)
            {
                advance(); // Pula 'if'
                
                if (peek().type != lexer::L_PAREN) 
                    error::report("Expected '('");
                else 
                    advance();

                auto cond = expression();

                if (peek().type != lexer::R_PAREN) 
                    error::report("Expected ')'");
                else 
                    advance();

                if (peek().type != lexer::L_BRACES) {
                    error::report("Expected '{' before if body");
                    testPanic();
                }
                advance(); // Consome '{'
                auto thenStmt = block();

                if (peek().type == lexer::ELSE)
                {
                    advance(); // Pula 'else'
                    
                    if (peek().type == lexer::IF)
                        return new IfStmt(cond, thenStmt, new BlockStmt({ parseStatement() }));
                    else if (peek().type == lexer::L_BRACES) 
                    {
                        advance(); // Consome '{'
                        return new IfStmt(cond, thenStmt, block());
                    } 
                    else
                        error::report("Expected '{' or 'if' after else");
                }
                return new IfStmt(cond, thenStmt);
            }
            else if (tk.type == lexer::RETURN)
            {
                advance();

                Expr* expr = nullptr;
                if (peek().type != lexer::SEMICOLON)
                    expr = expression();

                if (expr == nullptr && !isEnd() && peek().type != lexer::SEMICOLON)
                    testPanic();

                consumeSemicolon();
                return new ReturnStmt(expr);
            }

            auto expr = expression();
            if (expr == nullptr) 
                testPanic();
            consumeSemicolon();
            return new ExprStmt(expr);
        }

        Expr* expression()
        {
            return equality();
        }

        Expr* equality() 
        {
            Expr* left = comparison();

            auto tk = peek();
            if (tk.type == lexer::EQ_EQ || tk.type == lexer::NOT_EQ) 
            {
                advance();
                BinaryExprType tp = (tk.type == lexer::EQ_EQ) ? 
                    BinaryExprType::EQUALS : BinaryExprType::UNEQUALS;
                return new BinaryExpr(left, tp, comparison());
            }
            
            return left;
        }

        Expr* comparison() 
        {
            Expr* left = term();

            lexer::TokenType type = peek().type; 

            if (type == lexer::GRATER_THEN || type == lexer::GREATER_OR_EQUALS || 
                type == lexer::LESSER_THEN || type == lexer::LESSER_OR_EQUALS) 
            {
                auto tk = advance(); 
                
                BinaryExprType op;
                if (tk.type == lexer::GRATER_THEN) op = BinaryExprType::GREATER;
                else if (tk.type == lexer::GREATER_OR_EQUALS) op = BinaryExprType::GREATER_EQ;
                else if (tk.type == lexer::LESSER_THEN) op = BinaryExprType::LESSER;
                else op = BinaryExprType::LESSER_EQ;

                return new BinaryExpr(left, op, term());
            }

            return left;
        }

        Expr* term()
        {
            Expr* left = factor();

            lexer::Token tk = peek();
            while (tk.type == lexer::PLUS || tk.type == lexer::MINUS) {
                advance();
                left = new BinaryExpr(
                    left, 
                    tk.type == lexer::PLUS ? BinaryExprType::ADD : BinaryExprType::SUB,
                    factor()
                );
                tk = peek();
            }

            return left;
        }

        Expr* factor()
        {
            Expr* left = unary();

            lexer::Token tk = peek();
            while (tk.type == lexer::STAR || tk.type == lexer::SLASH || tk.type == lexer::MOD) {
                advance();
                auto newType = tk.type == lexer::STAR ? 
                        BinaryExprType::MUL : 
                        tk.type == lexer::SLASH ? BinaryExprType::DIV : BinaryExprType::MOD;
                left = new BinaryExpr(
                    left, 
                    newType,
                    unary()
                );
                tk = peek();
            }

            return left;
        }

        Expr* unary()
        {
            lexer::Token tk = peek();
            if (tk.type == lexer::MINUS || tk.type == lexer::NOT)
            {
                advance();
                auto newType = tk.type == lexer::MINUS ? UnaryExprType::MINUS : UnaryExprType::NOT;
                return new UnaryExpr(newType, unary());
            }
            else
                return primary();
        }

        Expr* primary()
        {
            auto tk = advance();

            if (tk.type == lexer::INT_LITERAL)
                return new LiteralExpr(Evaluation(std::stoll(tk.src, 0)));
            else if (tk.type == lexer::FLOAT_LITERAL)
                return new LiteralExpr(Evaluation(std::stod(tk.src, 0)));
            else if (tk.type == lexer::BOOL_LITERAL)
                return new LiteralExpr(Evaluation(tk.src == "true"));
            else if (tk.type == lexer::IDENTIFYER)
                return new VarExpr(tk.src);
            else if (tk.type == lexer::L_PAREN)
            {
                auto res = expression();
                if (advance().type != lexer::R_PAREN)
                {
                    error::report("Token de fechamento de parenteses ausente no primary: " + tk.src);
                }
                return res;
            }

            error::report("Token inesperado: '" + tk.src + "' do tipo " + std::to_string(tk.type));
            return nullptr;
        }

    public:
        Parser(lexer::Lexer& lx) : tokens(lx.getAllToken()) { }
        ~Parser() { delete tree; }

        AstTree* getAst() 
        {
            if (tree->root->isEmpty())
                build();
            return tree;
        }

        void build()
        {
            while (!isEnd() && peek().type != lexer::END_OF_FILE)
            {
                tree->addStmt(parseStatement());
            }
        }
    };
}
