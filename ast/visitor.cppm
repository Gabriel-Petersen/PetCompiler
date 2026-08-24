export module ast.visitor;

import ast.node;
import ast.statements;
import ast.expressions;

import ast.debug_nodes;

export class Visitor
{
public:
    virtual ~Visitor() = default;

    virtual void visit(BlockStmt& node) = 0;
    virtual void visit(VarDeclStmt& node) = 0;
    virtual void visit(AssignmentStmt& node) = 0;
    virtual void visit(PrintStmt& node) = 0;
    virtual void visit(ExprStmt& node) = 0;
    virtual void visit(ReturnStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;

    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(LiteralExpr& node) = 0;
    virtual void visit(VarExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
};