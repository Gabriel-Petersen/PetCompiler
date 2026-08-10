#pragma once
#include "tree_node.h"
#include "../type_system/type_info.h"
#include "../parser/runtime.h"
#include "../semantic_analyzer/visitor.h"
#include <vector>

namespace ast
{
    class BlockStmt : public Stmt
    {
    public:
        std::vector<Stmt*> expressions;

        BlockStmt(std::initializer_list<Stmt*> expr) : expressions(expr) { }
        BlockStmt(std::vector<Stmt*> expr) : expressions(std::move(expr)) { }
        BlockStmt() { }

        ~BlockStmt() 
        {
            for (auto& stmt : expressions) 
                if (stmt != nullptr)
                    delete stmt;
        }

        void addStmt(Stmt* stmt) { expressions.push_back(stmt); }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        void execute() override
        {
            runtime::enterScope();
            for (auto& stmt : expressions) 
                if (stmt != nullptr) stmt->execute();
            runtime::exitScope();
        }

        bool isEmpty() { return expressions.empty(); }
    };

    class IfStmt : public Stmt
    {
    public:
        Expr* condition;
        BlockStmt* thenBlock;
        BlockStmt* elseBlock;

    
        IfStmt(Expr* condition, BlockStmt* thenBlock, BlockStmt* elseBlock) :
            condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) { }
        IfStmt(Expr* condition, BlockStmt* thenBlock) :
            condition(condition), thenBlock(thenBlock), elseBlock(nullptr) { }

        ~IfStmt()
        {
            delete condition;
            delete thenBlock;
            delete elseBlock;
        }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        void execute() override
        {
            if (condition->evaluate().isTrue())
                thenBlock->execute();
            else if (elseBlock != nullptr)
                elseBlock->execute();
        }
    };

    struct ReturnSignal {
        Evaluation value;
        ReturnSignal(Evaluation v) : value(v) {}
    };

    class ReturnStmt : public Stmt
    {
    public:
        Expr* expression;
    
        ReturnStmt(Expr* expression) : expression(expression) { }

        ~ReturnStmt() { delete expression; }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        void execute() override {
            Evaluation res = (expression != nullptr) ? expression->evaluate() : Evaluation();
            throw ReturnSignal(res);
        }
    };

    class VarDeclStmt : public Stmt
    {
    public:
        Expr* initializer;
    
        const types::TypeInfo type;
        const std::string identifyer;

        VarDeclStmt(types::TypeInfo type, std::string identifyer, Expr* initializer = nullptr) :
            initializer(initializer), type(type), identifyer(identifyer) { }
        
        ~VarDeclStmt() {
            if (initializer != nullptr) delete initializer;
        }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        void execute() override 
        {
            Evaluation val = (initializer) ? initializer->evaluate() : Evaluation();
            runtime::define(identifyer, type, val);
        }
    };

    class AssignmentStmt : public Stmt 
    {
    public:
        std::string name;
        Expr* value;
    
        AssignmentStmt(std::string name, Expr* value) : name(name), value(value) { }
        void execute() override {
            Evaluation res = value->evaluate();
            runtime::define(name, runtime::variablePool[name].type, res, false); 
        }
        
        void accept(Visitor* visitor) override { visitor->visit(this); }
    };
}