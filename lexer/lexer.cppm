module;

#include <cctype>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

export module lexer;

import token;

export class Lexer 
{
    inline static std::unordered_map<std::string, TokenType> tokenMap;
    inline static std::unordered_map<char, TokenType> monoCharTokenMap;
    inline static std::unordered_map<std::string, TokenType> dualOpTokenMap;

    void initTokenMap()
    {
        if (!tokenMap.empty()) return;

        monoCharTokenMap['+'] = TokenType::PLUS;
        monoCharTokenMap['-'] = TokenType::MINUS;
        monoCharTokenMap['*'] = TokenType::STAR;
        monoCharTokenMap['/'] = TokenType::SLASH;
        monoCharTokenMap['%'] = TokenType::MOD;
        monoCharTokenMap['('] = TokenType::L_PAREN;
        monoCharTokenMap[')'] = TokenType::R_PAREN;
        monoCharTokenMap['{'] = TokenType::L_BRACES;
        monoCharTokenMap['}'] = TokenType::R_BRACES;
        monoCharTokenMap[';'] = TokenType::SEMICOLON;
        monoCharTokenMap[','] = TokenType::COMMA;
        monoCharTokenMap['.'] = TokenType::DOT;
        monoCharTokenMap['='] = TokenType::EQUAL;
        monoCharTokenMap['<'] = TokenType::LESSER_THEN;
        monoCharTokenMap['>'] = TokenType::GRATER_THEN;
        monoCharTokenMap['!'] = TokenType::NOT;

        tokenMap["if"] = TokenType::IF;
        tokenMap["else"] = TokenType::ELSE;
        tokenMap["while"] = TokenType::WHILE;
        tokenMap["for"] = TokenType::FOR;
        tokenMap["return"] = TokenType::RETURN;
        tokenMap["void"] = TokenType::TP_VOID;
        tokenMap["byte"] = TokenType::TP_BYTE;
        tokenMap["char"] = TokenType::TP_CHAR;
        tokenMap["small"] = TokenType::TP_SMALL;
        tokenMap["int"] = TokenType::TP_INT;
        tokenMap["long"] = TokenType::TP_LONG;
        tokenMap["float"] = TokenType::TP_FLOAT;
        tokenMap["double"] = TokenType::TP_DOUBLE;
        tokenMap["true"] = TokenType::BOOL_LITERAL;
        tokenMap["false"] = TokenType::BOOL_LITERAL;

        tokenMap["print"] = TokenType::PRINT;

        dualOpTokenMap["++"] = TokenType::PLUSPLUS;
        dualOpTokenMap["--"] = TokenType::MINUSMINUS;
        dualOpTokenMap["+="] = TokenType::PLUS_EQ;
        dualOpTokenMap["-="] = TokenType::MINUS_EQ;
        dualOpTokenMap["*="] = TokenType::STAR_EQ;
        dualOpTokenMap["/="] = TokenType::SLASH_EQ;
        dualOpTokenMap["%="] = TokenType::MOD_EQ;
        dualOpTokenMap["=="] = TokenType::EQ_EQ;
        dualOpTokenMap["!="] = TokenType::NOT_EQ;
        dualOpTokenMap["<="] = TokenType::LESSER_OR_EQUALS;
        dualOpTokenMap[">="] = TokenType::GREATER_OR_EQUALS;
    }

    bool isComment()
    {
        if (cursor + 1 >= buff.size()) return false;
        return (buff[cursor] == '!' && buff[cursor + 1] == '!');
    }

    void jumpUselessCode()
    {
        bool cleaned = true;

        while (cleaned) 
        {
            cleaned = false;

            while (cursor < buff.size() && 
                (buff[cursor] == ' '  || buff[cursor] == '\t' || 
                buff[cursor] == '\n' || buff[cursor] == '\r')) 
            {
                if (buff[cursor] == '\n') thisLine++;
                cursor++;
                cleaned = true;
            }

            if (isComment()) 
            {
                cursor += 2; 
                while (cursor < buff.size() && buff[cursor] != '\n')
                    cursor++;
                
                cleaned = true;
            }
        }
    }

    Token readNum()
    {
        std::string tokenstr;
        bool isfloat = false;

        while (cursor < buff.size())
        {
            char curr = buff[cursor];

            if (std::isdigit(static_cast<unsigned char>(curr)))
            {
                tokenstr.push_back(curr);
                ++cursor;
                continue;
            }

            if (curr == '.' && cursor + 1 < buff.size() &&
                std::isdigit(static_cast<unsigned char>(buff[cursor + 1])))
            {
                tokenstr.push_back(curr);
                ++cursor;
                isfloat = true;
                continue;
            }

            break;
        }

        if (!tokenstr.empty()) {
            return Token{
                isfloat ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL, 
                tokenstr, 
                TokenMeta{thisLine, cursor % (1+thisLine)}
            };
        }
        return Token{TokenType::UNKNOWN, "", {0, 0}};
    }

    Token readWord()
    {
        std::string tokenstr;
        while (cursor < buff.size() &&
                (std::isalnum(static_cast<unsigned char>(buff[cursor])) || buff[cursor] == '_'))
            tokenstr.push_back(buff[cursor++]);

        if (tokenMap.count(tokenstr))
        {
            return Token{
                tokenMap[tokenstr],
                tokenstr,
                TokenMeta{thisLine, cursor % (1+thisLine)}
            };
        }
        return Token{
            TokenType::IDENTIFYER,
            tokenstr,
            TokenMeta{thisLine, cursor % (1+thisLine)}
        };
    }

    Token readStrLiteral()
    {
        std::string tokenstr;
        ++cursor; // ignora as primeiras aspas
        while (cursor < buff.size() && buff[cursor] != '"')
            tokenstr.push_back(buff[cursor++]);

        if (cursor >= buff.size())
            return Token{TokenType::END_OF_FILE, "", {0, 0}};

        return Token{
            TokenType::STRING_LITERAL,
            tokenstr,
            TokenMeta{thisLine, (++cursor) % (1+thisLine)}
        };
    }

    bool verifyDualCharToken(Token& t)
    {
        if (buff[cursor] == '!' || monoCharTokenMap.count(buff[cursor])) 
        {
            if ((cursor + 1 ) >= buff.size())
                return false;

            std::string tokenstr = std::string({buff[cursor], buff[cursor+1]});
            if (dualOpTokenMap.count(tokenstr))
            {
                t.meta = TokenMeta{thisLine, (cursor + 1) % (1+thisLine)};
                t.src = tokenstr;
                t.type = dualOpTokenMap[tokenstr];
                cursor += 2;
                return true;
            }
        }
        return false;
    }

public:
    bool isValid = false;
    std::string buff;
    std::size_t cursor;
    std::size_t thisLine;

    explicit Lexer(const std::string& path) : cursor(0), thisLine(0)
    {
        initTokenMap();
        std::ifstream file(path, std::ios::binary);
        if (file)
        {
            isValid = true;
            buff = std::string(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>()
            );
        }
    }

    Token nextToken()
    {
        jumpUselessCode();
        if (cursor >= buff.size()) return Token{TokenType::END_OF_FILE, "", {0, 0}};

        char curr = buff[cursor];
        if (std::isdigit(static_cast<unsigned char>(curr))) return readNum();
        if (std::isalpha(static_cast<unsigned char>(curr)) || curr == '_') return readWord();
        if (curr == '"') return readStrLiteral();
        Token t{};
        if (verifyDualCharToken(t))
            return t;

        if (monoCharTokenMap.count(curr))
            return Token{
                monoCharTokenMap[curr],
                std::string(1, curr),
                TokenMeta{thisLine, cursor++ % (1+thisLine)}
            };
        else
        {
            if (cursor++ >= buff.size()) return Token{TokenType::END_OF_FILE, "", {0, 0}};
            return Token{TokenType::UNKNOWN, "", {0, 0}};
        }
    }

    std::vector<Token> getAllToken()
    {
        std::vector<Token> tkVet;
        auto tk = nextToken();
        while (tk.type != TokenType::END_OF_FILE)
        {
            tkVet.push_back(tk);
            tk = nextToken();
        }
        tkVet.push_back(tk);
        cursor = 0;
        thisLine = 0;
        return tkVet;
    }
};
