cmake_minimum_required(VERSION 3.28)
project(PetCompiler CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(pet_compiler)

target_sources(pet_compiler
    PRIVATE
        main.cpp
)

target_sources(pet_compiler
    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
            compile_error/compile_error.cppm

            lexer/token.cppm
            lexer/lexer.cppm

            type_system/type_info.cppm
            type_system/casts.cppm
            type_system/symbol_table.cppm

            ast/evaluation.cppm
            ast/ast.cppm
            ast/visitor.cppm
            ast/node.cppm
            
)

target_include_directories(pet_compiler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
