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

int main(int argc, char* argv[])
{
    std::string path;
    bool debugTokens = false;

    for (int i = 1; i < argc; i++) 
    {
        const std::string_view argument{argv[i]};

        if (argument == "--debug-tokens") {
            debugTokens = true;
            continue;
        }

        if (!path.empty())
        {
            std::cerr << "Erro: apenas um arquivo de entrada eh suportado por enquanto.\n";
            std::cerr << "Uso: " << argv[0] << " [--debug-tokens] [arquivo.pet]\n";
            return 1;
        }

        path = argument;
    }

    if (path.empty())
    {
        path = "test/test_modern.pet";
        std::cout << "Nenhum arquivo de entrada informado.\nUsando arquivo de teste: " << path << '\n';

        if (argc == 1)
        {
            char answer;

            std::cout << "[DEBUG] Subir uma pasta? [S/n] ";
            std::cin >> answer;

            if (std::toupper(static_cast<unsigned char>(answer)) == 'S') 
                path = "../" + path;
        }
    }

    
    std::cout << "Abrindo: " << path << '\n';

    Lexer lexer(path);

    if (!lexer.isValid) {
        std::cerr << "Nao foi possivel abrir o arquivo: " << path << '\n';
        return 1;
    }

    if (debugTokens)
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
