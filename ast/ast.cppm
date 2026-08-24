module;

#include "stl.h"

export module ast;

import ast.node;
import ast.statements;

export class AstTree
{
    std::unique_ptr<BlockStmt> root;
public:
    AstTree() : root(std::make_unique<BlockStmt>()) { }

    [[nodiscard]] BlockStmt& getRoot() { return *root; }
    [[nodiscard]] const BlockStmt& getRoot() const { return *root; }

    void addStmt(std::unique_ptr<Stmt> stmt) { root->addStatement(std::move(stmt)); }
};

