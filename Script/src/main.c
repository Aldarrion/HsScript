#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

typedef int8_t Bool8;
#define HS_FALSE 0
#define HS_TRUE 1

//------------------------------------------------------------------------------
typedef enum
{
    R_OK,
    R_ERROR
} EResult;

//------------------------------------------------------------------------------
typedef enum
{
    TOKEN_SEMICOLON,
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
typedef struct
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
const char* GetTokenString(ETokenType token)
{
    switch (token)
    {
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_EQUALS: return "TOKEN_EQUALS";
        case TOKEN_LEFT_BRACE: return "TOKEN_LEFT_BRACE";
        case TOKEN_RIGHT_BRACE: return "TOKEN_RIGHT_BRACE";
        case TOKEN_LEFT_CURLY: return "TOKEN_LEFT_CURLY";
        case TOKEN_RIGHT_CURLY: return "TOKEN_RIGHT_CURLY";
        case TOKEN_PLUS: return "TOKEN_PLUS";
        case TOKEN_MINUS: return "TOKEN_MINUS";
        case TOKEN_SLASH: return "TOKEN_SLASH";
        case TOKEN_STAR: return "TOKEN_STAR";
        case TOKEN_GREATER: return "TOKEN_GREATER";
        case TOKEN_LESS: return "TOKEN_LESS";
        case TOKEN_GREATER_EQUAL: return "TOKEN_GREATER_EQUAL";
        case TOKEN_LESS_EQUAL: return "TOKEN_LESS_EQUAL";
        case TOKEN_EQUAL_EQUAL: return "TOKEN_EQUAL_EQUAL";
        case TOKEN_NOT_EQUAL: return "TOKEN_NOT_EQUAL";
        case TOKEN_IF: return "TOKEN_IF";
        case TOKEN_ELSE: return "TOKEN_ELSE";
        case TOKEN_WHILE: return "TOKEN_WHILE";
        case TOKEN_FOR: return "TOKEN_FOR";
        case TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_INTEGER: return "TOKEN_INTEGER";
        case TOKEN_FLOAT: return "TOKEN_FLOAT";
        case TOKEN_END: return "TOKEN_END";
        default: return "ERROR_INVALID_TOKEN_TYPE";
    }
}

//------------------------------------------------------------------------------
static void PrintToken(SToken token)
{
    switch (token.type)
    {
        case TOKEN_STRING:      printf("\"%s\" (%s)\n", token.string, GetTokenString(token.type)); return;
        case TOKEN_IDENTIFIER:  printf("%s (%s)\n", token.name, GetTokenString(token.type)); return;
        case TOKEN_INTEGER:     printf("%d (%s)\n", token.intNum, GetTokenString(token.type)); return;
        case TOKEN_FLOAT:       printf("%f (%s)\n", token.floatNum, GetTokenString(token.type)); return;
        default:                printf("%s\n", GetTokenString(token.type)); return;
    }
}

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
static EResult Tokenize(char* code, int size, SToken** outTokens, int* outTokenCount)
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
            // TODO(pavel): Parse floats

            int number = 0;
            do {
                number *= 10;
                number += *c - '0';
                ++c;
            } while (*c >= '0' && *c <= '9');

            SToken token = { .type = TOKEN_INTEGER, .intNum = number };
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
    free(tokens);
    return R_ERROR;
}

//------------------------------------------------------------------------------
static EResult Parse()
{
    // Input = tokens
    // Output = Abstract syntax tree
    return R_OK;
}

//------------------------------------------------------------------------------
static EResult Compile()
{
    // Input = AST
    // Output = Bytecode
    return R_OK;
}

//------------------------------------------------------------------------------
static EResult CompileCode(char* code, int size)
{
    printf("Compiling code\n");
    EResult r;

    SToken* tokens;
    int tokenCount;

    r = Tokenize(code, size, &tokens, &tokenCount);
    if (r != R_OK)
        return r;

    // TODO(pavel): Free tokens and all their data

    printf("-- Printing\n");
    SToken* t = tokens;
    while (t->type != TOKEN_END)
    {
        PrintToken(*t);
        ++t;
    }

    r = Parse();
    if (r != R_OK)
        return r;

    r = Compile();
    if (r != R_OK)
        return r;

    return r;
}

//------------------------------------------------------------------------------
static Bool8 ReadFile(char** fileBuff, int* size)
{
    FILE* file = fopen("SimpleTest.hss", "rb");
    if (!file)
    {
        printf("File does not exist\n");
        return HS_FALSE;
    }

    Bool8 result = HS_TRUE;
    fseek(file , 0 , SEEK_END);
    int fileSize = ftell(file);
    rewind(file);

    *fileBuff = malloc(fileSize + 1);

    if (!*fileBuff)
    {
        printf("Failed to allocate memory for the file\n");
        goto end;
    }

    int readSize = fread(*fileBuff, 1, fileSize, file);
    int eof = feof(file);

    if (readSize != fileSize && !eof)
    {
        free(*fileBuff);
        result = HS_FALSE;
        printf("Failed to read the whole file\n");
        goto end;
    }

    (*fileBuff)[fileSize] = 0; // Zero terminate to have a proper string
    *size = readSize;
end:
    fclose(file);
    return result;
}

//------------------------------------------------------------------------------
static void Test()
{
    printf("--------------------- Test 1 ---------------------\n");

    char* file;
    int size;
    if (!ReadFile(&file, &size))
    {
        printf("Failed to read the file\n");
        return;
    }

    CompileCode(file, size);
}

//------------------------------------------------------------------------------
int main()
{
    Test();
    return 0;
}
