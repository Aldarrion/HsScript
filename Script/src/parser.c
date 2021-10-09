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

/*
Grammar from https://craftinginterpreters.com/appendix-i.html

program        → declaration* EOF ;



declaration    → classDecl
               | funDecl
               | varDecl
               | statement ;

classDecl      → "class" IDENTIFIER ( "<" IDENTIFIER )?
                 "{" function* "}" ;
funDecl        → "fun" function ;
varDecl        → "var" IDENTIFIER ( "=" expression )? ";" ;



statement      → exprStmt
               | forStmt
               | ifStmt
               | printStmt
               | returnStmt
               | whileStmt
               | block ;

exprStmt       → expression ";" ;
forStmt        → "for" "(" ( varDecl | exprStmt | ";" )
                           expression? ";"
                           expression? ")" statement ;
ifStmt         → "if" "(" expression ")" statement
                 ( "else" statement )? ;
printStmt      → "print" expression ";" ;
returnStmt     → "return" expression? ";" ;
whileStmt      → "while" "(" expression ")" statement ;
block          → "{" declaration* "}" ;



expression     → assignment ;

assignment     → ( call "." )? IDENTIFIER "=" assignment
               | logic_or ;

logic_or       → logic_and ( "or" logic_and )* ;
logic_and      → equality ( "and" equality )* ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;

unary          → ( "!" | "-" ) unary | call ;
call           → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
primary        → "true" | "false" | "nil" | "this"
               | NUMBER | STRING | IDENTIFIER | "(" expression ")"
               | "super" "." IDENTIFIER ;


function       → IDENTIFIER "(" parameters? ")" block ;
parameters     → IDENTIFIER ( "," IDENTIFIER )* ;
arguments      → expression ( "," expression )* ;

*/

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

static SToken* Expect(SToken* t, ETokenType type)
{
    // TODO(pavel): error handling
    assert(t->type == type);
    return t;
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
static SASTNode* Expr(SParserState* s)
{
    SASTNode* expr = Term(s);

    // TODO(pavel): Expressions

    return expr;
}

//------------------------------------------------------------------------------
static SASTNode* Statement(SParserState* s)
{
    SASTNode* stmt = AllocNode();
    // TODO(pavel): Match statements

    // Expression statement
    stmt->type = ANT_EXPR_STMT;
    stmt->stmt.expr = Expr(s);
    Expect(s->t++, TOKEN_SEMICOLON);

    return stmt;
}

//------------------------------------------------------------------------------
static SASTNode* VariableDeclaration(SParserState* s)
{
    SToken* name =  Expect(s->t++, TOKEN_IDENTIFIER);
                    Expect(s->t++, TOKEN_COLON);
    SToken* type =  Expect(s->t++, TOKEN_IDENTIFIER);

    SASTNode* initExpr = NULL;
    if (s->t->type == TOKEN_EQUALS)
    {
        ++s->t;
        initExpr = Expr(s);
    }

    SASTNode* varDecl = AllocNode();
    *varDecl = (SASTNode)
    {
        .type = ANT_DECL_VAR,
        .declVar =
        {
            .type = type,
            .name = name,
            .initExpr = initExpr,
        }
    };

    Expect(s->t++, TOKEN_SEMICOLON);

    return varDecl;
}

//------------------------------------------------------------------------------
static SASTNode* Declaration(SParserState* s)
{
    SASTNode* decl = AllocNode();
    decl->decl.sibling = NULL;

    switch (s->t->type)
    {
        case TOKEN_VAR:
        {
            ++s->t;
            decl->type = ANT_DECL_VAR;
            decl->decl.declVar = VariableDeclaration(s);
            break;
        }
        // TODO other declarations
        default: // Statement
        {
            decl->type = ANT_DECL_STMT;
            decl->decl.stmt = Statement(s);
            break;
        }
    }

    return decl;
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
        .stmt = { .child = NULL }
    };

    SASTNode** next = &(*root)->programChild;

    while (state.t->type != TOKEN_END)
    {
        *next = Declaration(&state);
        next = &(*next)->decl.sibling;
    }


    return R_OK;
}
