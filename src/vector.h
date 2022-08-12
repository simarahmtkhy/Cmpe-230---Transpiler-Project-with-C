#include <stdlib.h>
#define DEFAULT_CAPACITY 10

typedef struct Vector Vector;
typedef struct VectorAttributes VectorAttributes; 

struct VectorAttributes{
    Token *array;       // an array of Tokens
    int capacity;       // the capacity of the array
    int currentSize;    //the current size of the array
    Token nullToken;
};


struct Vector{
    VectorAttributes attributes;        // the attributes of the vector
    // pointers to the functions we need
    void (*pAdd)(Vector *, Token);
    void (*pIncrementSize)(Vector *);
    int (*pIsFull)(Vector *);
    Token (*pGet)(Vector *, int index);
    int (*pSize)(Vector *);
    void (*p_update_type)(Vector *, int index, int type);
    void (*p_free)(Vector *);
    void (*p_update_vector)(Vector *, int index, int vector_size);
    void (*p_update_matrix)(Vector *, int index, int matrix_i, int matrix_j);
    void (*p_update_isok) (Vector *, int index, int isOk);
    void (*p_update_value) (Vector *vector, int index, char new_value[]);

};

/*
 * Checks if the array is full
 */
int IsFull(Vector *vector){
    if (vector->attributes.currentSize == vector->attributes.capacity){
        return 1;
    } else{
        return 0;
    }
}

/*
 * Doubles the size of the array
 * Dynamically reallocates space in the heap
 */
void IncrementSize(Vector *vector){
    Token *array = realloc(vector->attributes.array, sizeof(Token) * vector->attributes.capacity * 2);
    vector->attributes.array = array;
    vector->attributes.capacity *= 2;
}

/*
 * Adds a token to the end of the vector if the capacity permits,
 * otherwise it increments the capacity of the inner array and calls itself again
 */
void Add(Vector *vector, Token token){
    if (IsFull(vector) == 0){
        vector->attributes.array[vector->attributes.currentSize] = token;
        vector->attributes.currentSize++;
    } else{
        IncrementSize(vector);
        Add(vector, token);
    }
}

/*
 * @returns
 * the size of the vector , i.e. the part of the array that is being used.
 */
int Size(Vector *vector){
    return vector->attributes.currentSize;
}

/*
 * returns the value at the specified index
 */
Token Get(Vector *vector, int index){
    if (index < vector->attributes.currentSize){
        return vector->attributes.array[index];
    } else {
        return vector->attributes.nullToken;
    }
}


void updateType(Vector *vector, int index,int type){
    if (index < vector->attributes.currentSize){
        vector->attributes.array[index].type = type;
    }
}

void update_vector(Vector *vector, int index, int vector_size) {
    if (index < vector->attributes.currentSize){
        vector->attributes.array[index].vector = vector_size;
    }
}

void update_matrix_i_j(Vector *vector, int index, int matrix_i, int matrix_j) {
    if (index < vector->attributes.currentSize) {
        vector->attributes.array[index].matrix_i = matrix_i;
        vector->attributes.array[index].matrix_j = matrix_j;
    }
}

void update_isok(Vector *vector, int index, int isOk) {
    if (index < vector->attributes.currentSize){
        vector->attributes.array[index].isOk = isOk;
    }
}

void update_value(Vector *vector, int index, char new_value[]) {
    if (index < vector->attributes.currentSize){
        strcpy(vector->attributes.array[index].value, new_value);
    }
}

void freeVector(Vector *vector){
    free(vector->attributes.array);
}


/*
 * Initializes the Vector
 */
void CreateVector(Vector *vector) {
    vector->pAdd = Add;
    vector->pGet = Get;
    vector->pIncrementSize = IncrementSize;
    vector->pIsFull = IsFull;
    vector->pSize = Size;
    vector->p_update_type = updateType;
    vector->p_update_vector = update_vector;
    vector->p_update_matrix = update_matrix_i_j;
    vector->p_update_isok = update_isok;
    vector->p_update_value = update_value;
    vector->p_free = freeVector;
    vector->attributes.currentSize = 0;
    vector->attributes.capacity = DEFAULT_CAPACITY;
    vector->attributes.array = malloc(sizeof(Token) * DEFAULT_CAPACITY);
    vector->attributes.nullToken.isOk = 0;
}
