module;

#include "stl.h"

export module ast.node;

import types.info;

export enum class AstNodeType {
    Expr, Block, If, Return, VarDecl, Assignment,
    Literal, Unary, Var, Binary,
    Print
};

export class Node
{
public:
    const AstNodeType nodeType;
    explicit Node(AstNodeType type) : nodeType(type) { }

    virtual ~Node() = default;
};

export class Expr : public Node
{
public:
    TypeInfo computedType;
    explicit Expr(AstNodeType type) : Node(type) { }
};

export class Stmt : public Node
{
public:
    explicit Stmt(AstNodeType type) : Node(type) { }
};

export class ExprStmt final : public Stmt
{
private:
    std::unique_ptr<Expr> expr;

public:
    explicit ExprStmt(std::unique_ptr<Expr> expression) : Stmt(AstNodeType::Expr), expr(std::move(expression)) { }

    [[nodiscard]] Expr& getExpr() { return *expr; }
    [[nodiscard]] const Expr& getExpr() const { return *expr; }
};