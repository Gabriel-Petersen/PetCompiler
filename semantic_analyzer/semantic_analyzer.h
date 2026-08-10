#pragma once

#include <memory>
#include "../ast/tree_node.h"
#include "../ast/statement_nodes.h"
#include "../ast/debug_nodes.h"
#include "../ast/expression_nodes.h"
#include "../type_system/symbol_table.h"
#include "../type_system/casts.h"

namespace analyzer
{
    using namespace ast;
    class SemanticAnalyzer : public Visitor
    {
        std::shared_ptr<types::SymbolTable> currentTable;

    public:
        
        SemanticAnalyzer() { currentTable = std::make_shared<types::SymbolTable>(); }

        void visit(BlockStmt* node) override
        {
            auto previous = currentTable;
            currentTable = std::make_shared<types::SymbolTable>(previous);

            for (auto stmt : node->expressions)
                if (stmt != nullptr) stmt->accept(this);

            currentTable = previous;
        }

        void visit(VarDeclStmt* node) override
        {
            currentTable->define(node->identifyer, node->type);
            if (node->initializer != nullptr) node->initializer->accept(this);
        }

        void visit(AssignmentStmt* node) override
        {
            auto var_type = currentTable->lookup(node->name);
            if (var_type == nullptr) {
                error::report("Compilation Error: Variable '" + node->name + "' not defined at this scope");
                return;
            }

            if (node->value != nullptr) 
                node->value->accept(this);

            auto severity = types::getCastSeverity(node->value->computedType, *var_type);

            if (severity == types::CastSeverity::IMPOSSIBLE)
                error::report("Compilation Error: Automatic cast to '" + node->name + "' is impossible");
            else if (severity == types::CastSeverity::DEMOTION) 
                std::cout << "[COMPILATION-WARN]: Implicit cast detected at '" << node->name << "'. May occur data loss.\n";
        }

        void visit(PrintStmt* node) override {
            if (node->target != nullptr) node->target->accept(this);
        }

        void visit(ExprStmt* node) override {
            if (node->expr != nullptr) node->expr->accept(this);
        }

        void visit(ReturnStmt* node) override {
            if (node->expression != nullptr) node->expression->accept(this);
        }

        void visit(IfStmt* node) override 
        {
            if (node->condition != nullptr) node->condition->accept(this);
            if (node->thenBlock != nullptr) node->thenBlock->accept(this);
            if (node->elseBlock != nullptr) node->elseBlock->accept(this);
        }

        void visit(BinaryExpr* node) override
        {
            if (node->left != nullptr) node->left->accept(this);
            if (node->right != nullptr) node->right->accept(this);

            if (node->left->computedType.isBool() != node->right->computedType.isBool())
                error::report("Compilation Error: Invalid operation between boolean and numeric type");
            
            node->computedType = types::decideStaticType(node->left->computedType, node->right->computedType);
        }

        void visit(LiteralExpr* node) override 
        { 
            auto val = node->evaluate();
            if (val.type == EvaluationType::BOOL)
                node->computedType = (types::TypeInfo){types::TypeKind::BOOL};
            else if (val.type == EvaluationType::FLOAT)
                node->computedType = (types::TypeInfo){types::TypeKind::DOUBLE};
            else
                node->computedType = (types::TypeInfo){types::TypeKind::LONG};
        }

        void visit(VarExpr* node) override
        {
            auto var_type = currentTable->lookup(node->name);
            if (var_type == nullptr)
                error::report("Compilation Error: Variable '" + node->name + "' not defined at this scope");
            else 
                node->computedType = *var_type;
        }

        void visit(UnaryExpr* node) override 
        {
            if (node->child == nullptr) return;
            
            node->child->accept(this);
            auto child_type = node->child->computedType;

            if (node->type == UnaryExprType::NOT)
            {
                if (!child_type.isBool())
                    error::report("Compolation Error: operator '!' cannot be applyed to not boolean types");
                node->computedType = child_type;
            }
            else
            {
                if (child_type.isBool())
                    error::report("Compilation Error: operator '-' cannot be applyed to boolean type");
                
                if (child_type.isUnsigned())
                    std::cout << "[COMPILATION-WARN]: operator '-' applyed to unsigned type may cause unexpected results. It will overflow due to two's complement.\n";

                node->computedType = child_type;
            }
        }
    };
}