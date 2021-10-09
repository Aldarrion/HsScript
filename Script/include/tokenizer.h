#pragma once

#include "inc.h"

//------------------------------------------------------------------------------
typedef enum
{
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_EQUALS,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_CURLY,
    TOKEN_RIGHT_CURLY,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_SLASH,
    TOKEN_STAR,

    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_NOT_EQUAL,

    TOKEN_VAR,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,

    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_INTEGER,
    TOKEN_FLOAT,

    TOKEN_END,
} ETokenType;

//------------------------------------------------------------------------------
typedef struct Token
{
    ETokenType type;
    union
    {
        char*   string;
        char*   name;
        int     intNum;
        float   floatNum;
    };
} SToken;

//------------------------------------------------------------------------------
EResult Tokenize(char* code, int size, SToken** outTokens, int* outTokenCount);
void FreeTokens(SToken** tokens, int* tokenCount);
