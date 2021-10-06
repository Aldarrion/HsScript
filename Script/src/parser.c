#include "parser.h"
#include "tokenizer.h"

#include <stdarg.h>
#include <assert.h>

//------------------------------------------------------------------------------
typedef struct
{
    SToken* t;
} SParserState;

//------------------------------------------------------------------------------
static SASTNode  g_Nodes[1024];
static SASTNode* g_NodesNext = g_Nodes;

static SASTNode* AllocNode()
{
    return g_NodesNext++;
}

#if 0
program        → statement* EOF ;

statement      → exprStmt
               | printStmt ;

exprStmt       → expression ";" ;
printStmt      → "print" expression ";" ;

term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;
#endif

//------------------------------------------------------------------------------
// Assignment : type? var = Expression;

//------------------------------------------------------------------------------
static Bool8 Match(const SToken* t, int count, ...)
{
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        ETokenType type = va_arg(args, ETokenType);
        if (type == t->type)
        {
            va_end(args);
            return HS_TRUE;
        }
    }

    va_end(args);
    return HS_FALSE;
}

static void Expect(SToken* t, ETokenType type)
{
    // TODO(pavel): error handling
    assert(t->type == type);
}

//------------------------------------------------------------------------------
SASTNode* MakeBinary(SASTNode* left, SToken* op, SASTNode* right)
{
    SASTNode* node = AllocNode();
    *node = (SASTNode)
    {
        .type = ANT_BINARY_OP,
        .binary =
        {
            .left = left,
            .right = right,
            .op = op,
        }
    };

    return node;
}

//------------------------------------------------------------------------------
SASTNode* MakeUnary(SToken* op, SASTNode* right)
{
    SASTNode* node = AllocNode();
    *node = (SASTNode)
    {
        .type = ANT_UNARY_OP,
        .unary =
        {
            .op = op,
            .right = right,
        }
    };

    return node;
}

//------------------------------------------------------------------------------
SASTNode* MakeLiteral(SToken* literal)
{
    SASTNode* node = AllocNode();
    *node = (SASTNode)
    {
        .type = ANT_LITERAL,
        .literal =
        {
            .token = literal,
        }
    };

    return node;
}

//------------------------------------------------------------------------------
static SASTNode* Primary(SParserState* s)
{
    if (Match(s->t, 3, TOKEN_INTEGER, TOKEN_FLOAT, TOKEN_IDENTIFIER))
    {
      return MakeLiteral(s->t++);
    }
    else
    {
        assert(!"Unexpected");
        return NULL;
    }
}

//------------------------------------------------------------------------------
static SASTNode* Unary(SParserState* s)
{
    if (Match(s->t, 1, TOKEN_MINUS))
    {
        SToken* op = s->t++;
        SASTNode* right = Unary(s);
        return MakeUnary(op, right);
    }

    return Primary(s);
}

//------------------------------------------------------------------------------
static SASTNode* Factor(SParserState* s)
{
    SASTNode* expr = Unary(s);
    while (Match(s->t, 2, TOKEN_STAR, TOKEN_SLASH))
    {
        SToken* op = s->t++;
        SASTNode* right = Unary(s);

        expr = MakeBinary(expr, op, right);
    }

    return expr;
}

//------------------------------------------------------------------------------
// Term
static SASTNode* Term(SParserState* s)
{
    SASTNode* expr = Factor(s);
    while (Match(s->t, 2, TOKEN_MINUS, TOKEN_PLUS))
    {
        SToken* op = s->t++;
        SASTNode* right = Factor(s);

        expr = MakeBinary(expr, op, right);
    }

    return expr;
}

//------------------------------------------------------------------------------
static SASTNode* Statement(SParserState* s)
{
    SASTNode* stmt = AllocNode();
    stmt->type = ANT_EXPR_STMT;
    stmt->expr.expr = Term(s);

    Expect(s->t++, TOKEN_SEMICOLON);
    return stmt;
}

//------------------------------------------------------------------------------
// Input = tokens
// Output = Abstract syntax tree
EResult Parse(SToken* tokens, int tokenCount, SASTNode** root)
{
    SParserState state =
    {
        .t = tokens,
    };

    *root = AllocNode();
    **root = (SASTNode)
    {
        .type = ANT_PROGRAM,
        .block = { .child = NULL }
    };

    SASTNode** next = &(*root)->block.child;

    while (state.t->type != TOKEN_END)
    {
        *next = Statement(&state);
        next = &(*next)->expr.sibling;
    }


    return R_OK;
}
