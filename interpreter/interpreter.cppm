module;

#include "stl.h"

export module interpreter;

import parser.evaluation;
import interpreter.runtime;
import ast;
import ast.node;
import ast.expressions;
import ast.statements;
import ast.debug_nodes;
import ast.visitor;
import ast.dispatcher;

class RuntimeScope
{
public:
    RuntimeScope() { runtime::enterScope(); }
    ~RuntimeScope() { runtime::exitScope(); }
    RuntimeScope(const RuntimeScope&) = delete;
    RuntimeScope& operator=(const RuntimeScope&) = delete;
};

class ReturnSignal
{
public:
    Evaluation value;
    explicit ReturnSignal(Evaluation value) : value(std::move(value)) { }
};

export class Interpreter final : public Visitor
{
    Evaluation lastEvaluation;

    [[nodiscard]] 
    static EvaluationType decideType(const Evaluation& left, const Evaluation& right, BinaryExprType operation)
    {
        if (operation == BinaryExprType::MOD)
            return EvaluationType::INT;

        if (left.type == EvaluationType::FLOAT || right.type == EvaluationType::FLOAT)
            return EvaluationType::FLOAT;

        return EvaluationType::INT;
    }
public:
    void execute(Stmt& statement) { Dispatcher::accept(statement, *this); }

    [[nodiscard]] Evaluation evaluate(Expr& expression)
    {
        Dispatcher::accept(expression, *this);
        return lastEvaluation;
    }

    [[nodiscard]] Evaluation interpret(AstTree& tree)
    {
        runtime::clear();
        lastEvaluation = Evaluation();

        try {
            execute(tree.getRoot());
        }
        catch (const ReturnSignal& signal) {
            return signal.value;
        }

        return Evaluation(0LL);
    }

    void visit(LiteralExpr& node) override { lastEvaluation = node.getValue(); }
    void visit(VarExpr& node) override { lastEvaluation = runtime::get(node.getVarName()); }

    void visit(UnaryExpr& node) override
    {
        Evaluation child = evaluate(node.getChild());

        switch (node.getType())
        {
        case UnaryExprType::NOT:
            if (child.type == EvaluationType::BOOL)
            {
                lastEvaluation = Evaluation(!child.data.bVal);
                return;
            }

            std::cout << "[RUNTIME-ERROR]: Operacao ! requer booleano.\n";
            lastEvaluation = Evaluation();
            return;

        case UnaryExprType::MINUS:
            if (child.type == EvaluationType::FLOAT)
            {
                lastEvaluation = Evaluation(-child.data.dVal);
                return;
            }

            if (child.type == EvaluationType::INT)
            {
                lastEvaluation = Evaluation(-child.data.iVal);
                return;
            }

            std::cout << "[RUNTIME-ERROR]: Operacao - requer numero.\n";
            lastEvaluation = Evaluation();
            return;
        }
    }

    void visit(BinaryExpr& node) override
    {
        Evaluation left = evaluate(node.getLeftChild());
        Evaluation right = evaluate(node.getRightChild());

        const BinaryExprType operation = node.getType();
        const bool leftIsBool = left.type == EvaluationType::BOOL;
        const bool rightIsBool = right.type == EvaluationType::BOOL;

        if (leftIsBool != rightIsBool)
        {
            std::cout << "[RUNTIME-ERROR]: Operacao entre number e bool.\n";
            lastEvaluation = Evaluation{};
            return;
        }

        if (leftIsBool)
        {
            switch (operation)
            {
            case BinaryExprType::EQUALS:
                lastEvaluation = Evaluation(left.data.bVal == right.data.bVal);
                return;

            case BinaryExprType::UNEQUALS:
                lastEvaluation = Evaluation(left.data.bVal != right.data.bVal);
                return;

            default:
                std::cout << "[RUNTIME-ERROR]: Operacao nao suportada " << std::static_cast<int>(operation) << "entre booleanos.\n";
                lastEvaluation = Evaluation();
                return;
            }
        }

        const EvaluationType resultType = decideType(left, right, operation);

        switch (operation)
        {
        case BinaryExprType::ADD:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() + right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() + right.getNumber<long long>());
            
            return;

        case BinaryExprType::SUB:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() - right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() - right.getNumber<long long>());
            
            return;

        case BinaryExprType::MUL:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() * right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() * right.getNumber<long long>());
            
            return;

        case BinaryExprType::DIV:
            if (resultType == EvaluationType::FLOAT)
            {
                const double divisor = right.getNumber<double>();

                if (divisor == 0.0)
                {
                    std::cout << "[RUNTIME-ERROR]: Divisao por zero.\n";
                    lastEvaluation = Evaluation{};
                    return;
                }

                lastEvaluation = Evaluation(left.getNumber<double>() / divisor);
            }
            else
            {
                const long long divisor = right.getNumber<long long>();

                if (divisor == 0)
                {
                    std::cout << "[RUNTIME-ERROR]: Divisao por zero.\n";
                    lastEvaluation = Evaluation{};
                    return;
                }

                lastEvaluation = Evaluation(left.getNumber<long long>() / divisor);
            }

            return;

        case BinaryExprType::MOD:
        {
            const long long divisor = right.getNumber<long long>();

            if (divisor == 0)
            {
                std::cout << "[RUNTIME-ERROR]: Resto por zero.\n";
                lastEvaluation = Evaluation{};
                return;
            }

            lastEvaluation = Evaluation(left.getNumber<long long>() % divisor);
            return;
        }

        case BinaryExprType::EQUALS:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() == right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() == right.getNumber<long long>());

            return;

        case BinaryExprType::UNEQUALS:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() != right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() != right.getNumber<long long>());

            return;

        case BinaryExprType::GREATER:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() > right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() > right.getNumber<long long>());

            return;

        case BinaryExprType::LESSER:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() < right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() < right.getNumber<long long>());

            return;

        case BinaryExprType::GREATER_EQ:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() >= right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() >= right.getNumber<long long>());

            return;

        case BinaryExprType::LESSER_EQ:
            if (resultType == EvaluationType::FLOAT)
                lastEvaluation = Evaluation(left.getNumber<double>() <= right.getNumber<double>());
            else
                lastEvaluation = Evaluation(left.getNumber<long long>() <= right.getNumber<long long>());
            
            return;
        }
    }

    void visit(ExprStmt& node) override { 
        static_cast<void>(evaluate(node.getExpr()));
    }

    void visit(PrintStmt& node) override
    {
        const Evaluation value = evaluate(node.getExpr());

        if (value.type == EvaluationType::BOOL) {
            std::cout << (value.data.bVal ? "true" : "false") << '\n';
            return;
        }

        if (value.type == EvaluationType::INT) {
            std::cout << value.data.iVal << '\n';
            return;
        }

        std::cout << value.data.dVal << '\n';
    }

    void visit(BlockStmt& node) override
    {
        RuntimeScope scope;

        for (auto& statement : node.getStatements())
        {
            if (statement)
                execute(*statement);
        }
    }

    void visit(IfStmt& node) override
    {
        const Evaluation condition =
            evaluate(node.getCondition());

        if (condition.isTrue())
        {
            execute(node.getThenBlock());
            return;
        }

        if (BlockStmt* elseBlock = node.getElseBlock())
            execute(*elseBlock);
    }

    void visit(ReturnStmt& node) override {
        throw ReturnSignal{evaluate(node.getExpr())};
    }

    void visit(VarDeclStmt& node) override
    {
        Evaluation value;

        if (Expr* initializer = node.getInitializer())
            value = evaluate(*initializer);

        runtime::define(node.identifier, node.type, std::move(value));
    }

    void visit(AssignmentStmt& node) override {
        runtime::assign(node.identifier, evaluate(node.getValue()));
    }
};
