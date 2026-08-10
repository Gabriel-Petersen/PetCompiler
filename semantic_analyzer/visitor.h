#pragma once

namespace ast
{
    class VarDeclStmt; class AssignmentStmt; class PrintStmt; class BlockStmt; class ReturnStmt; class IfStmt; class ExprStmt;
    class BinaryExpr; class LiteralExpr; class VarExpr; class UnaryExpr;

    class Visitor 
    {
    public:
        virtual ~Visitor() = default;

        // Visitas para Statements (comandos)
        virtual void visit(BlockStmt* node) = 0;
        virtual void visit(VarDeclStmt* node) = 0;
        virtual void visit(AssignmentStmt* node) = 0;
        virtual void visit(PrintStmt* node) = 0;
        virtual void visit(ExprStmt* node) = 0;
        virtual void visit(ReturnStmt* node) = 0;
        virtual void visit(IfStmt* node) = 0;

        // Visitas para Expressions (valores)
        virtual void visit(BinaryExpr* node) = 0;
        virtual void visit(LiteralExpr* node) = 0;
        virtual void visit(VarExpr* node) = 0;
        virtual void visit(UnaryExpr* node) = 0;
    };
}