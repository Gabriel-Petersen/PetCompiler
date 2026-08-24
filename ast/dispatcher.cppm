export module ast.dispatcher;

import ast.node;
import ast.statements;
import ast.expressions;
import ast.debug_nodes;
import ast.visitor;

export class Dispatcher {
public:
    static void accept(Node& node, Visitor& visitor) 
    {
        switch (node.nodeType) 
        {
            case AstNodeType::Expr:
                visitor.visit(static_cast<ExprStmt&>(node));
                break;
            case AstNodeType::Block:
                visitor.visit(static_cast<BlockStmt&>(node));
                break;
            case AstNodeType::If:
                visitor.visit(static_cast<IfStmt&>(node));
                break;
            case AstNodeType::Return:
                visitor.visit(static_cast<ReturnStmt&>(node));
                break;
            case AstNodeType::VarDecl:
                visitor.visit(static_cast<VarDeclStmt&>(node));
                break;
            case AstNodeType::Assignment:
                visitor.visit(static_cast<AssignmentStmt&>(node));
                break;
            case AstNodeType::Literal:
                visitor.visit(static_cast<LiteralExpr&>(node));
                break;
            case AstNodeType::Unary:
                visitor.visit(static_cast<UnaryExpr&>(node));
                break;
            case AstNodeType::Var:
                visitor.visit(static_cast<VarExpr&>(node));
                break;
            case AstNodeType::Binary:
                visitor.visit(static_cast<BinaryExpr&>(node));
                break;
            case AstNodeType::Print:
                visitor.visit(static_cast<PrintStmt&>(node));
                break;
        }
    }
};
