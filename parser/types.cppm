export module parser.types;

import token;
import types.info;
import error;

export namespace type_parser
{
    [[nodiscard]] bool isTypeToken(TokenType type)
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

    [[nodiscard]] TypeInfo parse(const Token& token)
    {
        switch (token.type)
        {
        case TokenType::TP_BYTE:
            return TypeInfo{TypeKind::BYTE};

        case TokenType::TP_CHAR:
            return TypeInfo{TypeKind::CHAR};

        case TokenType::TP_SMALL:
            return TypeInfo{TypeKind::SMALL};

        case TokenType::TP_INT:
            return TypeInfo{TypeKind::INT};

        case TokenType::TP_LONG:
            return TypeInfo{TypeKind::LONG};

        case TokenType::TP_FLOAT:
            return TypeInfo{TypeKind::FLOAT};

        case TokenType::TP_DOUBLE:
            return TypeInfo{TypeKind::DOUBLE};

        case TokenType::TP_VOID:
            return TypeInfo{TypeKind::VOID};

        default:
            error::report(
                "Unexpected type token on declaration: " + token.src
            );

            return TypeInfo{TypeKind::INT};
        }
    }
}
