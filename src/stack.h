#include <stdlib.h>
#define DEFAULT_CAPACITY 10

typedef struct StackAttributes{
    Token *array;
    int capacity;
    int currentSize;
    Token nullToken;
}StackAttributes;

typedef struct Stack Stack;

struct Stack{
    StackAttributes attributes;
    void (*pPush)(Stack *, Token);
    void (*pIncrementSize)(Stack *);
    int (*pIsFull)(Stack *);
    int (*pSize)(Stack *);
    Token (*pPeek)(Stack *);
    Token (*pPop)(Stack *);
    int (*pIsEmpty)(Stack *);
    void (*p_free)(Stack *);
};

/*
 * Checks if the array is full
 */
int StackIsFull(Stack *stack){
    if (stack->attributes.currentSize == stack->attributes.capacity){
        return 1;
    } else{
        return 0;
    }
}

/*
 * Checks if the Stack is empty
 */
int IsEmpty(Stack *stack){
    if (stack->attributes.currentSize == 0){
        return 1;
    } else{
        return 0;
    }
}

/*
 * Doubles the size of the array
 * Dynamically reallocates space in the heap
 */
void StackIncrementSize(Stack *stack){
    Token *array = realloc(stack->attributes.array, sizeof(Token) * stack->attributes.capacity * 2);
    stack->attributes.array = array;
    stack->attributes.capacity *= 2;
}

/*
 * Adds a token to the end of the vector if the capacity permits,
 * otherwise it increments the capacity of the inner array and calls itself again
 */
void Push(Stack *stack, Token token){
    if (StackIsFull(stack) == 0){
        stack->attributes.array[stack->attributes.currentSize] = token;
        stack->attributes.currentSize++;
    } else{
        StackIncrementSize(stack);
        Push(stack,token);
    }
}

/*
 * @returns
 * the size of the vector , i.e. the part of the array that is being used.
 */
int StackSize(Stack *stack){
    return stack->attributes.currentSize;
}

Token Peek(Stack *stack){
    if (IsEmpty(stack) == 0){
        Token t = stack->attributes.array[stack->attributes.currentSize-1];
        return t;
    } else {
        return stack->attributes.nullToken;
    }
}

Token Pop(Stack *stack){
    if (IsEmpty(stack) == 0){
        Token t = stack->attributes.array[stack->attributes.currentSize-1];
        stack->attributes.currentSize--;
        return t;
    } else {
        return stack->attributes.nullToken;
    }
}

void freeStack(Stack *stack){
    free(stack->attributes.array);
}

void CreateStack(Stack *stack){
    stack->pSize = StackSize;
    stack->pIsFull = StackIsFull;
    stack->pIncrementSize = StackIncrementSize;
    stack->pIsEmpty = IsEmpty;
    stack->pPeek = Peek;
    stack->pPop = Pop;
    stack->pPush = Push;
    stack->p_free = freeStack;
    stack->attributes.currentSize = 0;
    stack->attributes.capacity = DEFAULT_CAPACITY;
    stack->attributes.array = malloc(sizeof(Token) * DEFAULT_CAPACITY);
    stack->attributes.nullToken.isOk = 0;
}
