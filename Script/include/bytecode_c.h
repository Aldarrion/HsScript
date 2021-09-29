#pragma once

#include <stdlib.h>
#include <string.h>

#include "bytecode_d.h"

inline SStackData CreateStack(int size)
{
	SStackData stackData;
	
	stackData.begin = malloc(size * sizeof(byte));
    //printf("A %p\n", stackData.begin);
	stackData.end = stackData.begin + size * sizeof(byte);
	stackData.stackPointer = stackData.begin;
	
	return stackData;
}

inline void AddInstruction(SStackData* instructionStack, EInstruction instruction)
{
	*instructionStack->stackPointer++ = instruction;
}

inline void DeleteStack(SStackData stackData)
{
    //printf("D %p\n", stackData.begin);
	free(stackData.begin);
}

inline void InitVM(SVMData* vmData, SStackData instructionStack, int dataSize, FuncArray functions)
{
	vmData->instructionStack = instructionStack;
	vmData->dataStack.base = CreateStack(dataSize);
	vmData->dataStack.reversePointer = vmData->dataStack.base.end;
	vmData->functions = functions;
}

inline void DeleteVM(SVMData* vmData, Bool8 keepInstructions, Bool8 keepFunctions)
{
	// delete instructions
	if (!keepInstructions)
	{
        DeleteStack(vmData->instructionStack);
	}
	
	if (!keepFunctions)
	{
		free(vmData->functions.begin);
	}
	
	// delete data
	DeleteStack(vmData->dataStack.base);
}

// without modifying pointers
inline hsbint LoadInt(byte* pointer)
{
	hsbint value;
	memcpy(&value, pointer, sizeof(hsbint));
	return value;
}

inline hsbfloat LoadFloat(byte* pointer)
{
	hsbfloat value;
	memcpy(&value, pointer, sizeof(hsbfloat));
	return value;
}

inline hsbbool LoadBool(byte* pointer)
{
	hsbbool value;
	memcpy(&value, pointer, sizeof(hsbbool));
	return value;
}

inline hsbaddress LoadAddress(byte* pointer)
{
	hsbaddress value;
	memcpy(&value, pointer, sizeof(hsbaddress));
	return value;
}

inline void StoreInt(byte* pointer, hsbint value)
{
	memcpy(pointer, &value, sizeof(hsbint));
}

inline void StoreFloat(byte* pointer, hsbfloat value)
{
	memcpy(pointer, &value, sizeof(hsbfloat));
}

inline void StoreBool(byte* pointer, hsbbool value)
{
	memcpy(pointer, &value, sizeof(hsbbool));
}

inline void StoreAddress(byte* pointer, hsbaddress value)
{
	memcpy(pointer, &value, sizeof(hsbaddress));
}

// modifies the pointer (for forward stack - memory address increases as stack grows)
inline hsbint LoadIntFwd(byte** pointer)
{
	*pointer -= sizeof(hsbint);
	return LoadInt(*pointer);
}

inline hsbfloat LoadFloatFwd(byte** pointer)
{
	*pointer -= sizeof(hsbfloat);
	return LoadFloat(*pointer);
}

inline hsbbool LoadBoolFwd(byte** pointer)
{
	*pointer -= sizeof(hsbbool);
	return LoadBool(*pointer);
}

inline void StoreIntFwd(byte** pointer, hsbint value)
{
	memcpy(*pointer, &value, sizeof(hsbint));
	*pointer += sizeof(hsbint);
}

inline void StoreFloatFwd(byte** pointer, hsbfloat value)
{
	StoreFloat(*pointer, value);
	*pointer += sizeof(hsbfloat);
}

inline void StoreBoolFwd(byte** pointer, hsbbool value)
{
	StoreBool(*pointer, value);
	*pointer += sizeof(hsbbool);
}

// store and load address to the variable space
inline void StoreAddressVar(byte** varPointer, hsbaddress address)
{
	// var stack grows in the opposite direction
	*varPointer -= sizeof(hsbaddress);
	memcpy(*varPointer, &address, sizeof(hsbaddress));
}

inline hsbaddress LoadAddressVar(byte** varPointer)
{
	hsbaddress address;
	memcpy(&address, *varPointer, sizeof(hsbaddress));
	// delete address from the variable stack
	*varPointer += sizeof(hsbaddress);
	return address;
}

// Stack pointer operations

// save instruction stack pointer to the variable space
inline void SaveInsStackPointerVar(SVMData* vmData)
{
	hsbaddress address = vmData->instructionStack.stackPointer - vmData->instructionStack.begin;
	StoreAddressVar(&vmData->dataStack.reversePointer, address);
}

// load instruction stack pointer from the variable space
inline void LoadInsStackPointerVar(SVMData* vmData)
{
	hsbaddress address = LoadAddressVar(&vmData->dataStack.reversePointer);
	vmData->instructionStack.stackPointer = vmData->instructionStack.begin + address;
}



inline Bool8 VMProcessInstructions(SVMData* vmData, int count)
{
	for (int i=0; i < count; ++i)
	{
		EInstruction instruction = *vmData->instructionStack.stackPointer++;
		
		switch (instruction)
		{
			case INS_NOOP:
			{
				break;
			}
			
			case INS_ADD_I:
			{
				hsbint second = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				hsbint first = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				hsbint result = first + second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}
			
			case INS_ADD_F:
			{
				hsbfloat second = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				hsbfloat first = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				hsbfloat result = first + second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}
			case INS_SUBSTRACT_I:
			{
				hsbint second = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				hsbint first = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				hsbint result = first - second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}
			case INS_SUBSTRACT_F:
			{
				hsbfloat second = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				hsbfloat first = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				hsbfloat result = first - second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}
			case INS_MULTIPLY_I:
			{
				hsbint second = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				hsbint first = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				hsbint result = first * second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}
			case INS_MULTIPLY_F:
			{
				hsbfloat second = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				hsbfloat first = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				hsbfloat result = first * second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}
			case INS_DIVIDE_I:
			{
				hsbint second = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				hsbint first = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				hsbint result = first / second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}
			case INS_DIVIDE_F:
			{
				hsbfloat second = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				hsbfloat first = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				hsbfloat result = first / second;
				StoreIntFwd(&vmData->dataStack.base.stackPointer, result);
				break;
			}

			case INS_LITERAL_I:
			{
				// load from instructions
				hsbint value = LoadInt(vmData->instructionStack.stackPointer);
				vmData->instructionStack.stackPointer += sizeof(hsbint);
				
				// store to data
				StoreIntFwd(&vmData->dataStack.base.stackPointer, value);
				break;
			}
			case INS_LITERAL_F:
			{
				// load from instructions
				hsbfloat value = LoadFloat(vmData->instructionStack.stackPointer);
				vmData->instructionStack.stackPointer += sizeof(hsbfloat);
				
				// store to data
				StoreFloatFwd(&vmData->dataStack.base.stackPointer, value);
				break;
			}
			
			case INS_LITERAL_B:
			{
				// load from instructions
				hsbbool value = LoadBool(vmData->instructionStack.stackPointer);
				vmData->instructionStack.stackPointer += sizeof(hsbbool);
				
				// store to data
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, value);
				break;
			}
			case INS_NEGATE_B:
			{
				hsbbool value = LoadBoolFwd(&vmData->dataStack.base.stackPointer);
				
				// negate (bools have values 0 or 1)
				value = 1 - value;
				
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, value);
				
				break;
			}
			case INS_AND_B:
			{
				hsbbool second = LoadBoolFwd(&vmData->dataStack.base.stackPointer);
				hsbbool first = LoadBoolFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first & second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}
			case INS_OR_B:
			{
				hsbbool second = LoadBoolFwd(&vmData->dataStack.base.stackPointer);
				hsbbool first = LoadBoolFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first | second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}
			
			
			case INS_CMP_I_EQ:
			{
				hsbint second = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				hsbint first = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first == second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}
			case INS_CMP_I_LESS:
			{
				hsbint second = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				hsbint first = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first < second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}
			case INS_CMP_I_LESS_EQ:
			{
				hsbint second = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				hsbint first = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first <= second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}
			
			case INS_CMP_F_EQ:
			{
				hsbfloat second = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				hsbfloat first = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first == second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}
			case INS_CMP_F_LESS:
			{
				hsbfloat second = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				hsbfloat first = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first < second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}
			case INS_CMP_F_LESS_EQ:
			{
				hsbfloat second = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				hsbfloat first = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				hsbbool result = first <= second;
				StoreBoolFwd(&vmData->dataStack.base.stackPointer, result);
				
				break;
			}

			case INS_ALLOC_VAR_I:
			{
				vmData->dataStack.reversePointer -= sizeof(hsbint);
				break;
			}
			
			case INS_ALLOC_VAR_F:
			{
				vmData->dataStack.reversePointer -= sizeof(hsbfloat);
				break;
			}
			
			case INS_DEALLOC_VAR_I:
			{
				vmData->dataStack.reversePointer += sizeof(hsbint);
				break;
			}
			
			case INS_DEALLOC_VAR_F:
			{
				vmData->dataStack.reversePointer += sizeof(hsbfloat);
				break;
			}

			case INS_SAVE_VAR_I:
			{
				int offset = *vmData->instructionStack.stackPointer++;
				hsbint value = LoadIntFwd(&vmData->dataStack.base.stackPointer);
				
				StoreInt(vmData->dataStack.reversePointer + offset, value);
				break;
			}
			
			case INS_SAVE_VAR_F:
			{
				int offset = *vmData->instructionStack.stackPointer++;
				hsbfloat value = LoadFloatFwd(&vmData->dataStack.base.stackPointer);
				
				StoreFloat(vmData->dataStack.reversePointer + offset, value);
				break;
			}
			
			case INS_LOAD_VAR_I:
			{
				int offset = *vmData->instructionStack.stackPointer++;
				
				hsbint value = LoadInt(vmData->dataStack.reversePointer + offset);
				
				StoreIntFwd(&vmData->dataStack.base.stackPointer, value);
				break;
			}
			
			case INS_LOAD_VAR_F:
			{
				int offset = *vmData->instructionStack.stackPointer++;
				
				hsbfloat value = LoadFloat(vmData->dataStack.reversePointer + offset);
				
				StoreFloatFwd(&vmData->dataStack.base.stackPointer, value);
				break;
			}

			case INS_JUMP:
			{
				hsbaddress address = LoadAddress(vmData->instructionStack.stackPointer);
				// do not need to move the stack pointer, we're jumping anyway
				// vmData->instructionStack.stackPointer += sizeof(hsbaddress);
				vmData->instructionStack.stackPointer = vmData->instructionStack.begin + address;
				
				break;
			}
			
			case INS_COND_JUMP_B:
			{
				hsbaddress address = LoadAddress(vmData->instructionStack.stackPointer);
				vmData->instructionStack.stackPointer += sizeof(hsbaddress);
				
				hsbbool value = LoadBoolFwd(&vmData->dataStack.base.stackPointer);
				
				if (value != 0)
				{
					// jump
					vmData->instructionStack.stackPointer = vmData->instructionStack.begin + address;
				}
				
				break;
			}

			case INS_CALL:
			{
				hsbaddress address = LoadAddress(vmData->instructionStack.stackPointer);
				vmData->instructionStack.stackPointer += sizeof(hsbaddress);
				
				// save current position
				SaveInsStackPointerVar(vmData);
				// move to the function instructions
				vmData->instructionStack.stackPointer = vmData->instructionStack.begin + address;
				break;
			}
			
			case INS_RETURN:
			{
				LoadInsStackPointerVar(vmData);
				break;
			}
			
			case INS_CALL_EXT:
			{
				// TODO - add support for some native functions, implemented in C
				break;
			}
			
			default:
				// error, unrecognized instruction, exit immediately 
				return HS_FALSE;
		}
		
		// no instructions left, reached end of the program
		if (vmData->instructionStack.stackPointer >= vmData->instructionStack.end)
		{
			return HS_FALSE;
		}
	}
	
	return HS_TRUE;
}
