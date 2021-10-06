#include "tokenizer.h"
#include "parser.h"
#include "inc.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

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
static EResult Compile()
{
    // Input = AST
    // Output = Bytecode
    return R_OK;
}

//------------------------------------------------------------------------------
static void PrintNode(SASTNode* node)
{
    switch (node->type)
    {
        case ANT_PROGRAM:
        case ANT_BLOCK:
        {
            SASTNode* child = node->stmt.child;
            while (child)
            {
                PrintNode(child);
                child = child->stmt.sibling;
            }
            break;
        }

        case ANT_EXPR_STMT:
        {
            PrintNode(node->stmt.expr);
            break;
        }

        case ANT_LITERAL:
        {
            SToken* token = node->literal.token;
            switch (token->type)
            {
                case TOKEN_INTEGER:
                {
                    printf("%d ", token->intNum);
                    break;
                }
                case TOKEN_FLOAT:
                {
                    printf("%f ", token->floatNum);
                    break;
                }
                case TOKEN_IDENTIFIER:
                {
                    printf("%s ", token->name);
                    break;
                }
                default: assert(0); break;
            }
            break;
        }
        case ANT_UNARY_OP:
        {
            SToken* op = node->unary.op;
            switch (op->type)
            {
                case TOKEN_MINUS:
                {
                    printf("-");
                    break;
                }
                default: assert(0); break;
            }
            PrintNode(node->unary.right);
            break;
        }
        case ANT_BINARY_OP:
        {
            SToken* op = node->binary.op;
            printf("(");
            switch (op->type)
            {
                case TOKEN_PLUS:
                {
                    printf("+ ");
                    break;
                }
                case TOKEN_MINUS:
                {
                    printf("- ");
                    break;
                }
                case TOKEN_STAR:
                {
                    printf("* ");
                    break;
                }
                case TOKEN_SLASH:
                {
                    printf("/ ");
                    break;
                }
                default: assert(0); break;
            }
            PrintNode(node->binary.left);
            PrintNode(node->binary.right);
            printf(") ");
            break;
        }
        default: assert(0); break;
    }
}

//------------------------------------------------------------------------------
void PrintAST(SASTNode* root)
{
    PrintNode(root);
    printf("<-- result\n");
    printf("(+ (+ 2 (/ (* y asd ) 123 ) ) (* x 2 ) ) <-- expected\n");
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
        goto end;


    //printf("-- Printing\n");
    //SToken* t = tokens;
    //while (t->type != TOKEN_END)
    //{
    //    PrintToken(*t);
    //    ++t;
    //}

    SASTNode* astRoot;
    r = Parse(tokens, tokenCount, &astRoot);
    if (r != R_OK)
        goto end;

    PrintAST(astRoot);

    r = Compile();
    if (r != R_OK)
        goto end;

end:
    FreeTokens(&tokens, &tokenCount);
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
