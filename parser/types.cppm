module;

#include <string>

export module parser.types;

import token;
import types.info;
import types.structure;
import types.registry;
import error;

export namespace type_parser
{
    [[nodiscard]] bool isPrimitiveTypeToken(TokenType type)
    {
        switch (type)
        {
        case TokenType::TP_BYTE:
        case TokenType::TP_CHAR:
        case TokenType::TP_SMALL:
        case TokenType::TP_INT:
        case TokenType::TP_LONG:
        case TokenType::TP_FLOAT:
        case TokenType::TP_DOUBLE:
        case TokenType::TP_VOID:
            return true;

        default:
            return false;
        }
    }

    [[nodiscard]] TypeInfo parse(const Token& token, const TypeRegistry& registry)
    {
        switch (token.type)
        {
        case TokenType::TP_BYTE:
            return registry.getPrimitiveType(PrimitiveKind::BYTE);

        case TokenType::TP_CHAR:
            return registry.getPrimitiveType(PrimitiveKind::CHAR);

        case TokenType::TP_SMALL:
            return registry.getPrimitiveType(PrimitiveKind::SMALL);

        case TokenType::TP_INT:
            return registry.getPrimitiveType(PrimitiveKind::INT);

        case TokenType::TP_LONG:
            return registry.getPrimitiveType(PrimitiveKind::LONG);

        case TokenType::TP_FLOAT:
            return registry.getPrimitiveType(PrimitiveKind::FLOAT);

        case TokenType::TP_DOUBLE:
            return registry.getPrimitiveType(PrimitiveKind::DOUBLE);

        case TokenType::TP_VOID:
            return registry.getPrimitiveType(PrimitiveKind::VOID);

        default:
            std::string err = "Unexpected type token on declaration: " + token.src;
            error::report(err);

            return TypeInfo::errorType();
        }
    }
}
