#pragma once
#include "statement_nodes.h"

namespace ast
{
    class AstTree {
    public:
        BlockStmt* root;

        AstTree() : root(new BlockStmt({})) {}
        ~AstTree() { delete root; }

        void addStmt(Stmt* stmt) { root->addStmt(stmt); }

        Evaluation interpret() 
        {
            try {
                if (root) 
                    root->execute();
            } 
            catch (const ReturnSignal& sig) {
                return sig.value;
            }
            return Evaluation((long long)0);
        }
    };
}