struct Token{
    char value[512];
    int type;
    int isOk;
    int matrix_i;
    int matrix_j;
    int vector;
};

typedef struct Token Token;

//https://www.javatpoint.com/convert-infix-to-postfix-notation

/*
    lookup table for int type
    0  -> scalar keyword
    1  -> vector keyword
    2  -> matrix keyword
    3  -> function keyword (tr, sqrt, choose)
    4  -> for keyword
    5  -> left paranthesis
    6  -> right paranthesis
    7  -> left square brace
    8  -> right square brace
    9  -> left curly brace
    10 -> right curly brace
    11 -> equals
    12 -> multiplication
    13 -> addition
    14 -> subtraction
    15 -> comma 
    16 -> colon
    17 -> undetermined
    18 -> comment
    19 -> scalar variable
    20 -> vector variable
    21 -> matrix variable
    22 -> for variable
    23 -> print function keyword(print, printsep)
    24 -> in keyword
    25 -> indexed matrix
    26 -> indexed vector
    27 -> scalar function keyword //choose sqrt tr
    28 -> matrix function keyword //tr

    Token  expression_type(Vector *tokens, int start_index, int end_index);
    int expression(Vector *tokens, int start_index, int end_index, int expected_type);
*/
