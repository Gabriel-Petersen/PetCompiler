#pragma once

#include "../compile_error/compile_error.h"
#include "../semantic_analyzer/visitor.h"
#include "../type_system/type_info.h"

namespace ast
{
    enum class EvaluationType {INT, FLOAT, BOOL};
    
    struct Evaluation {
        EvaluationType type;
        union {
            double dVal;
            long long iVal;
            bool bVal;
        } data;

        bool isTrue()
        {
            switch (type)
            {
            case EvaluationType::BOOL:
                return data.bVal;            
            case EvaluationType::INT:
                return data.iVal;
            case EvaluationType::FLOAT:
                return data.dVal;
            default:
                error::report("Unexpected EvaluationType: " + std::to_string(static_cast<int>(type)));
                return false;
                break;
            }
        }

        template<typename T>
        T getNumber() const
        {
            if (type == EvaluationType::INT) return (T)data.iVal;
            if (type == EvaluationType::FLOAT) return (T)data.dVal;
            return (T)data.bVal;
        }

        Evaluation(long long v) : type(EvaluationType::INT) { data.iVal = v; }
        Evaluation(double v) : type(EvaluationType::FLOAT) { data.dVal = v; }
        Evaluation(bool v) : type(EvaluationType::BOOL) { data.bVal = v; }
        Evaluation() : type(EvaluationType::INT) { data.iVal = 0; }
    };

    class Node {
    public:
        virtual void accept(Visitor* visitor) = 0;
    };

    class Expr : public Node 
    {
    public:
        types::TypeInfo computedType;
        virtual Evaluation evaluate() const = 0;
        virtual ~Expr() {};
    };

    class Stmt : public Node {
    public:
        virtual void execute() = 0;
        virtual ~Stmt() {};
    };

    class ExprStmt : public Stmt 
    {
    public:
        Expr* expr;
    
        ExprStmt(Expr* expr) : expr(expr) { }
        ~ExprStmt() { delete expr; }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        void execute() override { expr->evaluate(); }
    };
}