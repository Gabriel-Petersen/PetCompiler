module;

#include <memory>
#include <utility>

export module ast:debug_nodes;

import ast;
import ast.visitor;
import ast:node;

export class PrintStmt : public Stmt
{
    std::unique_ptr<Expr> expr;
public:
    explicit PrintStmt(std::unique_ptr<Expr> _expr) : expr(std::move(_expr)) { }
    void accept(Visitor& visitor) override { visitor.visit(*this); }

    [[nodiscard]] Expr& getExpr() { return *expr; }
    [[nodiscard]] const Expr& getExpr() const { return *expr; }
};
