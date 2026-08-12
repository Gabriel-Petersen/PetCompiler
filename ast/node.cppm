module;

#include <memory>
#include <utility>

export module ast:node;

import ast;
import ast.visitor;
import types.info;

export class Node
{
public:
    virtual ~Node() = default;

    virtual void accept(Visitor& visitor) = 0;
};

export class Expr : public Node
{
public:
    TypeInfo computedType;
};

export class Stmt : public Node
{
};

export class ExprStmt final : public Stmt
{
private:
    std::unique_ptr<Expr> expr;

public:
    explicit ExprStmt(std::unique_ptr<Expr> expression) : expr(std::move(expression)) { }

    void accept(Visitor& visitor) override { visitor.visit(*this); }

    [[nodiscard]] Expr& getExpr() { return *expr; }
    [[nodiscard]] const Expr& getExpr() const { return *expr; }
};