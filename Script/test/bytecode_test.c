#include <stdio.h>

#include "bytecode_c.h"


static const int DATA_SIZE = 200;

int TestSimpleAdding()
{
	Bool8 testResult = HS_FALSE;
	SVMData vmData;
	
	SStackData instructionStack = CreateStack(100);
	AddInstruction(&instructionStack, INS_LITERAL_I);
	StoreIntFwd(&instructionStack.stackPointer, 1);
	AddInstruction(&instructionStack, INS_LITERAL_I);
	StoreIntFwd(&instructionStack.stackPointer, 20);
	AddInstruction(&instructionStack, INS_ADD_I);
	AddInstruction(&instructionStack, INS_LITERAL_I);
	StoreIntFwd(&instructionStack.stackPointer, 300);
	AddInstruction(&instructionStack, INS_LITERAL_I);
	StoreIntFwd(&instructionStack.stackPointer, 4000);
	AddInstruction(&instructionStack, INS_ADD_I);
	AddInstruction(&instructionStack, INS_ADD_I);
	
	instructionStack.end = instructionStack.stackPointer;
    instructionStack.stackPointer = instructionStack.begin;

	FuncArray funcArray;
	
	InitVM(&vmData, instructionStack, DATA_SIZE, funcArray);
	
	while(VMProcessInstructions(&vmData, 1))
	{
		//printf(".");
	}
	
	testResult = LoadInt(vmData.dataStack.base.begin) == 4321 && vmData.dataStack.reversePointer == vmData.dataStack.base.end;
	
	DeleteVM(&vmData, HS_FALSE, HS_TRUE);
	
	printf("TestSimpleAdding: ");
	if (testResult)
	{
		printf("passed\n");
	}
	else
	{
		printf("FAILED\n");
	}

    return 1 - testResult;
}

int TestVariable()
{
    Bool8 testResult = HS_FALSE;
    SVMData vmData;

    SStackData instructionStack = CreateStack(100);
	AddInstruction(&instructionStack, INS_ALLOC_VAR_I);
	AddInstruction(&instructionStack, INS_ALLOC_VAR_I);
    AddInstruction(&instructionStack, INS_LITERAL_I);
    StoreIntFwd(&instructionStack.stackPointer, 1);
    AddInstruction(&instructionStack, INS_SAVE_VAR_I);
    *instructionStack.stackPointer++ = 0 * sizeof(hsbint);
    AddInstruction(&instructionStack, INS_LITERAL_I);
    StoreIntFwd(&instructionStack.stackPointer, 20);
    AddInstruction(&instructionStack, INS_SAVE_VAR_I);
    *instructionStack.stackPointer++ = 1 * sizeof(hsbint);

    // load variables and add them
    AddInstruction(&instructionStack, INS_LOAD_VAR_I);
    *instructionStack.stackPointer++ = 0 * sizeof(hsbint);
    AddInstruction(&instructionStack, INS_LOAD_VAR_I);
    *instructionStack.stackPointer++ = 1 * sizeof(hsbint);

    AddInstruction(&instructionStack, INS_ADD_I);
	
	// dealloc variables
	AddInstruction(&instructionStack, INS_DEALLOC_VAR_I);
	AddInstruction(&instructionStack, INS_DEALLOC_VAR_I);

    instructionStack.end = instructionStack.stackPointer;
    instructionStack.stackPointer = instructionStack.begin;

    FuncArray funcArray;

    InitVM(&vmData, instructionStack, DATA_SIZE, funcArray);

    while (VMProcessInstructions(&vmData, 1))
    {
        //printf(".");
    }

    testResult = LoadInt(vmData.dataStack.base.begin) == 21 && vmData.dataStack.reversePointer == vmData.dataStack.base.end;

    DeleteVM(&vmData, HS_FALSE, HS_TRUE);

    printf("TestVariable: ");
    if (testResult)
    {
        printf("passed\n");
    }
    else
    {
        printf("FAILED\n");
    }

    return 1 - testResult;
}

int TestSimpleLoop()
{
    Bool8 testResult = HS_FALSE;
    SVMData vmData;
	
	int numIterations = 5;

    SStackData instructionStack = CreateStack(100);

	AddInstruction(&instructionStack, INS_ALLOC_VAR_I);
	AddInstruction(&instructionStack, INS_ALLOC_VAR_I);
	AddInstruction(&instructionStack, INS_LITERAL_I);
    StoreIntFwd(&instructionStack.stackPointer, 0);
	AddInstruction(&instructionStack, INS_SAVE_VAR_I);
    *instructionStack.stackPointer++ = 0 * sizeof(hsbint);
	AddInstruction(&instructionStack, INS_LITERAL_I);
    StoreIntFwd(&instructionStack.stackPointer, numIterations);
	AddInstruction(&instructionStack, INS_SAVE_VAR_I);
    *instructionStack.stackPointer++ = 1 * sizeof(hsbint);
	
	hsbaddress loopStartAddress = instructionStack.stackPointer - instructionStack.begin;
	
	// load variable, increment it and save it
	AddInstruction(&instructionStack, INS_LOAD_VAR_I);
    *instructionStack.stackPointer++ = 0 * sizeof(hsbint);
	AddInstruction(&instructionStack, INS_LITERAL_I);
    StoreIntFwd(&instructionStack.stackPointer, 1);
	AddInstruction(&instructionStack, INS_ADD_I);
	AddInstruction(&instructionStack, INS_SAVE_VAR_I);
    *instructionStack.stackPointer++ = 0 * sizeof(hsbint);
	
	// load both and compare them
	AddInstruction(&instructionStack, INS_LOAD_VAR_I);
    *instructionStack.stackPointer++ = 0 * sizeof(hsbint);
    AddInstruction(&instructionStack, INS_LOAD_VAR_I);
    *instructionStack.stackPointer++ = 1 * sizeof(hsbint);
	
	AddInstruction(&instructionStack, INS_CMP_I_EQ);
	AddInstruction(&instructionStack, INS_NEGATE_B);
	
	// if the values are not equal, jump back
	AddInstruction(&instructionStack, INS_COND_JUMP_B);
	StoreAddress(instructionStack.stackPointer, loopStartAddress);
	instructionStack.stackPointer += sizeof(hsbaddress);
	
	// load the variable (so the test could check the value)
	AddInstruction(&instructionStack, INS_LOAD_VAR_I);
    *instructionStack.stackPointer++ = 0 * sizeof(hsbint);
	
	// dealloc variables
	AddInstruction(&instructionStack, INS_DEALLOC_VAR_I);
	AddInstruction(&instructionStack, INS_DEALLOC_VAR_I);
	

    instructionStack.end = instructionStack.stackPointer;
    instructionStack.stackPointer = instructionStack.begin;

    FuncArray funcArray;

    InitVM(&vmData, instructionStack, DATA_SIZE, funcArray);

    while (VMProcessInstructions(&vmData, 1))
    {
        //printf(".");
    }

    testResult = LoadInt(vmData.dataStack.base.begin) == numIterations && vmData.dataStack.reversePointer == vmData.dataStack.base.end;

    DeleteVM(&vmData, HS_FALSE, HS_TRUE);

    printf("TestSimpleLoop: ");
    if (testResult)
    {
        printf("passed\n");
    }
    else
    {
        printf("FAILED\n");
    }

    return 1 - testResult;
}

int main()
{
    int fails = 0;
    fails += TestSimpleAdding();
    fails += TestVariable();
    fails += TestSimpleLoop();
	
	return fails;
}
