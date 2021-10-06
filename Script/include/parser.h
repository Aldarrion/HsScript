#pragma once

#include "inc.h"

//------------------------------------------------------------------------------
typedef enum
{
    ANT_PROGRAM,
    ANT_BLOCK,
    ANT_EXPR_STMT,

    ANT_LITERAL,
    ANT_UNARY_OP,
    ANT_BINARY_OP,
} EASTNodeType;

//------------------------------------------------------------------------------
typedef struct ASTNode
{
    EASTNodeType type;
    union
    {
        // Statement
        struct
        {
            struct ASTNode* sibling;
            union
            {
                struct ASTNode* child;  // Block statement
                struct ASTNode* expr;   // Expression statement
            };
        } stmt;

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

