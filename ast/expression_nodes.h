#pragma once
#include "tree_node.h"
#include "../compile_error/compile_error.h"
#include "../parser/runtime.h"

namespace ast
{
    class LiteralExpr : public Expr 
    {
        const Evaluation eval;
    public:
        LiteralExpr(Evaluation eval) : eval(eval) { }
        void accept(Visitor* visitor) override { visitor->visit(this); }
        Evaluation evaluate() const override { return eval; }
    };

    enum class UnaryExprType {
        MINUS, NOT
    };

    class UnaryExpr : public Expr 
    {
    public:
        const UnaryExprType type;
        Expr* child;
    
        UnaryExpr(UnaryExprType tp, Expr* child) : type(tp), child(child) { }

        ~UnaryExpr() { delete child; }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        Evaluation evaluate() const override
        {
            Evaluation childValue = child->evaluate();
            if (childValue.type == EvaluationType::BOOL)
            {
                if (type == UnaryExprType::NOT)
                    return Evaluation(!childValue.data.bVal);
                else
                    error::report("Operacao invalida: -[BOOL_LITERAL]");
            }
            else
            {
                if (type == UnaryExprType::MINUS)
                {
                    if (childValue.type == EvaluationType::FLOAT)
                        return Evaluation(-childValue.data.dVal);
                    else if (childValue.type == EvaluationType::INT)
                        return Evaluation(-childValue.data.iVal);
                    else
                        error::report("Operacao invalida: ![NUM_LITERAL]");
                }
            }
            return Evaluation();
        }
    };

    class VarExpr : public Expr
    {
    public:
        std::string name;
    
        VarExpr(std::string variableName) : name(variableName) { }
        Evaluation evaluate() const override { return runtime::get(name); }
        void accept(Visitor* visitor) override { visitor->visit(this); }
    };

    enum BinaryExprType {
            ADD, SUB, MUL, DIV, MOD,
            EQUALS, UNEQUALS, 
            GREATER, LESSER, GREATER_EQ, LESSER_EQ
        };

    class BinaryExpr : public Expr
    {
        static EvaluationType decideType (Evaluation l, Evaluation r, BinaryExprType type)
        {
            if (type == BinaryExprType::MOD) return EvaluationType::INT;
            if (l.type == EvaluationType::FLOAT || r.type == EvaluationType::FLOAT)
                return EvaluationType::FLOAT;
            return EvaluationType::INT;
        }
        
    public:
        Expr* left;
        const BinaryExprType type;
        Expr* right;

        BinaryExpr(Expr* l, BinaryExprType type, Expr* r) : left(l), type(type), right(r) { }

        ~BinaryExpr()
        {
            delete left;
            delete right;
        }

        void accept(Visitor* visitor) override { visitor->visit(this); }

        Evaluation evaluate() const override
        {
            if (!left || !right) return Evaluation();
            Evaluation l = left->evaluate();
            Evaluation r = right->evaluate();

            if ((l.type == EvaluationType::BOOL) == (r.type == EvaluationType::BOOL))
            {
                if (l.type == EvaluationType::BOOL)
                {
                    if (type == BinaryExprType::EQUALS)
                        return Evaluation(l.data.bVal == r.data.bVal);
                    else if (type == BinaryExprType::UNEQUALS)
                        return Evaluation(l.data.bVal != r.data.bVal);
                    else
                        error::report("Operacao nao suportada entre booleanos: " + std::to_string(type));
                }
                else
                {
                    auto evTp = decideType(l, r, type);
                    switch (type)
                    {
                    case BinaryExprType::ADD:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() + r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() + r.getNumber<long long>());

                    case BinaryExprType::SUB:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() - r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() - r.getNumber<long long>());

                    case BinaryExprType::MUL:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() * r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() * r.getNumber<long long>());

                    case BinaryExprType::DIV:
                        if (evTp == EvaluationType::FLOAT) 
                        {
                            if (r.getNumber<double>() == 0) 
                                error::report("Divisao por zero nao permitida");
                            else 
                                return Evaluation(l.getNumber<double>() / r.getNumber<double>());
                        }
                        else
                        {
                            if (r.getNumber<double>() == 0)
                                error::report("Divisao por zero nao permitida");
                            else 
                                return Evaluation(l.getNumber<long long>() / r.getNumber<long long>());
                        }
                        break;
                    case BinaryExprType::MOD:
                        if (evTp == EvaluationType::FLOAT)
                            error::report("Operacao de resto (MOD) entre floats nao permitida");
                        else
                            return Evaluation(l.getNumber<long long>() % r.getNumber<long long>());
                        break;
                    case BinaryExprType::EQUALS:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() == r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() == r.getNumber<long long>());

                    case BinaryExprType::UNEQUALS:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() != r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() != r.getNumber<long long>());

                    case BinaryExprType::GREATER:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() > r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() > r.getNumber<long long>());

                    case BinaryExprType::GREATER_EQ:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() >= r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() >= r.getNumber<long long>());

                    case BinaryExprType::LESSER:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() < r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() < r.getNumber<long long>());

                    case BinaryExprType::LESSER_EQ:
                        if (evTp == EvaluationType::FLOAT)
                            return Evaluation(l.getNumber<double>() <= r.getNumber<double>());
                        else
                            return Evaluation(l.getNumber<long long>() <= r.getNumber<long long>());
                    }
                }
            }
            else
                error::report("Operacao nao suportada entre number e bool");
            return Evaluation();
        }
    };
}