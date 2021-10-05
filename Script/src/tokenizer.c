#include "tokenizer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
static Bool8 IsWhitespace(char c)
{
    return c == ' '
        | c == '\t'
        | c == '\r'
        | c == '\n';
}

//------------------------------------------------------------------------------
static Bool8 IsIdentifier(char c)
{
    return (c != 0)
        & !IsWhitespace(c)
        & c != '='
        & c != '>'
        & c != '<'
        & c != '!'
        & c != '('
        & c != ')'
        & c != '{'
        & c != '}';
}

//------------------------------------------------------------------------------
static Bool8 IsNumber(char c)
{
    return (c > '0' & c < '9');
}

//------------------------------------------------------------------------------
static Bool8 IsIdentifierStart(char c)
{
    return IsIdentifier(c)
        & (c < '0' | c > '9');
}

//------------------------------------------------------------------------------
static void AddToken(SToken token, SToken** tokens, int* tokenCount, int* tokenCapacity)
{
    if (*tokenCount == *tokenCapacity)
    {
        *tokenCapacity *= 2;
        *tokens = realloc(*tokens, *tokenCapacity * sizeof(SToken));
    }

    (*tokens)[*tokenCount] = token;
    ++(*tokenCount);
}

//------------------------------------------------------------------------------
static void AddSimpleToken(ETokenType type, SToken** tokens, int* tokenCount, int* tokenCapacity)
{
    SToken token = { .type = type };
    AddToken(token, tokens, tokenCount, tokenCapacity);
}

//------------------------------------------------------------------------------
void FreeTokens(SToken** tokens, int* tokenCount)
{
    for (int i = 0; i < *tokenCount; ++i)
    {
        SToken* token = (*tokens) + i;
        switch (token->type)
        {
            case TOKEN_STRING: free(token->string); break;
            case TOKEN_IDENTIFIER: free(token->name); break;
            default: break;
        }
    }

    free(*tokens);
    *tokens = NULL;
    *tokenCount = 0;
}

//------------------------------------------------------------------------------
EResult Tokenize(char* code, int size, SToken** outTokens, int* outTokenCount)
{
    printf("Tokenizing\n");
    *outTokens = NULL;
    *outTokenCount = 0;

    int tokenCapacity = 4;
    SToken* tokens = malloc(tokenCapacity * sizeof(SToken));
    int tokenCount = 0;

    char* c = code;

    // Note that if *c != 0 then *c must be valid (maybe 0 though)
    while (*c)
    {
        if (*c == '/' && *(c + 1) == '/') // Comment
        {
            c += 2;
            while (*c)
            {
                // This eats the newline which is fine since whitespace is not significant
                if (*c == '\n')
                {
                    if (*(c + 1) == '\r')
                        ++c;
                    break;
                }
                ++c;
            }
        }
        else if (*c == ';')
        {
            AddSimpleToken(TOKEN_SEMICOLON, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '=' && *(c + 1) == '=')
        {
            AddSimpleToken(TOKEN_EQUAL_EQUAL, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '!' && *(c + 1) == '=')
        {
            AddSimpleToken(TOKEN_NOT_EQUAL, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '>' && *(c + 1) == '=')
        {
            AddSimpleToken(TOKEN_GREATER_EQUAL, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '<' && *(c + 1) == '=')
        {
            AddSimpleToken(TOKEN_LESS_EQUAL, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '=')
        {
            AddSimpleToken(TOKEN_EQUALS, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '>')
        {
            AddSimpleToken(TOKEN_GREATER, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '<')
        {
            AddSimpleToken(TOKEN_LESS, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '(')
        {
            AddSimpleToken(TOKEN_LEFT_BRACE, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == ')')
        {
            AddSimpleToken(TOKEN_RIGHT_BRACE, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '{')
        {
            AddSimpleToken(TOKEN_LEFT_CURLY, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '}')
        {
            AddSimpleToken(TOKEN_RIGHT_CURLY, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '+')
        {
            AddSimpleToken(TOKEN_PLUS, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '-')
        {
            AddSimpleToken(TOKEN_MINUS, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '/')
        {
            AddSimpleToken(TOKEN_SLASH, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '*')
        {
            AddSimpleToken(TOKEN_STAR, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c == '"') // String
        {
            char* start = c + 1;
            ++c;
            while (*c && *c != '"')
                ++c;

            if (!*c)
            {
                printf("ERROR: Matching closing quote for a string not found\n");
                goto error;
            }

            SToken token = { .type = TOKEN_STRING };
            int size = c - start;
            if (size > 0)
            {
                token.string = malloc(size + 1);
                memcpy(token.string, start, size);
                token.string[size] = 0;
            }

            AddToken(token, &tokens, &tokenCount, &tokenCapacity);
        }
        else if (*c >= '0' && *c <= '9') // Number
        {
            char* start = c;
            Bool8 hasDot = HS_FALSE;

            do {
                if (*c == '.')
                {
                    if (hasDot)
                    {
                        printf("ERROR: Not a valid number format\n");
                        goto error;
                    }
                    hasDot = HS_TRUE;
                }

                ++c;
            } while (*c && ((*c >= '0' & *c <= '9') | (*c == '.')));

            char previous = *c;
            *c = 0;

            SToken token;
            if (hasDot)
            {
                token.type = TOKEN_FLOAT;
                sscanf(start, "%f", &token.floatNum);
            }
            else
            {
                token.type = TOKEN_INTEGER;
                sscanf(start, "%d", &token.intNum);
            }

            *c = previous;

            AddToken(token, &tokens, &tokenCount, &tokenCapacity);
            continue;
        }
        else if (IsWhitespace(*c)) // Whitespace
        {
            // Continue
        }
        else // Identifier
        {
            char* start = c;
            while (IsIdentifier(*c))
                ++c;

            int size = c - start;
            if (strncmp(start, "if", size) == 0)
            {
                AddSimpleToken(TOKEN_IF, &tokens, &tokenCount, &tokenCapacity);
            }
            else if (strncmp(start, "else", size) == 0)
            {
                AddSimpleToken(TOKEN_ELSE, &tokens, &tokenCount, &tokenCapacity);
            }
            else if (strncmp(start, "while", size) == 0)
            {
                AddSimpleToken(TOKEN_WHILE, &tokens, &tokenCount, &tokenCapacity);
            }
            else if (strncmp(start, "for", size) == 0)
            {
                AddSimpleToken(TOKEN_FOR, &tokens, &tokenCount, &tokenCapacity);
            }
            else
            {
                SToken token = { .type = TOKEN_IDENTIFIER };
                token.name = malloc(size + 1);
                memcpy(token.name, start, size);
                token.name[size] = 0;

                AddToken(token, &tokens, &tokenCount, &tokenCapacity);
            }
            continue;
        }

        ++c;
    }

    AddSimpleToken(TOKEN_END, &tokens, &tokenCount, &tokenCapacity);

    *outTokens = tokens;
    *outTokenCount = tokenCount;

    printf("Tokenizing done\n");
    return R_OK;

error:
    printf("ERROR tokenizing\n");
    FreeTokens(&tokens, &tokenCount);
    return R_ERROR;
}
