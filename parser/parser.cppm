module;

#include <memory>
#include <utility>

export module parser;

import error;
import lexer;
import parser.token_cursor;
import parser.expressions;
import parser.statements;
import ast;

export class Parser {
private:
    bool ownsAst = true;
    bool isBuilt = false;
    TokenCursor cursor;
    ExpressionParser exprParser;
    StatementParser stmtParser;
    std::unique_ptr<AstTree> ast;

public:
    explicit Parser(Lexer& lx) : 
        cursor(TokenCursor(lx.getAllToken())), exprParser(cursor), stmtParser(cursor, exprParser), ast(std::make_unique<AstTree>()) { }

    void build()
    {
        if (isBuilt) return;

        while (!cursor.isAtEnd()) {
            auto stmt = stmtParser.parseStmt();
            if (stmt != nullptr) ast->addStmt(std::move(stmt));
        }
        
        isBuilt = true;
    }

    std::unique_ptr<AstTree> takeAst() 
    { 
        if (!ownsAst) {
            error::report("Trying to take ownership of AST from Parser multiple times; Parser no longer owns AST");
            return nullptr;
        }

        if (!isBuilt) build();
        ownsAst = false;
        return std::move(ast);
    }
};
