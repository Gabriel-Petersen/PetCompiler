module;

#include <memory>

export module ast;

export class VarDeclStmt;
export class AssignmentStmt;
export class PrintStmt;
export class BlockStmt;
export class ReturnStmt;
export class IfStmt;
export class ExprStmt;

export class BinaryExpr;
export class LiteralExpr;
export class VarExpr;
export class UnaryExpr;

export class AstTree
{
    std::unique_ptr<BlockStmt> root;
public:
    AstTree() : root(std::make_unique<BlockStmt>()) { }

    [[nodiscard]] BlockStmt& getRoot() { return *root; }
    [[nodiscard]] const BlockStmt& getRoot() const { return *root; }

    void addStmt(ptr<Stmt> stmt) { root->addStmt(std::move(stmt)); }
};
