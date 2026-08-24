module;

#include "stl.h"

export module ast.debug_nodes;

import ast.node;

export class PrintStmt : public Stmt
{
    std::unique_ptr<Expr> expr;
public:
    explicit PrintStmt(std::unique_ptr<Expr> _expr) : Stmt(AstNodeType::Print), expr(std::move(_expr)) { }

    [[nodiscard]] Expr& getExpr() { return *expr; }
    [[nodiscard]] const Expr& getExpr() const { return *expr; }
};
