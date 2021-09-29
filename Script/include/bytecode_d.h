#pragma once

#include "inc.h"

typedef uint8_t byte;

typedef int16_t hsbint;
typedef float hsbfloat;
typedef uint16_t hsbaddress;
typedef uint8_t hsbbool;

struct SVMData;
typedef void (NativeFP)(struct SVMData* vmData);

typedef struct
{
	byte* begin;
	byte* end;
	byte* stackPointer;
} SStackData;

typedef struct
{
	SStackData base;
	byte* reversePointer;
} SDoubleStackData;

typedef struct
{
	int count;
	NativeFP* begin;
} FuncArray;

typedef struct
{
	SStackData instructionStack;
	SDoubleStackData dataStack;
	FuncArray functions;
} SVMData;

typedef enum
{
	INS_NOOP,
	
	INS_ADD_I, 
	INS_ADD_F,
	INS_SUBSTRACT_I,
	INS_SUBSTRACT_F,
	INS_MULTIPLY_I,
	INS_MULTIPLY_F,
	INS_DIVIDE_I,
	INS_DIVIDE_F,
	
	INS_LITERAL_I,
	INS_LITERAL_F,
	
	// bools
	INS_LITERAL_B,
	INS_NEGATE_B,
	INS_AND_B,
	INS_OR_B,
	
	INS_CMP_I_EQ,
	INS_CMP_I_LESS,
	INS_CMP_I_LESS_EQ,
	INS_CMP_F_EQ,
	INS_CMP_F_LESS,
	INS_CMP_F_LESS_EQ,
	
	INS_ALLOC_VAR_I,
	INS_ALLOC_VAR_F,
	INS_DEALLOC_VAR_I,
	INS_DEALLOC_VAR_F,
	
	INS_SAVE_VAR_I,
	INS_SAVE_VAR_F,
	INS_LOAD_VAR_I,
	INS_LOAD_VAR_F,
	
	INS_JUMP,
	INS_COND_JUMP_B,
	
	INS_CALL,
	INS_RETURN,
	
	INS_CALL_EXT
	
} EInstruction;


