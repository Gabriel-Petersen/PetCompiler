# PetCompiler

Projeto experimental de uma linguagem de programação própria escrito em C++23.

O objetivo de longo prazo do projeto é desenvolver um compilador completo para a linguagem Pet, incluindo análise léxica, análise sintática, análise semântica, sistema de tipos e geração de código nativo.

Atualmente, grande parte da infraestrutura de frontend já existe, enquanto o backend de compilação ainda não foi iniciado. Um interpretador simples está presente principalmente como ferramenta de desenvolvimento e validação da AST, não como objetivo final do projeto.

O projeto é desenvolvido por uma única pessoa e encontra-se em fase de experimentação e reorganização arquitetural. Mudanças frequentes na estrutura interna são esperadas.

## Objetivos

O objetivo final é que o fluxo de compilação tenha aproximadamente a seguinte estrutura:

```text
Código fonte (.pet)
        ↓
Lexer
        ↓
Parser
        ↓
AST
        ↓
Análise Semântica
        ↓
Sistema de Tipos
        ↓
Geração de IR
        ↓
Backend de Compilação
        ↓
Executável Nativo
```

Atualmente apenas parte desse pipeline está implementada.

## Estado Atual

Implementado:

- Lexer funcional.
- Parser funcional.
- Construção de AST.
- Execução de AST através de interpretador de apoio.
- Sistema inicial de tipos.
- Estruturas básicas para análise semântica.
- Infraestrutura de diagnóstico e reporte de erros.

Em desenvolvimento:

- Migração gradual para módulos C++23.
- Consolidação do sistema de tipos.
- Análise semântica mais robusta.
- Revisão das regras de escopo.
- Preparação para futura geração de código.

Ainda não iniciado:

- Backend de compilação.
- Integração com LLVM.
- Geração de código nativo.
- Otimizações.

## Estrutura do Projeto

```text
ast/
    Nós da árvore sintática abstrata e interpretador.

lexer/
    Tokenização do código-fonte.

parser/
    Construção da AST a partir da sequência de tokens.

semantic_analyzer/
    Infraestrutura para verificações semânticas.

type_system/
    Representação e manipulação de tipos.

compile_error/
    Sistema de coleta e emissão de diagnósticos.

test/
    Programas de teste da linguagem.
```

## Sobre os Módulos C++23

O projeto está em processo de migração para recursos modernos de C++23.

Atualmente coexistem:

- arquivos header tradicionais (`.h`);
- módulos (`.cppm`);
- código ainda aguardando migração.

A adoção de módulos ainda é parcial, mas representa a arquitetura final pretendida.

## Requisitos

- CMake 3.28 ou superior
- Ninja
- GCC 16 ou superior

## Compilação e Execução

O script auxiliar recompila o projeto e executa o binário:

```bash
./compile-linux.sh
```

ou

```powershell
powershell .\compile-windows.ps1
```

Atualmente o programa principal utiliza arquivos presentes em `test/` para validação manual das funcionalidades implementadas.

## Exemplo

Exemplo de programa válido:

```pet
print 1;

int x = 2;
print x;

if (x > 0)
{
    int y = 9;
    x = 4;
    print x + y;
}

print y;
return x + 3;
```

## Próximos Passos

- [ ] Concluir migração para módulos C++23.
- [ ] Revisar arquitetura do sistema de tipos.
- [ ] Concluir infraestrutura de análise semântica.
- [ ] Definir comportamento definitivo para escopo e resolução de símbolos.
- [ ] Melhorar os diagnósticos de compilação.
- [ ] Planejar camada intermediária de representação.
- [ ] Investigar integração com LLVM.
- [ ] Implementar geração de código.

## Observações

O estado atual do código não representa a visão final do projeto.

Diversas decisões existentes foram tomadas para acelerar experimentação, testes e validação de conceitos. Conforme a arquitetura amadurecer, componentes internos poderão ser reescritos ou substituídos sem compromisso de compatibilidade.