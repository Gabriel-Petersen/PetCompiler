module;

#include <memory>
#include <string>
#include <utility>

export module parser.statements;

import lexer.token;
import parser.token_cursor;
import parser.expressions;
import parser.types;
import error;
import ast.node;
import ast.statements;
import ast.debug_nodes;

template<typename T>
using ptr = std::unique_ptr<T>;

export class StatementParser {
private:
    TokenCursor& cursor;
    ExpressionParser& exprParser;

    bool consumeSemicolon()
    {
        if (cursor.match(TokenType::SEMICOLON)) return true;
        error::report("Semicolon ';' expected");
        return false;
    }

    ptr<VarDeclStmt> parseVariableDeclaration()
    {
        auto typeToken = cursor.advance();
        auto typeInfo = type_parser::parse(typeToken);

        if (!cursor.check(TokenType::IDENTIFYER))
        {
            error::report("Expected identifyer after type declaration");
            cursor.synchronize();
            return nullptr;
        }

        std::string identifyer = cursor.advance().src;
        ptr<Expr> initializer = nullptr;

        if (cursor.match(TokenType::EQUAL))
        {
            initializer = exprParser.parseExpr();
            if (initializer == nullptr) {
                cursor.synchronize();
                return nullptr;
            }
        }

        if (!consumeSemicolon()) {
            cursor.synchronize();
            return nullptr;
        }

        return std::make_unique<VarDeclStmt>(identifyer, typeInfo, std::move(initializer));
    }

    ptr<AssignmentStmt> parseAssignment()
    {
        std::string name = cursor.advance().src;

        if (!cursor.consume(TokenType::EQUAL, "Expected '=' after assignment target")) {
            cursor.synchronize();
            return nullptr;
        }
        
        auto val = exprParser.parseExpr();
        if (val == nullptr) {
            cursor.synchronize();
            return nullptr;
        }
            
        if (!consumeSemicolon()) {
            cursor.synchronize();
            return nullptr;
        }

        return std::make_unique<AssignmentStmt>(name, std::move(val));
    }

    ptr<IfStmt> parseIf()
    {
        cursor.advance(); // Pula 'if'
                
        if (!cursor.consume(TokenType::L_PAREN, "Expected '(' after 'if'")) {
            cursor.synchronize();
            return nullptr;
        }

        auto cond = exprParser.parseExpr();
        if (cond == nullptr) {
            error::report("If needs a condition. Expected condition not found")
            cursor.synchronize();
            return nullptr;
        }

        if (!cursor.consume(TokenType::R_PAREN, "Expected ')' after 'if'")) {
            cursor.synchronize();
            return nullptr;
        }

        if (!cursor.check(TokenType::L_BRACES)) {
            error::report("Expected '{' before if body");
            cursor.synchronize();
            return nullptr;
        }

        auto thenStmt = parseBlock();

        if (cursor.match(TokenType::ELSE))
        {
            if (cursor.check(TokenType::IF)) 
            {
                auto nestedIf = parseIf();
                if (nestedIf == nullptr) {
                    error::report("Nested if expected not found, discarding else and putting normal if as fallback")
                    return std::make_unique<IfStmt>(std::move(cond), std::move(thenStmt));
                }

                auto elseBlock = std::make_unique<BlockStmt>();
                elseBlock->addStatement(std::move(nestedIf));
                return std::make_unique<IfStmt>(std::move(cond), std::move(thenStmt),std::move(elseBlock));
            }

            if (cursor.check(TokenType::L_BRACES))
                return std::make_unique<IfStmt>(std::move(cond), std::move(thenStmt), parseBlock());
            else
                error::report("Expected '{' or 'if' after else");
        }

        return std::make_unique<IfStmt>(std::move(cond), std::move(thenStmt));
    }

    ptr<ReturnStmt> parseReturn()
    {
        if (!cursor.consume(TokenType::RETURN, "Internal error consuming 'return' token"))
            return nullptr;

        auto expr = nullptr;

        if (cursor.check(TokenType::SEMICOLON))
            expr = exprParser.parseExpr();

        if (expr == nullptr && !isEnd() && peek().type != TokenType::SEMICOLON)
            cursor.synchronize();

        if (!consumeSemicolon())
            return nullptr;

        return std::make_unique<ReturnStmt>(std::move(expr));
    }

    ptr<ExprStmt> parseExprStmt()
    {
        auto expr = exprParser.parseExpr();

        if (expr == nullptr) {
            cursor.synchronize();
            return nullptr;
        }

        if (!consumeSemicolon()) {
            cursor.synchronize();
            return nullptr;
        }

        return std::make_unique<ExprStmt>(std::move(expr));
    }

    ptr<PrintStmt> parsePrint()
    {
        if (!cursor.consume(TokenType::PRINT, "Internal error consuming 'print' token"))
            return nullptr;

        if (!cursor.consume(TokenType::L_PAREN, "Expected '(' after 'print' statement"))
            return nullptr;

        auto expr = exprParser.parseExpr();

        if (expr == nullptr) {
            error::report("Expected expression at 'print' statement")
            cursor.synchronize();
            return nullptr;
        }

        if (!cursor.consume(TokenType::R_PAREN, "Expected 'R' after 'print' inner expression"))
            return nullptr;

        if (!consumeSemicolon())
            return nullptr;

        return std::make_unique<PrintStmt>(std::move(expr));
    }

public:
    explicit StatementParser(TokenCursor& cursor, ExpressionParser& exprParser) :
        cursor(cursor), exprParser(exprParser) { }

    ptr<Stmt> parseStmt() 
    {
        auto tk = cursor.peek();
        if (type_parser::isTypeToken(tk.type)) 
            return parseVariableDeclaration();
        if (tk.type == TokenType::IDENTIFYER &&  cursor.lookAhead().type == TokenType::EQUAL)
            return parseAssignment();
        if (tk.type == TokenType::L_BRACES)
            return parseBlock();
        if (tk.type == TokenType::IF)
            return parseIf();
        if (tk.type == TokenType::RETURN)
            return parseReturn();

        // DEBUG NODES
        if (tk.type == TokenType::PRINT)
            return parsePrint();

        return parseExprStmt();
    }

    ptr<BlockStmt> parseBlock()
    {
        if (!cursor.consume(TokenType::L_BRACES, "Expected '{' before block")) // consome '{'
            return nullptr;
        
        auto block = std::make_unique<BlockStmt>();

        while (!cursor.isAtEnd() && cursor.peek().type != TokenType::END_OF_FILE && cursor.peek().type != TokenType::R_BRACES)
        {
            auto stmt = parseStmt();
            if (stmt != nullptr) block->addStatement(std::move(stmt));
        }

        if (!cursor.match(TokenType::R_BRACES)) // consome '}' 
            error::report("Expected }");
        
        return block;
    }
};
