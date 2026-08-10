#include <iostream>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast.h"
#include "compile_error/compile_error.h"

int main() 
{
    lexer::Lexer lx("test/test.pet");

    char ans;
    std::cout << "Deseja imprimir os tokens? [S/n] ";
    std::cin >> ans;
    if (std::toupper(ans) == 'S')
    {
        for (auto& val : lx.getAllToken())
            std::cout << "[" << val.type << "]" << " ";
        std::cout << std::endl;
    }

    parser::Parser p(lx);
    auto minhaArvore = p.getAst();
    std::cout << "Parser finalizado.\n";

    if (error::hasErrors()) 
    {
        error::dump();
        return 1;
    }

    if (minhaArvore == nullptr) { std::cerr << "Arvore nullptr\n"; return 1; }
    
    std::cout << "\n-------------Inicializando execucao do programa-----------------\n\n";
    long long ret = minhaArvore->interpret().getNumber<long long>();
    std::cout << "[PROGRAM-RETURN]: " << ret << std::endl;

    return 0;
}