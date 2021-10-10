#pragma once

#include "inc.h"

//------------------------------------------------------------------------------
typedef enum
{
    ANT_PROGRAM,
    ANT_DECL_VAR,
    ANT_DECL_STMT,

    ANT_ASSIGN,

    ANT_EXPR_STMT,
    ANT_BLOCK,

    ANT_LITERAL,
    ANT_UNARY_OP,
    ANT_BINARY_OP,
} EASTNodeType;

//------------------------------------------------------------------------------
typedef struct Statement
{
    union
    {
        struct ASTNode* block;  // Block statement
        struct ASTNode* expr;   // Expression statement
    };
} SStatement;

//------------------------------------------------------------------------------
typedef struct ASTNode
{
    EASTNodeType type;
    union
    {
        struct ASTNode* programChild;

        // Declaration
        struct
        {
            struct ASTNode* sibling; // Also a declaration
            union
            {
                struct ASTNode* declVar;
                struct ASTNode* stmt;
            };
        } decl;

        // Variable declaration
        struct
        {
            struct Token* type;
            struct Token* name;
            struct ASTNode* initExpr; // Possibly NULL
        } declVar;

        struct Statement stmt;

        struct Assign
        {
            struct Token* var;
            struct ASTNode* assign;
        } assign;

        // Expressions
        struct
        {
            struct ASTNode* left;
            struct ASTNode* right;
            struct Token* op;
        } binary;

        struct
        {
            struct Token* op;
            struct ASTNode* right;
        } unary;

        struct
        {
            struct Token* token;
        } literal;
    };
} SASTNode;

//------------------------------------------------------------------------------
EResult Parse(struct Token* tokens, int tokenCount, SASTNode** outRoot);

