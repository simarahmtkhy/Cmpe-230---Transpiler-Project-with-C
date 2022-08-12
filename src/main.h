#include "token.h"
#include "vector.h"
#include "stack.h"

void extract_token(char *token_start, int token_len, int token_type);
void tokenizer(char line[]);
void error(int error_id);
void parser();
Token get_variable(char name[]);
int parse_expression(int start_index, Vector *tokens, int delimiter_type);
int is_variable(char name[]);
Token type_check(Token op1, Token op2, Token operator);
int has_higher_precedence(int incoming, int top);
int is_ok_ending(int start_index);
int get_expression(int start_index, int delimiter_type);
Token evaluate_postfix(Vector *postfix);
void initialize_scalar(char name[]);
void initialize_vector(char name[], int vector_size);
void initialize_matrix(char name[], int matrix_i, int matrix_j);
int expression(Vector *tokens, int start_index, int end_index, int expected_type);
Token infix_to_postfix(Vector *subtokens, int start, int end);
int parse_vector_matrix_initialization(int start_index, Token *expected_type);
void assign_type(Token *variable, int index);
void output_generator(FILE *out);
void tokens_status();
Token expression_type(Vector *tokens, int start_index, int end_index);
void initialize_for_loop_variable(char name[]);
void remove_for_variables();
int get_expression_output(int start_index, int delimiter_type);
int expression_2(Vector *tokens, int start_index, int end_index, int expected_type);
Token infix_to_postfix_2(Vector *subtokens, int start, int end);
Token expression_type_2(Vector *tokens, int start_index, int end_index);
Token evaluate_postfix_2(Vector *postfix);
Token type_check_2(Token op1, Token op2, Token operator);
int get_expression_output(int start_index, int delimiter_type);

