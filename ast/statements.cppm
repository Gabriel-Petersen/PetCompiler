module;

#include "stl.h"

export module ast.statements;

import ast.node;
import types.info;

template<typename T>
using ptr = std::unique_ptr<T>;

export class BlockStmt : public Stmt
{
private:
    std::vector<ptr<Stmt>> statements;
public:
    explicit BlockStmt() : Stmt(AstNodeType::Block) { }
    explicit BlockStmt(std::vector<ptr<Stmt>> _stmts) : Stmt(AstNodeType::Block), statements(std::move(_stmts)) { }

    void addStatement(ptr<Stmt> stmt) { statements.push_back(std::move(stmt)); }

    [[nodiscard]] const std::vector<ptr<Stmt>>& getStatements() const { return statements; }
    [[nodiscard]] std::vector<ptr<Stmt>>& getStatements() { return statements; }
};

export class IfStmt : public Stmt
{
private:
    ptr<Expr> condition;
    ptr<BlockStmt> thenBlock;
    ptr<BlockStmt> elseBlock;

public: 
    explicit IfStmt(ptr<Expr> _cond, ptr<BlockStmt> _then, ptr<BlockStmt> _else = nullptr) : 
        Stmt(AstNodeType::If), condition(std::move(_cond)), thenBlock(std::move(_then)), elseBlock(std::move(_else)) { }

    [[nodiscard]] const Expr& getCondition() const { return *condition; }
    [[nodiscard]] Expr& getCondition() { return *condition; }
    [[nodiscard]] const BlockStmt& getThenBlock() const { return *thenBlock; }
    [[nodiscard]] BlockStmt& getThenBlock() { return *thenBlock; }
    [[nodiscard]] const BlockStmt* getElseBlock() const { return elseBlock.get(); }
    [[nodiscard]] BlockStmt* getElseBlock() { return elseBlock.get(); }
};

export class ReturnStmt : public Stmt
{
private:
    ptr<Expr> expression;
public:
    explicit ReturnStmt(ptr<Expr> expr) : Stmt(AstNodeType::Return), expression(std::move(expr)) { }

    [[nodiscard]] Expr& getExpr() { return *expression; }
    [[nodiscard]] const Expr& getExpr() const { return *expression; }
};

export class VarDeclStmt : public Stmt
{
private:
    ptr<Expr> initializer;
    
public:
    const std::string identifier;
    const TypeInfo type;

    explicit VarDeclStmt(std::string _ident, const TypeInfo _type, ptr<Expr> init = nullptr) :
        Stmt(AstNodeType::VarDecl), initializer(std::move(init)), identifier(std::move(_ident)), type(_type) { }
    
    [[nodiscard]] const Expr* getInitializer() const { return initializer.get(); }
    [[nodiscard]] Expr* getInitializer() { return initializer.get(); }
};

export class AssignmentStmt : public Stmt
{
private:
    ptr<Expr> value;
    
public:
    const std::string identifier;

    explicit AssignmentStmt(std::string _ident, ptr<Expr> _value) : 
        Stmt(AstNodeType::Assignment), value(std::move(_value)), identifier(std::move(_ident)) { }
    
    [[nodiscard]] const Expr& getValue() const { return *value; }
    [[nodiscard]] Expr& getValue() { return *value; }
};
