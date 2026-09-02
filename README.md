# PetCompiler

PetCompiler é um projeto experimental de implementação de uma linguagem de programação própria, desenvolvido em C++23.

O objetivo de longo prazo é construir um compilador completo para a linguagem Pet, incluindo análise léxica, análise sintática, análise semântica, sistema de tipos, geração de representação intermediária e geração de código nativo.

Atualmente, o frontend básico da linguagem está funcional. Programas Pet podem ser tokenizados, convertidos em uma árvore sintática abstrata e executados por um interpretador de apoio.

O interpretador não é o produto final do projeto. Ele existe principalmente como ferramenta de desenvolvimento, debugging e validação do lexer, parser, AST e futuras etapas do compilador.

## Objetivo

O fluxo final planejado para o compilador é aproximadamente:

```text
Código-fonte Pet
        ↓
Lexer
        ↓
Parser
        ↓
AST
        ↓
Análise semântica
        ↓
Sistema de tipos
        ↓
Representação intermediária
        ↓
Backend de compilação
        ↓
Código nativo
```

A integração com LLVM é uma possibilidade planejada para o backend, mas ainda não foi iniciada. O projeto atualmente não instala, configura ou utiliza LLVM.

## Pipeline atual

O pipeline funcional atual é:

```text
Arquivo .pet
        ↓
Lexer
        ↓
Sequência de tokens
        ↓
TokenCursor
        ↓
ExpressionParser + StatementParser
        ↓
AST
        ↓
Dispatcher
        ↓
Visitor Interpreter
        ↓
Runtime
        ↓
Resultado do programa
```

Esse pipeline permite validar o frontend da linguagem antes da implementação de um backend de compilação.

## Estado atual

### Implementado

- Lexer funcional.
- Representação de tokens.
- Cursor seguro para navegação e consumo de tokens.
- Parser de expressões com precedência.
- Parser de statements.
- Construção de AST.
- Ownership da AST baseado em RAII e smart pointers.
- Dispatcher para os tipos concretos da AST.
- Interface baseada no padrão Visitor.
- Interpretador implementado como Visitor.
- Runtime básico para armazenamento de variáveis e controle de profundidade.
- Sistema inicial de tipos.
- Infraestrutura de coleta e emissão de diagnósticos.
- Entrada por arquivo informado na linha de comando.
- Impressão opcional dos tokens para debugging.
- Scripts de compilação para Linux e Windows.
- Suíte local de experimentos para programas válidos e inválidos.

### Parcialmente implementado

- Sistema de tipos.
- Diagnósticos sintáticos.
- Recuperação simples após erros de parsing.
- Regras de escopo.
- Infraestrutura de análise semântica.

### Ainda não iniciado

- Geração de representação intermediária.
- Integração com LLVM.
- Backend de compilação.
- Integração com LLVM.
- Geração de código nativo.
- Otimizações.
- Suporte a projetos com múltiplos arquivos.
- Sistema completo de argumentos de linha de comando.

## Arquitetura em C++ moderno

O código principal utiliza módulos C++ e recursos modernos de gerenciamento de memória.

Entre as decisões atuais de arquitetura estão:

- módulos C++ como unidades principais de organização;
- `std::unique_ptr` para representar ownership exclusivo dos nós da AST;
- RAII para gerenciamento automático do tempo de vida dos objetos;
- ausência de `delete` manual no pipeline moderno;
- classes, structs e enums exportados diretamente;
- namespaces exportados reservados para APIs compostas por funções livres;
- separação entre estrutura da AST e comportamento de interpretação;
- Visitor e Dispatcher como mecanismos de processamento dos nós;
- Lexer, parsers e interpretador organizados como componentes independentes.

O compilador utilizado precisa possuir suporte suficientemente recente a módulos C++. Versões anteriores do GCC apresentaram falhas na geração e leitura de interfaces compiladas de módulos, especialmente com partes mais complexas da biblioteca padrão.

## Estrutura do projeto

```text
ast/
    Estrutura da árvore sintática abstrata, tipos de nós,
    Visitor e Dispatcher.

compile_error/
    Coleta, armazenamento e emissão de diagnósticos.

interpreter/
    Interpretador de apoio e runtime utilizado durante a execução
    da AST.

lexer/
    Representação de tokens e transformação do código-fonte
    em uma sequência de tokens.

parser/
    Navegação sobre tokens, parsing de tipos, expressões e
    statements, além da construção da AST.

semantic_analyzer/
    Infraestrutura relacionada à futura análise semântica.

type_system/
    Representação de tipos, conversões e tabela de símbolos.

test/
    Programas Pet usados para validação manual do pipeline.
```

## Sintaxe atualmente suportada

A linguagem possui suporte inicial a:

- literais inteiros;
- literais de ponto flutuante;
- literais booleanos;
- declarações de variáveis;
- atribuições;
- expressões aritméticas;
- operadores unários;
- comparações;
- igualdade e desigualdade;
- blocos;
- `if`;
- `else`;
- `else if`;
- `return`;
- `print(...)`.

O conjunto de construções ainda é reduzido e está sujeito a alterações.

## Exemplo

```pet
print(1);

int x = 2;
print(x);

if (x > 0)
{
    int y = 9;
    x = 4;
    print(x + y);
}

print(y);
return x + 3;
```

Saída atual:

```text
1
2
13
[RUNTIME-WARN]: Variavel 'y' (depth 2) acessada de um escopo mais raso (1)!
9
[PROGRAM-RETURN]: 7
```

O acesso a `y` fora do bloco é permitido pelo runtime de debugging atual, mas produz um aviso. Esse comportamento não representa necessariamente a regra final de escopo da linguagem.

## Requisitos

### Requisitos gerais

- CMake 3.28 ou superior
- Ninja
- Compilador com suporte recente a C++23 e módulos C++

### Linux

- GCC 16 ou superior

O projeto utiliza explicitamente `g++-16` no script Linux.

### Windows

- MSYS2 com ambiente UCRT64
- GCC compatível com o pipeline atual de módulos
- Ninja
- CMake
- PowerShell

O script Windows espera encontrar o compilador em:

```text
C:/msys64/ucrt64/bin/g++.exe
```

## Compilação no Linux

Compile com:

```bash
./compile-linux.sh
```

Para apagar completamente o diretório de build e reconfigurar o projeto:

```bash
./compile-linux.sh --force
```

O modo `--force` é recomendado após:

- trocar a versão do compilador;
- trocar o gerador do CMake;
- alterar nomes de módulos;
- mover arquivos `.cppm`;
- modificar significativamente o grafo de imports.

## Compilação no Windows

No PowerShell:

```powershell
.\compile-windows.ps1
```

Para remover o build anterior antes de compilar:

```powershell
.\compile-windows.ps1 --force
```

## Execução

Após a compilação, execute um programa Pet informando seu caminho:

```bash
./build/pet_compiler caminho/do/programa.pet
```

Exemplo:

```bash
./build/pet_compiler test/test_modern.pet
```

No Windows, o executável correspondente estará no diretório de build configurado pelo CMake.

## Debugging de tokens

Para imprimir os tokens produzidos pelo lexer:

```bash
./build/pet_compiler caminho/do/programa.pet --debug-tokens
```

A flag pode aparecer antes ou depois do arquivo:

```bash
./build/pet_compiler --debug-tokens caminho/do/programa.pet
```

Essa opção é útil para distinguir erros originados no lexer de problemas posteriores no parser, na AST ou no interpretador.

## Execução sem argumentos

Se nenhum arquivo for informado, o programa utiliza:

```text
test/test_modern.pet
```

como fallback.

Nesse modo, o programa também pergunta se deve subir um nível de diretório antes de resolver o caminho. Esse comportamento existe apenas como conveniência temporária de desenvolvimento.

## Limitações atuais da linha de comando

Por enquanto, apenas um arquivo de entrada é suportado:

```bash
./build/pet_compiler arquivo.pet
```

O processamento de múltiplos arquivos, projetos, opções de compilação e seleção de backend será implementado futuramente.

Alterações em arquivos `.pet` não exigem recompilação, pois os programas são lidos em tempo de execução pelo frontend.

## Próximos passos

- [ ] Consolidar o sistema de tipos.
- [ ] Implementar análise semântica no pipeline moderno.
- [ ] Definir as regras finais de escopo e resolução de símbolos.
- [ ] Melhorar mensagens e localização de erros.
- [ ] Melhorar a recuperação após erros de parsing.
- [ ] Adicionar metadados de linha e coluna aos diagnósticos.
- [ ] Expandir a gramática da linguagem.
- [ ] Adicionar novos tipos de expressão e statement.
- [ ] Definir uma representação intermediária.
- [ ] Investigar a integração com LLVM.
- [ ] Implementar o backend de compilação.
- [ ] Gerar código nativo.
- [ ] Implementar suporte a múltiplos arquivos.
- [ ] Expandir a interface de linha de comando.
- [ ] Automatizar testes de regressão versionados.

## Observações

O projeto ainda está em fase experimental e não possui compromisso de estabilidade de API, sintaxe ou representação interna.

A AST, o interpretador e o runtime existem atualmente para validar o frontend e facilitar o desenvolvimento. Componentes internos podem ser alterados ou substituídos conforme a arquitetura de compilação amadurecer.

Apesar do nome PetCompiler, o projeto ainda não gera código nativo. O termo representa o objetivo final do projeto, não a totalidade do estado atual.

