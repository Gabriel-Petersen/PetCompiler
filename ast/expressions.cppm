module;

#include "stl.h"

export module ast.expressions;

import ast.node;
import types.info;
import parser.evaluation;

template<typename T>
using ptr = std::unique_ptr<T>;

export class LiteralExpr : public Expr 
{
    const Evaluation eval;
public:
    explicit LiteralExpr(Evaluation eval) : Expr(AstNodeType::Literal), eval(eval) { }

    [[nodiscard]] Evaluation getValue() const { return eval; }
};

export enum class UnaryExprType {
    MINUS, NOT
};

export class UnaryExpr : public Expr
{
    const UnaryExprType type;
    ptr<Expr> child;
public:
    explicit UnaryExpr(UnaryExprType tp, ptr<Expr> _child) : 
        Expr(AstNodeType::Unary), type(tp), child(std::move(_child)) { }

    [[nodiscard]] UnaryExprType getType() const { return type; }
    [[nodiscard]] Expr& getChild() { return *child; }
    [[nodiscard]] const Expr& getChild() const { return *child; }
};

export class VarExpr : public Expr
{
    const std::string name;
public:
    explicit VarExpr(std::string variable_name) : Expr(AstNodeType::Var), name(std::move(variable_name)) { }

    [[nodiscard]] const std::string& getVarName() const { return name; }
};

export enum class BinaryExprType {
    ADD, SUB, MUL, DIV, MOD,
    EQUALS, UNEQUALS, 
    GREATER, LESSER, GREATER_EQ, LESSER_EQ
};

export class BinaryExpr : public Expr
{
    ptr<Expr> left;
    const BinaryExprType type;
    ptr<Expr> right;
public:
    explicit BinaryExpr(ptr<Expr> _left, BinaryExprType _type, ptr<Expr> _right) :
        Expr(AstNodeType::Binary), left(std::move(_left)), type(_type), right(std::move(_right)) { }
    
    [[nodiscard]] BinaryExprType getType() const { return type; }
    [[nodiscard]] Expr& getLeftChild() { return *left; }
    [[nodiscard]] const Expr& getLeftChild() const { return *left; }
    [[nodiscard]] Expr& getRightChild() { return *right; }
    [[nodiscard]] const Expr& getRightChild() const { return *right; }
};
