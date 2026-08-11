export module semantic.visitor;

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

export class Visitor
{
public:
    virtual ~Visitor() = default;

    virtual void visit(BlockStmt* node) = 0;
    virtual void visit(VarDeclStmt* node) = 0;
    virtual void visit(AssignmentStmt* node) = 0;
    virtual void visit(PrintStmt* node) = 0;
    virtual void visit(ExprStmt* node) = 0;
    virtual void visit(ReturnStmt* node) = 0;
    virtual void visit(IfStmt* node) = 0;

    virtual void visit(BinaryExpr* node) = 0;
    virtual void visit(LiteralExpr* node) = 0;
    virtual void visit(VarExpr* node) = 0;
    virtual void visit(UnaryExpr* node) = 0;
};