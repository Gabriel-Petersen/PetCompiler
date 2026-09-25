module;

#include <memory>
#include <string>
#include <utility>
#include <unordered_map>

export module parser.statements;

import token;
import parser.token_cursor;
import parser.expressions;
import parser.types;
import error;
import ast.node;
import ast.statements;
import ast.expressions;
import ast.debug_nodes;
import types.registry;

template<typename T>
using ptr = std::unique_ptr<T>;

export class StatementParser {
private:
    TokenCursor& cursor;
    TypeRegistry& registry;
    ExpressionParser& exprParser;
    std::unordered_map<TokenType, AssignmentOperation> assignmentTokens;

    [[nodiscard]] bool consumeSemicolon()
    {
        if (cursor.match(TokenType::SEMICOLON)) return true;
        error::report("Semicolon ';' expected");
        return false;
    }

    [[nodiscard]] ptr<VarDeclStmt> parseVariableDeclaration()
    {
        const Token& typeToken = cursor.advance();
        auto typeInfo = type_parser::parse(typeToken, registry);

        if (!cursor.check(TokenType::IDENTIFYER))
        {
            error::report("Expected identifyer after type declaration");
            cursor.synchronize();
            return nullptr;
        }

        std::string identifier = cursor.advance().src;
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

        return std::make_unique<VarDeclStmt>(std::move(identifier), typeInfo, std::move(initializer));
    }

    [[nodiscard]] ptr<AssignmentStmt> parseAssignment()
    {
        const Token& identifierToken = cursor.advance();
        auto target = std::make_unique<VarExpr>(identifierToken.src);

        const auto operationIt = assignmentTokens.find(cursor.peek().type);
        if (operationIt == assignmentTokens.end())
        {
            error::report("Internal parser error: expected assignment operation");
            cursor.synchronize();
            return nullptr;
        }

        cursor.advance();

        auto val = exprParser.parseExpr();
        if (val == nullptr) {
            cursor.synchronize();
            return nullptr;
        }
            
        if (!consumeSemicolon()) {
            cursor.synchronize();
            return nullptr;
        }

        return std::make_unique<AssignmentStmt>(std::move(target), operationIt->second, std::move(val));
    }

    [[nodiscard]] ptr<IfStmt> parseIf()
    {
        cursor.advance(); // Pula 'if'
                
        if (!cursor.consume(TokenType::L_PAREN, "Expected '(' after 'if'")) {
            cursor.synchronize();
            return nullptr;
        }

        auto cond = exprParser.parseExpr();
        if (cond == nullptr) 
        {
            error::report("If needs a condition. Expected condition not found");
            cursor.synchronize();
            return nullptr;
        }

        if (!cursor.consume(TokenType::R_PAREN, "Expected ')' after 'if'")) {
            cursor.synchronize();
            return nullptr;
        }

        if (!cursor.check(TokenType::L_BRACES)) 
        {
            error::report("Expected '{' before if body");
            cursor.synchronize();
            return nullptr;
        }

        auto thenStmt = parseBlock();
        if (thenStmt == nullptr) return nullptr;

        if (cursor.match(TokenType::ELSE))
        {
            if (cursor.check(TokenType::IF)) 
            {
                auto nestedIf = parseIf();
                if (nestedIf == nullptr) {
                    error::report("Invalid nested if after 'else'");
                    return nullptr;
                }

                auto elseBlock = std::make_unique<BlockStmt>();
                elseBlock->addStatement(std::move(nestedIf));
                return std::make_unique<IfStmt>(std::move(cond), std::move(thenStmt),std::move(elseBlock));
            }

            if (cursor.check(TokenType::L_BRACES))
            {
                auto elseBlock = parseBlock();
                if (elseBlock == nullptr) return nullptr;
                return std::make_unique<IfStmt>(std::move(cond), std::move(thenStmt), std::move(elseBlock));
            }
            else
            {
                error::report("Expected '{' or 'if' after else");
                cursor.synchronize();
                return nullptr;
            }
        }

        return std::make_unique<IfStmt>(std::move(cond), std::move(thenStmt));
    }

    [[nodiscard]] ptr<ReturnStmt> parseReturn()
    {
        if (!cursor.consume(TokenType::RETURN, "Internal error consuming 'return' token"))
            return nullptr;

        ptr<Expr> expr = nullptr;

        if (!cursor.check(TokenType::SEMICOLON))
        {
            expr = exprParser.parseExpr();
            if (expr == nullptr) {
                cursor.synchronize();
                return nullptr;
            }
        }

        if (!consumeSemicolon()) {
            cursor.synchronize();
            return nullptr;
        }

        return std::make_unique<ReturnStmt>(std::move(expr));
    }

    [[nodiscard]] ptr<ExprStmt> parseExprStmt()
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

    [[nodiscard]] ptr<PrintStmt> parsePrint()
    {
        if (!cursor.consume(TokenType::PRINT, "Internal error consuming 'print' token"))
            return nullptr;

        if (!cursor.consume(TokenType::L_PAREN, "Expected '(' after 'print' statement")) {
            cursor.synchronize();
            return nullptr;
        }

        auto expr = exprParser.parseExpr();

        if (expr == nullptr) {
            error::report("Expected expression at 'print' statement");
            cursor.synchronize();
            return nullptr;
        }

        if (!cursor.consume(TokenType::R_PAREN, "Expected ')' after 'print' expression")) {
            cursor.synchronize();
            return nullptr;
        }

        if (!consumeSemicolon()) {
            cursor.synchronize();
            return nullptr;
        }

        return std::make_unique<PrintStmt>(std::move(expr));
    }

public:
    explicit StatementParser(TokenCursor& cursor, TypeRegistry& registry, ExpressionParser& exprParser) :
        cursor(cursor), registry(registry), exprParser(exprParser) { 
            assignmentTokens[TokenType::EQUAL] = AssignmentOperation::ASSIGN;
            assignmentTokens[TokenType::PLUS_EQ] = AssignmentOperation::ADD_ASSIGN;
            assignmentTokens[TokenType::MINUS_EQ] = AssignmentOperation::SUB_ASSIGN;
            assignmentTokens[TokenType::STAR_EQ] = AssignmentOperation::MUL_ASSIGN;
            assignmentTokens[TokenType::SLASH_EQ] = AssignmentOperation::DIV_ASSIGN;
            assignmentTokens[TokenType::MOD_EQ] = AssignmentOperation::MOD_ASSIGN;
        }

    [[nodiscard]] ptr<Stmt> parseStmt() 
    {
        const Token& tk = cursor.peek();
        if (type_parser::isPrimitiveTypeToken(tk.type)) 
            return parseVariableDeclaration();
        if (tk.type == TokenType::IDENTIFYER && assignmentTokens.find(cursor.lookAhead().type) != assignmentTokens.end())
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

    [[nodiscard]] ptr<BlockStmt> parseBlock()
    {
        if (!cursor.consume(TokenType::L_BRACES, "Expected '{' before block")) // consome '{'
            return nullptr;
        
        auto block = std::make_unique<BlockStmt>();

        while (!cursor.isAtEnd() && !cursor.check(TokenType::R_BRACES))
        {
            const std::size_t initialPosition = cursor.position();
            auto stmt = parseStmt();

            if (stmt != nullptr) 
                block->addStatement(std::move(stmt));
            else if (cursor.position() == initialPosition) {
                error::report("Parser failed without consuming any token");
                cursor.advance();
            }
        }

        if (!cursor.match(TokenType::R_BRACES)) { // consome '}' 
            error::report("Expected '}' after block");
            return nullptr;
        }
        
        return block;
    }
};
