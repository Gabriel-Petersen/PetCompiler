#pragma once
#include "tree_node.h"
#include <iostream>

namespace ast
{
    class PrintStmt : public Stmt
    {
    public:
        Expr* target;
    
        PrintStmt(Expr* target) : target(target) { }
        ~PrintStmt() { delete target; }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        void execute() override 
        { 
            auto ev = target->evaluate();
            if (ev.type == EvaluationType::BOOL)
                std::cout << (ev.data.bVal ? "true" : "false") << "\n";
            else
                std::cout << ev.getNumber<double>() << "\n";
        }
    };
}