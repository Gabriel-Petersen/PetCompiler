#include <cctype>
#include <iostream>
#include <memory>
#include <string>

import error;
import lexer;
import token;
import parser;
import parser.evaluation;
import ast;
import interpreter;

int main()
{
    std::string path = "test/test_modern.pet";

    char answer;

    std::cout << "[DEBUG] Subir uma pasta? [S/n] ";
    std::cin >> answer;

    if (std::toupper(static_cast<unsigned char>(answer)) == 'S') 
        path = "../" + path;

    std::cout << "Abrindo: " << path << '\n';

    Lexer lexer(path);

    if (!lexer.isValid) {
        std::cerr << "Nao foi possivel abrir o arquivo: " << path << '\n';
        return 1;
    }

    std::cout << "Deseja imprimir os tokens? [S/n] ";
    std::cin >> answer;

    if (std::toupper(static_cast<unsigned char>(answer)) == 'S')
    {
        for (const Token& token : lexer.getAllToken())
            std::cout << '[' << static_cast<int>(token.type) << "] ";

        std::cout << '\n';
    }

    Parser parser(lexer);

    std::unique_ptr<AstTree> tree = parser.takeAst();

    std::cout << "Parser finalizado.\n";

    if (error::hasErrors()) {
        error::dump();
        return 1;
    }

    if (!tree) {
        std::cerr << "AST nullptr\n";
        return 1;
    }

    std::cout << "\n" << "-------------Inicializando execucao do programa-----------------\n\n";

    Interpreter interpreter;
    Evaluation result = interpreter.interpret(*tree);

    if (error::hasErrors()) {
        error::dump();
        return 1;
    }

    std::cout << "[PROGRAM-RETURN]: " << result.getNumber<long long>() << '\n';

    return 0;
}
