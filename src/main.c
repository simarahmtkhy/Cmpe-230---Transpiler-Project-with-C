#include <stdio.h>
#include <string.h>
#include "main.h"
#include "string_functions.h"
#include <math.h>

Vector tokens;
Vector variables;
Vector postfix_vector;
Vector for_variables;
int in_for_loop;
int line_number;
int num_error;
int flag;
int extra_curly;
int flag_for_loop;
FILE *file;
FILE *out;

//General pseudocode of the program
/*
process_line(char line[], File c_file) {
	tokens = tokenizer(line)
	parser(tokens)
	//no syntax error at this point
	c_line = line_builder(tokens)
	printf(C_file, c_line)
}

print(C.out, neededfunctions)
while(nextLine) {
	line
	process_line(line, C.out)
}

parser(Vector tokens) {
	expression(subTokens) //calls expression when necessarry
}

expression(Vector subTokens) {
	Vector postfixSubTokens = infixToPostfix(subTokens)
	evaluatePostfix(postfixSubTokens) 
}
*/

/**
 * @brief Tokenizes expressions. 
 * Takes a single line from the .mat file, returns a vector of tokens. 
 * Does not modify the initial line, uses two pointers to denote a token. When found, copies 
 * the token to the token struct. Uses token_type to keep track of which token is being parsed. 
 * @param line 
 */
void tokenizer(char line[]) { //                                                                                    NEED TO STRIPLAST WHITESPACE
    CreateVector(&tokens);

    char append = ' '; // needs an not important character at the end to read the last token.
    strncat(line, &append, 1);
    char *token_start = line;
    int line_len = strlen(line);
    int token_type = 0; //0: loopbegin, whitespace; 1: specialcharacter 2: alphanumeric
    int token_len = 0;
    char current;

    int index = 0;
    while (index < line_len) {
        char current = line[index]; 
        
        if (is_comment(current) == 1) {
            if (token_type == 0) {
                if (token_start == line) {
                    extract_token(token_start, line_len - index, 18);
                } else {
                    extract_token(token_start + 1, line_len - index, 18);
                }
            } else if (token_type == 1) {
                extract_token(token_start, token_len, 17);
                extract_token(token_start + 1, line_len - index, 18);
            } else if (token_type == 2) {
                extract_token(token_start, token_len, 17);
                extract_token(token_start + 1, line_len - index, 18);
            }
            break;
        }

        if (is_dot(current) == 1) {
            if (token_type == 2) {
                token_len++;
                index++;
                continue;
            } else {
                error(1);
                break;
            }
        }

        if (is_whitespace(current) == 1) {
            if (token_type == 0) {
                token_len = 0;
                token_start = line + index;
                index++;
            } else if (token_type == 1) {
                extract_token(token_start, token_len, 17);
                token_len = 0;
                token_start = line + index;
                index++;
            } else if (token_type == 2) {
                extract_token(token_start, token_len, 17);
                token_len = 0;
                token_start = line + index;
                index++;
            }   
            token_type = 0;     
            continue;
        }

        if (is_special_character(current) == 1) {
            if (token_type == 0) {
                token_len = 1;
                token_start = line + index;
                index++;
            } else if (token_type == 1) {
                extract_token(token_start, token_len, 17);
                token_len = 1;
                token_start = line + index;
                index++;
            } else if (token_type == 2) {
                extract_token(token_start, token_len, 17);
                token_len = 1;
                token_start = line + index;
                index++;
            }
            token_type = 1;
            continue;
        }

        if (is_alphanumeric(current) == 1) {
            if (token_type == 0) {
                token_len = 1;
                token_start = line + index;
                index++;
            } else if (token_type == 1) {
                extract_token(token_start, token_len, 17);
                token_len = 1;
                token_start = line + index;
                index++;
            } else if (token_type == 2) {
                token_len++;
                index++;
            }
            token_type = 2;
            continue;
        }

        error(2);
        break;
    }
}

/**
 * @brief Error functio print out an error to the output file when necessary.
 * error_id is used for the debugging phase.
 * 
 * @param error_id indicates which error() function was set off.
 */
void error(int error_id) {
    num_error++;
    //fprintf(out, "Error on line %d.\n", line_number);
    printf("Error on line %d.\n", line_number);
    // printf("\t\t\t\tError on line %d. Error ID: %d\n", line_number, error_id);
}

/*
    Returns -1 if there is an error. Assigns type and attribute values to tokens inside of an expression.
    Returns the index of the last token of the expression.
*/
int get_expression_output(int start_index, int delimiter_type) { //if delimiter not found, must stop at the last token
    int delimiter_stack_brace = 0;
    int delimiter_stack_paranthesis = 0;
    int tokens_size = tokens.pSize(&tokens); //Vector pointer to int conversion error
    char expression_string[512];
    char variable_string[256];
    memset(expression_string, '\0', sizeof(expression_string));
    memset(variable_string, '\0', sizeof(variable_string));
    
    int index = start_index;
    while(1) {
        Token token = tokens.pGet(&tokens, index);

        if (is_variable(token.value) == 1) {
            int variable_index = index;
        
            if (token.type == 26) {
                index++;
                index++;
                int expr_end_index = get_expression_output(index, 8); // RIGHT BRACE DELIMITER

                strcat(variable_string, "get_vector_elem(");
                strcat(variable_string, token.value);
                strcat(variable_string, ", ");
                Token expression = expression_type_2(&tokens, index, expr_end_index);
                strcat(variable_string, expression.value);
                strcat(variable_string, ")");

                tokens.p_update_value(&tokens, variable_index, variable_string);
                memset(variable_string, '\0', sizeof(variable_string));

                index = expr_end_index + 1; //check index

            } else if (token.type == 25) {
                index++;
                index++;
                int expr_end_index_1 = get_expression_output(index, 15); //                                                              COMMA COMMA COMMA DELIMITER

                strcat(variable_string, "get_matrix_elem(");
                strcat(variable_string, token.value);
                strcat(variable_string, ", ");
                Token expression_1 = expression_type_2(&tokens, index, expr_end_index_1);
                strcat(variable_string, expression_1.value);
                strcat(variable_string, ", ");

                int expr_end_index_2 = get_expression_output(expr_end_index_1 + 2, 8); //                                                              COMMA COMMA COMMA DELIMITER
                Token expression_2 = expression_type_2(&tokens, expr_end_index_1 + 2, expr_end_index_2);
                strcat(variable_string, expression_2.value);
                strcat(variable_string, ")");
                tokens.p_update_value(&tokens, variable_index, variable_string);
                memset(variable_string, '\0', sizeof(variable_string));

                index = expr_end_index_2 + 1;
            }
        } else if (is_function_keyword(token.value) == 1) {
            int keyword_index = index;

            if (strcmp(token.value, "choose") == 0) {
                index++;
                int expr_start_index_1 = index + 1;
                int expr_end_index_1 = get_expression_output(expr_start_index_1, 15); // COMMA DELIMITER

                strcat(expression_string, "choose(");
                Token expression_1 = expression_type_2(&tokens, expr_start_index_1, expr_end_index_1);
                strcat(expression_string, expression_1.value);
                strcat(expression_string, ", ");

                index = expr_end_index_1 + 1;
                int expr_start_index_2 = index + 1;
                int expr_end_index_2 = get_expression_output(expr_start_index_2, 15); // COMMA DELIMITER

                Token expression_2 = expression_type_2(&tokens, expr_start_index_2, expr_end_index_2);
                strcat(expression_string, expression_2.value);
                strcat(expression_string, ", ");
                
                index = expr_end_index_2 + 1;
                int expr_start_index_3 = index + 1;
                int expr_end_index_3 = get_expression_output(expr_start_index_3, 15); //COMMA DELIMITER

                Token expression_3 = expression_type_2(&tokens, expr_start_index_3, expr_end_index_3);
                strcat(expression_string, expression_3.value);
                strcat(expression_string, ", ");

                index = expr_end_index_3 + 1;
                int expr_start_index_4 = index + 1;
                int expr_end_index_4 = get_expression_output(expr_start_index_4, 6);
                Token expression_4 = expression_type_2(&tokens, expr_start_index_4, expr_end_index_4);
                strcat(expression_string, expression_4.value);
                strcat(expression_string, ")");

                index = expr_end_index_4 + 1;

            } else if (strcmp(token.value, "sqrt") == 0) {
                index++;
                index++;
                int expr_end_index = get_expression_output(index, 6);
                
                strcat(expression_string, "sqrt(");
                Token expression = expression_type_2(&tokens, index, expr_end_index);
                strcat(expression_string, expression.value);
                strcat(expression_string, ")");

                index = expr_end_index + 1;

            } else if (strcmp(token.value, "tr") == 0) {
                index++;
                index++;
                int expr_end_index = get_expression_output(index, 6); // COMMA DELIMITER

                Token expression = expression_type_2(&tokens, index, expr_end_index); 
                Token result = expression_type(&tokens, index, expr_end_index); //checks type manually
                if (result.type == 19 || result.type == 27) {
                    strcat(expression_string, expression.value);

                } else if (result.type == 20 || result.type == 21 || result.type == 28) {
                    strcat(expression_string, "transpose(");
                    strcat(expression_string, expression.value);
                    strcat(expression_string, ")");
                }

                index = expr_end_index + 1;
            }
            tokens.p_update_value(&tokens, keyword_index, expression_string);
            memset(expression_string, '\0', sizeof(expression_string));
        
        } else if (is_comment(token.value[0]) == 1) { //COMMENT DELIMITER
            return index - 1;

        } else if (is_single_comma(token.value) == 1) { //COMMA DELIMITER
            return index - 1;

        } else if (is_single_colon(token.value) == 1) { //COLON DELIMITER
            return index - 1;

        } else if (is_single_left_paranthesis(token.value) == 1) {
            if (delimiter_type == 6) {
                delimiter_stack_paranthesis++;
            }

        } else if (is_single_right_paranthesis(token.value) == 1) { //RIGHT PARANTHESIS DELIMITER
            if (delimiter_type == 6) {
                if (delimiter_stack_paranthesis == 0) {
                    return index - 1;
                } else {
                    delimiter_stack_paranthesis--;
                }
            }
            
        } else if (is_single_left_brace(token.value) == 1) {
            if (delimiter_type == 8) {
                delimiter_stack_brace++;
            }
        
        } else if (is_single_right_brace(token.value) == 1) { // RIGHT SQUARE BRACE DELIMITER
            if (delimiter_type == 8) {
                if (delimiter_stack_brace == 0) {
                    return index - 1;
                } else {
                    delimiter_stack_brace--;
                }
            }

        } else {
            if (index == tokens.pSize(&tokens)) { //END OF TOKENS AND WE NEED TO RETURN; NO COMMENT FOUND
                return index - 1;
            }
        }

        index++;
        if (index == tokens_size) {
            if (delimiter_type == 18) {
               index--;
            }
            return index;
        }
    }
}

int get_expression(int start_index, int delimiter_type) { //if delimiter not found, must stop at the last token
    int delimiter_stack_brace = 0;
    int delimiter_stack_paranthesis = 0;
    int tokens_size = tokens.pSize(&tokens); //Vector pointer to int conversion error
    if (tokens_size == start_index) {
        return -1;
    }
    
    int index = start_index;
    while(1) {
        Token token = tokens.pGet(&tokens, index);
        if (token.isOk != 1) {
            return -1;
        }
        
        if (is_variable(token.value) == 1) {
            assign_type(&token, index);
            token = tokens.pGet(&tokens, index);
            int variable_index = index;

            Token left_brace_check = tokens.pGet(&tokens, index + 1);
            if (is_single_left_brace(left_brace_check.value) == 1) {

                if (token.type == 20) {
                    index++;
                    Token left_brace = tokens.pGet(&tokens, index);
                    if (is_single_character(left_brace.value) != 1) {
                        return -1;
                    }
                    if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
                        return -1;
                    }
                    tokens.p_update_type(&tokens, index, 7);
                    left_brace.type = 7;

                    index++;
                    int expr_end_index = get_expression(index, 8); // RIGHT BRACE DELIMITER
                    if (expr_end_index == -1) {
                        return -1;
                    }
                    if (expression(&tokens, index, expr_end_index, 19) != 1 && expression(&tokens, index, expr_end_index, 25) != 1 && expression(&tokens, index, expr_end_index, 26) != 1 && expression(&tokens, index, expr_end_index, 27) != 1) {
                        return -1;
                    }   
                    
                    //index = expr_end_index; //check index
                    Token right_brace = tokens.pGet(&tokens, expr_end_index + 1);
                    if (is_single_character(right_brace.value) != 1) {
                        return -1;
                    }
                    if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
                        return -1;
                    }
                    tokens.p_update_type(&tokens, expr_end_index + 1, 8);
                    right_brace.type = 8;
                    index = expr_end_index + 1; //check index
                    tokens.p_update_type(&tokens, variable_index, 26);

                } else if (token.type == 21) {
                    index++;
                    Token left_brace = tokens.pGet(&tokens, index);
                    if (is_single_character(left_brace.value) != 1) {
                        return -1;
                    }
                    if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
                        return -1;
                    }
                    tokens.p_update_type(&tokens, index, 7);
                    left_brace.type = 7;

                    index++;
                    int expr_end_index_1 = get_expression(index, 15); //                                                              COMMA COMMA COMMA DELIMITER
                    if (expr_end_index_1 == -1) {
                        return -1;
                    }
                    if (expression(&tokens, index, expr_end_index_1, 19) != 1 && expression(&tokens, index, expr_end_index_1, 25) != 1 && expression(&tokens, index, expr_end_index_1, 26) != 1 && expression(&tokens, index, expr_end_index_1, 27) != 1) {
                        return -1;
                    }
               
                    Token comma = tokens.pGet(&tokens, expr_end_index_1 + 1);
                    if (is_single_character(comma.value) != 1) {
                        return -1;
                    }
                    if (is_comma(comma.value[0]) != 1 || comma.isOk != 1) {
                        return -1;
                    }
                    tokens.p_update_type(&tokens, expr_end_index_1 + 1, 15);                       //CURRENTLY SENDING RIGHT AND LEFT SQUARE BRACES TO EXPRESSION
                    comma.type = 15;

                    int expr_end_index_2 = get_expression(expr_end_index_1 + 2, 8); //                                                              COMMA COMMA COMMA DELIMITER
                    if (expr_end_index_2 == -1) {
                        return -1;
                    }
                    if (expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 19) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 20) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 21) != 1) {
                        return -1;
                    }
                    Token right_brace = tokens.pGet(&tokens, expr_end_index_2 + 1);
                    if (is_single_character(right_brace.value) != 1) {
                        return -1;
                    }
                    if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
                        return -1;
                    }
                    tokens.p_update_type(&tokens, expr_end_index_2 + 1, 8);
                    right_brace.type = 8;
                    index = expr_end_index_2 + 1;
                    tokens.p_update_type(&tokens, variable_index, 25);

                } else {
                    return -1;
                }
            }
        } else if (is_number_literal(token.value) == 1 || is_float(token.value) == 1) {
            tokens.p_update_type(&tokens, index, 19);
            token.type = 19;

        } else if (is_function_keyword(token.value) == 1) {
            tokens.p_update_type(&tokens, index, 3);
            token.type = 3;
            int keyword_index = index;

            if (strcmp(token.value, "choose") == 0) {
                tokens.p_update_type(&tokens, keyword_index, 27);
                token.type = 27;

                index++;
                Token left_paranthesis = tokens.pGet(&tokens, index);
                if (is_single_character(left_paranthesis.value) != 1) {
                    return -1;
                }
                if (is_left_paranthesis(left_paranthesis.value[0]) != 1 || left_paranthesis.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 5);
                left_paranthesis.type = 5;

                int expr_start_index_1 = index + 1;
                int expr_end_index_1 = get_expression(expr_start_index_1, 15); // COMMA DELIMITER
                if (expr_end_index_1 == -1) {
                    return -1;
                }
                if (expression(&tokens, expr_start_index_1, expr_end_index_1, 19) != 1 && expression(&tokens, expr_start_index_1, expr_end_index_1, 25) != 1 && expression(&tokens, expr_start_index_1, expr_end_index_1, 26) != 1 && expression(&tokens, expr_start_index_1, expr_end_index_1, 27) != 1) {
                    return -1;
                }
                
                index = expr_end_index_1 + 1;
                Token comma_1 = tokens.pGet(&tokens, index);
                if (is_single_character(comma_1.value) != 1) {
                    return -1;
                }
                if (is_comma(comma_1.value[0]) != 1 || comma_1.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 15);
                comma_1.type = 15;

                int expr_start_index_2 = index + 1;
                int expr_end_index_2 = get_expression(expr_start_index_2, 15); // COMMA DELIMITER
                if (expr_end_index_2 == -1) {
                    return -1;
                }
                if (expression(&tokens, expr_start_index_2, expr_end_index_2, 19) != 1 && expression(&tokens, expr_start_index_2, expr_end_index_2, 25) != 1 && expression(&tokens, expr_start_index_2, expr_end_index_2, 26) != 1 && expression(&tokens, expr_start_index_2, expr_end_index_2, 27) != 1) {
                    return -1;
                }

                index = expr_end_index_2 + 1;
                Token comma_2 = tokens.pGet(&tokens, index);
                if (is_single_character(comma_2.value) != 1) {
                    return -1;
                }
                if (is_comma(comma_2.value[0]) != 1 || comma_2.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 15);
                comma_2.type = 15;

                int expr_start_index_3 = index + 1;
                int expr_end_index_3 = get_expression(expr_start_index_3, 15); //COMMA DELIMITER
                if (expr_end_index_3 == -1) {
                    return -1;
                }
                if (expression(&tokens, expr_start_index_3, expr_end_index_3, 19) != 1 && expression(&tokens, expr_start_index_3, expr_end_index_3, 25) != 1 && expression(&tokens, expr_start_index_3, expr_end_index_3, 26) != 1 && expression(&tokens, expr_start_index_3, expr_end_index_3, 27) != 1) {
                    return -1;
                }

                index = expr_end_index_3 + 1;
                Token comma_3 = tokens.pGet(&tokens, index);
                if (is_single_character(comma_3.value) != 1) {
                    return -1;
                }
                if (is_comma(comma_3.value[0]) != 1 || comma_3.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 15);
                comma_3.type = 15;

                int expr_start_index_4 = index + 1;
                int expr_end_index_4 = get_expression(expr_start_index_4, 6);
                if (expr_end_index_4 == -1) {
                    return -1;
                }
                if (expression(&tokens, expr_start_index_4, expr_end_index_4, 19) != 1 && expression(&tokens, expr_start_index_4, expr_end_index_4, 25) != 1 && expression(&tokens, expr_start_index_4, expr_end_index_4, 26) != 1 && expression(&tokens, expr_start_index_4, expr_end_index_4, 27) != 1) {
                    return -1;
                }

                index = expr_end_index_4 + 1;
                Token right_paranthesis = tokens.pGet(&tokens, index);
                if (is_single_character(right_paranthesis.value) != 1) {
                    return -1;
                }
                if (is_right_paranthesis(right_paranthesis.value[0]) != 1 || right_paranthesis.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 6);
                right_paranthesis.type = 6;

            } else if (strcmp(token.value, "sqrt") == 0) {
                tokens.p_update_type(&tokens, keyword_index, 27);
                token.type = 27;

                index++;
                Token left_paranthesis = tokens.pGet(&tokens, index);
                if (is_single_character(left_paranthesis.value) != 1) {
                    return -1;
                }
                if (is_left_paranthesis(left_paranthesis.value[0]) != 1 || left_paranthesis.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 5);
                left_paranthesis.type = 5;

                index++;
                int expr_end_index = get_expression(index, 6);
                if (expr_end_index == -1) {
                    printf("in1\n");
                    return -1;
                }
                if (expression(&tokens, index, expr_end_index, 19) != 1 && expression(&tokens, index, expr_end_index, 25) != 1 && expression(&tokens, index, expr_end_index, 26) != 1 && expression(&tokens, index, expr_end_index, 27) != 1) {
                    printf("in2\n");
                    return -1;
                }
                
                index = expr_end_index + 1;
                Token right_paranthesis = tokens.pGet(&tokens, index);
                if (is_single_character(right_paranthesis.value) != 1) {
                    return -1;
                }
                if (is_right_paranthesis(right_paranthesis.value[0]) != 1 || right_paranthesis.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 6);
                right_paranthesis.type = 6;



            } else if (strcmp(token.value, "tr") == 0) {
                index++;
                Token left_paranthesis = tokens.pGet(&tokens, index);
                if (is_single_character(left_paranthesis.value) != 1) {
                    return -1;
                }
                if (is_left_paranthesis(left_paranthesis.value[0]) != 1 || left_paranthesis.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 5);
                left_paranthesis.type = 5;

                index++;
                int expr_end_index = get_expression(index, 6); // COMMA DELIMITER
                if (expr_end_index == -1) {
                    return -1;
                }
                Token result = expression_type(&tokens, index, expr_end_index); //checks type manually
                if (result.type == 19 || result.type == 27) {
                    tokens.p_update_type(&tokens, keyword_index, 27);

                } else if (result.type == 20 || result.type == 28) {
                    tokens.p_update_type(&tokens, keyword_index, 28);
                    tokens.p_update_matrix(&tokens, keyword_index, 1, result.vector);

                } else if (result.type == 21 || result.type == 28) {
                    tokens.p_update_type(&tokens, keyword_index, 28);
                    tokens.p_update_matrix(&tokens, keyword_index, result.matrix_j, result.matrix_j);
                } else {
                    return -1;
                }

                index = expr_end_index + 1;
                Token right_paranthesis = tokens.pGet(&tokens, index);
                if (is_single_character(right_paranthesis.value) != 1) {
                    return -1;
                }
                if (is_right_paranthesis(right_paranthesis.value[0]) != 1 || right_paranthesis.isOk != 1) {
                    return -1;
                }
                tokens.p_update_type(&tokens, index, 6);
                right_paranthesis.type = 6;
            }

        } else if (is_comment(token.value[0]) == 1) { //COMMENT DELIMITER
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 18);
            token.type = 18;
            if (delimiter_type != token.type) {
                return -1;
            }
            return index - 1;

        } else if (is_single_comma(token.value) == 1) { //COMMA DELIMITER
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 15);
            token.type = 15;

            if (delimiter_type != token.type) {
                return -1;
            }
            return index - 1;

        } else if (is_single_colon(token.value) == 1) { //COLON DELIMITER
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 16);
            token.type = 16;

            if (delimiter_type != token.type) {
                return -1;
            }
            return index - 1;

        } else if (is_single_left_paranthesis(token.value) == 1) {
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 5);
            token.type = 5;
            if (delimiter_type == 6) {
                delimiter_stack_paranthesis++;
            }

        } else if (is_single_right_paranthesis(token.value) == 1) { //RIGHT PARANTHESIS DELIMITER
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 6);
            token.type = 6;
            if (delimiter_type == 6) {
                if (delimiter_stack_paranthesis == 0) {
                    return index - 1;
                } else {
                    delimiter_stack_paranthesis--;
                }
            }
        } else if (is_single_right_curly_brace(token.value) == 1) { //RIGHT CURLY BRACE DELIMITER
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 10);
            token.type = 10;

            if (delimiter_type != token.type) {
                return -1;
            }
            return index - 1;

        } else if (is_single_left_brace(token.value) == 1) {
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 8);
            token.type = 8;
            if (delimiter_type == 8) {
                delimiter_stack_brace++;
            }
        
        } else if (is_single_right_brace(token.value) == 1) { // RIGHT SQUARE BRACE DELIMITER
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 8);
            token.type = 8;
            if (delimiter_type == 8) {
                if (delimiter_stack_brace == 0) {
                    return index - 1;
                } else {
                    delimiter_stack_brace--;
                }
            }

        } else if (is_single_star(token.value) == 1) {
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 12);
            token.type = 12;

        } else if (is_single_plus(token.value) == 1) {
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 13);
            token.type = 13;

        } else if (is_single_minus(token.value) == 1) {
            if (token.isOk != 1) {
                return -1;
            }
            tokens.p_update_type(&tokens, index, 14);
            token.type = 14;

        } else {
            if (index == tokens.pSize(&tokens)) { //END OF TOKENS AND WE NEED TO RETURN; NO COMMENT FOUND
                return index - 1;
            } else {
                return -1;
            }
        }
        index++;
        if (index == tokens_size) {
            if (delimiter_type == 18) {
               index--;
            }
            return index;
        }
    }
}


//parser -> assignment, decleration (with keyword), for loop, print(), printsep(), 
//needs expression parser, expression parser is recursive.
//parser is not recursive.
//function_parser is used as the last part (else) of these two functions.7
//function_parser is not recursive, but it calls expression parser, which is.

void parser() {
    if (tokens.pSize(&tokens) == 0) {
        return;
    }
    Token token = tokens.pGet(&tokens, 0); //checking first token
    if (in_for_loop == 1 && is_single_right_curly_brace(token.value) == 1) {
        tokens.p_update_type(&tokens, 0, 10);
        token.type = 10;
        in_for_loop--;
        remove_for_variables();
        
        if (is_ok_ending(1) != 1) {
            error(15);
            return;
        }

    } else if (strcmp("scalar", token.value) == 0) {
        tokens.p_update_type(&tokens, 0, 0);

        Token variable = tokens.pGet(&tokens, 1);
        if (is_alphanumeric_literal(variable.value) != 1 || variable.isOk != 1) {
            error(16);
            return;
        }
        tokens.p_update_type(&tokens, 1, 19);
        if (is_variable(variable.value) == 1) {
            error(128);
            return;
        }
        initialize_scalar(variable.value); 

        if (is_ok_ending(2) != 1) {
            error(17);
            return;
        }

    } else if (strcmp("vector", token.value) == 0) {
        tokens.p_update_type(&tokens, 0, 1);

        Token variable = tokens.pGet(&tokens, 1);
        if (is_alphanumeric_literal(variable.value) != 1 || variable.isOk != 1) {
            error(18);
            return;
        }
        tokens.p_update_type(&tokens, 1, 20);

        Token left_brace = tokens.pGet(&tokens, 2);
        if (is_single_character(left_brace.value) != 1) {
            error(19);
            return;
        }
        if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
            error(20);
            return;
        }
        tokens.p_update_type(&tokens, 2, 7);

        Token vector_size = tokens.pGet(&tokens, 3);
        if (is_number_literal(vector_size.value) != 1 || vector_size.isOk != 1) {
            error(21);
            return;
        }
        tokens.p_update_type(&tokens, 3, 19);
        int integer_vector_size = atoi(vector_size.value);
        if (is_variable(variable.value) == 1) {
            error(129);
            return;
        }
        initialize_vector(variable.value, integer_vector_size);
        
        Token right_brace = tokens.pGet(&tokens, 4);
        if (is_single_character(right_brace.value) != 1) {
            error(22);
            return;
        }
        if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
            error(23);
            return;
        }
        tokens.p_update_type(&tokens, 4, 8);

        if (is_ok_ending(5) != 1) {
            error(24);
            return;
        }

    } else if (strcmp("matrix", token.value) == 0) {
        tokens.p_update_type(&tokens, 0, 2);

        Token variable = tokens.pGet(&tokens, 1);
        if (is_alphanumeric_literal(variable.value) != 1 || variable.isOk != 1) {
            error(25);
            return;
        }
        tokens.p_update_type(&tokens, 1, 21);

        Token left_brace = tokens.pGet(&tokens, 2);
        if (is_single_character(left_brace.value) != 1) {
            error(26);
            return;
        }
        if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
            error(27);
            return;
        }
        tokens.p_update_type(&tokens, 2, 7);

        Token matrix_size_i = tokens.pGet(&tokens, 3);
        if (is_number_literal(matrix_size_i.value) != 1 || matrix_size_i.isOk != 1) {
            error(28);
            return;
        }
        tokens.p_update_type(&tokens, 3, 19);
        int integer_matrix_size_i = atoi(matrix_size_i.value);

        Token comma = tokens.pGet(&tokens, 4);
        if (is_single_character(comma.value) != 1) {
            error(29);
            return;
        }
        if (is_comma(comma.value[0]) != 1 || comma.isOk != 1) {
            error(30);
            return;
        }
        tokens.p_update_type(&tokens, 4, 15);

        Token matrix_size_j = tokens.pGet(&tokens, 5);
        if (is_number_literal(matrix_size_j.value) != 1 || matrix_size_j.isOk != 1) {
            error(31);
            return;
        }
        tokens.p_update_type(&tokens, 5, 19);
        int integer_matrix_size_j = atoi(matrix_size_j.value);
        if (is_variable(variable.value) == 1) {
            error(130);
            return;
        }
        initialize_matrix(variable.value, integer_matrix_size_i, integer_matrix_size_j);

        Token right_brace = tokens.pGet(&tokens, 6);
        if (is_single_character(right_brace.value) != 1) {
            error(32);
            return;
        }
        if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
            error(33);
            return;
        }
        tokens.p_update_type(&tokens, 6, 8);

        if (is_ok_ending(7) != 1) {
            error(34);
            return;
        }

    } else if (strcmp("for", token.value) == 0) { // FOR LOOP DOESN'T WORK // EXPRESSION END INDEX AND NOT TAKING COLONS
        tokens.p_update_type(&tokens, 0, 4);

        Token left_paranthesis = tokens.pGet(&tokens, 1);
        if (is_single_character(left_paranthesis.value) != 1) {
            error(35);
            return;
        }
        if(is_left_paranthesis(left_paranthesis.value[0]) != 1 || left_paranthesis.isOk != 1) {
            error(36);
            return;
        }
        tokens.p_update_type(&tokens, 1, 5);

        Token variable_1 = tokens.pGet(&tokens, 2);
        if (is_alphanumeric_literal(variable_1.value) != 1 || variable_1.isOk != 1) {
            error(37);
            return;
        }
        tokens.p_update_type(&tokens, 2, 19);
        initialize_scalar(variable_1.value);
        initialize_for_loop_variable(variable_1.value);

        int right_paranthesis_index;
        Token in = tokens.pGet(&tokens, 3);
        if (strcmp(in.value, "in") == 0 && in.isOk == 1) {
            in.type = 24;
            tokens.p_update_type(&tokens, 3, 24);

            int expr_end_1 = get_expression(4, 16);
            if (expr_end_1 == -1) {
                error(38);
                return;
            }
            if (expression(&tokens, 4, expr_end_1, 19) != 1 && expression(&tokens, 4, expr_end_1, 25) != 1 && expression(&tokens, 4, expr_end_1, 26) != 1 && expression(&tokens, 4, expr_end_1, 27) != 1) {
                error(39);
                return;
            }

            Token colon_1 = tokens.pGet(&tokens, expr_end_1 + 1); 
            if (is_single_character(colon_1.value) != 1) {
                error(40);
                return;
            }
            if (is_colon(colon_1.value[0]) != 1 || colon_1.isOk != 1) {
                error(41);
                return;
            }
            tokens.p_update_type(&tokens, expr_end_1 + 1, 16);
            
            int expr_end_2 = get_expression(expr_end_1 + 2, 16);
            if (expr_end_2 == -1) {
                error(42);
                return;
            }
            if (expression(&tokens, expr_end_1 + 2, expr_end_2, 19) != 1 && expression(&tokens, expr_end_1 + 2, expr_end_2, 25) != 1 && expression(&tokens, expr_end_1 + 2, expr_end_2, 26) != 1 && expression(&tokens, expr_end_1 + 2, expr_end_2, 27) != 1) {
                error(43);
                return;
            }

            Token colon_2 = tokens.pGet(&tokens, expr_end_2 + 1); 
            if (is_single_character(colon_2.value) != 1) {
                error(44);
                return;
            }
            if (is_colon(colon_2.value[0]) != 1 || colon_2.isOk != 1) {
                error(45);
                return;
            }
            tokens.p_update_type(&tokens, expr_end_2 + 1, 16);

            int expr_end_3 = get_expression(expr_end_2 + 2, 6);
            if (expr_end_3 == -1) {
                error(46);
                return;
            }
            if (expression(&tokens, expr_end_2 + 2, expr_end_3, 19) != 1 && expression(&tokens, expr_end_2 + 2, expr_end_3, 25) != 1 && expression(&tokens, expr_end_2 + 2, expr_end_3, 26) != 1 && expression(&tokens, expr_end_2 + 2, expr_end_3, 27) != 1) {
                error(47);
                return;
            }
            right_paranthesis_index = expr_end_3 + 1;

        } else {
            Token comma_1 = tokens.pGet(&tokens, 3);
            if (is_single_character(comma_1.value) != 1) {
                error(48);
                return;
            }
            if (is_comma(comma_1.value[0]) != 1 || comma_1.isOk != 1) {
                error(49);
                return;
            }
            tokens.p_update_type(&tokens, 3, 15);

            Token variable_2 = tokens.pGet(&tokens, 4);
            if (is_alphanumeric_literal(variable_2.value) != 1 || variable_2.isOk != 1) {
                error(50);
                return;
            }
            tokens.p_update_type(&tokens, 4, 19);
            initialize_for_loop_variable(variable_2.value);
            initialize_scalar(variable_2.value); //                             NEED TO INDICATE THAT THIS IS A FOR LOOP SCALAR

            Token in = tokens.pGet(&tokens, 5);
            if (strcmp(in.value, "in") != 0 || in.isOk != 1) {
                error(51);
                return;
            }
            tokens.p_update_type(&tokens, 5, 24);

            int expr_end_1 = get_expression(6, 16);
            if (expr_end_1 == -1) {
                error(52);
                return;
            }
            if (expression(&tokens, 6, expr_end_1, 19) != 1 && expression(&tokens, 6, expr_end_1, 25) != 1 && expression(&tokens, 6, expr_end_1, 26) != 1 && expression(&tokens, 6, expr_end_1, 27) != 1) {
                error(53);
                return;
            }

            Token colon_1 = tokens.pGet(&tokens, expr_end_1 + 1); 
            if (is_single_character(colon_1.value) != 1) {
                error(54);
                return;
            }
            if (is_colon(colon_1.value[0]) != 1 || colon_1.isOk != 1) {
                error(55);
                return;
            }
            tokens.p_update_type(&tokens, expr_end_1 + 1, 16);

            int expr_end_2 = get_expression(expr_end_1 + 2, 16);
            if (expr_end_2 == -1) {
                error(56);
                return;
            }
            if (expression(&tokens, expr_end_1 + 2, expr_end_2, 19) != 1 && expression(&tokens, expr_end_1 + 2, expr_end_2, 25) != 1 && expression(&tokens, expr_end_1 + 2, expr_end_2, 26) != 1 && expression(&tokens, expr_end_1 + 2, expr_end_2, 27) != 1) {
                error(57);
                return;
            }

            Token colon_2 = tokens.pGet(&tokens, expr_end_2 + 1); 
            if (is_single_character(colon_2.value) != 1) {
                error(58);
                return;
            }
            if (is_colon(colon_2.value[0]) != 1 || colon_2.isOk != 1) {
                error(59);
                return;
            }
            tokens.p_update_type(&tokens, expr_end_2 + 1, 16);

            int expr_end_3 = get_expression(expr_end_2 + 2, 15);
            if (expr_end_3 == -1) {
                error(60);
                return;
            }
            if (expression(&tokens, expr_end_2 + 2, expr_end_3, 19) != 1 && expression(&tokens, expr_end_2 + 2, expr_end_3, 25) != 1 && expression(&tokens, expr_end_2 + 2, expr_end_3, 26) != 1 && expression(&tokens, expr_end_2 + 2, expr_end_3, 27) != 1) {
                error(61);
                return;
            }

            Token comma_2 = tokens.pGet(&tokens, expr_end_3 + 1);
            if (is_single_character(comma_2.value) != 1) {
                error(62);
                return;
            }
            if (is_comma(comma_2.value[0]) != 1 || comma_2.isOk != 1) {
                error(63);
                return;
            }
            tokens.p_update_type(&tokens, expr_end_3 + 1, 15);

            int expr_end_4 = get_expression(expr_end_3 + 2, 16);
            if (expr_end_4 == -1) {
                error(64);
                return;
            }
            if (expression(&tokens, expr_end_3 + 2, expr_end_4, 19) != 1 && expression(&tokens, expr_end_3 + 2, expr_end_4, 25) != 1 && expression(&tokens, expr_end_3 + 2, expr_end_4, 26) != 1 && expression(&tokens, expr_end_3 + 2, expr_end_4, 27) != 1) {
                error(65);
                return;
            }

            Token colon_3 = tokens.pGet(&tokens, expr_end_4 + 1);
            if (is_single_character(colon_3.value) != 1) {
                error(66);
                return;
            }
            if (is_colon(colon_3.value[0]) != 1 || colon_3.isOk != 1) {
                error(67);
                return;
            }
            tokens.p_update_type(&tokens, expr_end_4 + 1, 16);

            int expr_end_5 = get_expression(expr_end_4 + 2, 16); ///ADD COMMA AS DELIMITER IN GET_EXPRESSION()
            if (expr_end_5 == -1) {
                error(68);
                return;
            }
            if (expression(&tokens, expr_end_4 + 2, expr_end_5, 19) != 1 && expression(&tokens, expr_end_4 + 2, expr_end_5, 25) != 1 && expression(&tokens, expr_end_4 + 2, expr_end_5, 26) != 1 && expression(&tokens, expr_end_4 + 2, expr_end_5, 27) != 1) {
                error(69);
                return;
            }

            Token colon_4 = tokens.pGet(&tokens, expr_end_5 + 1);
            if (is_single_character(colon_4.value) != 1) {
                error(70);
                return;
            }
            if (is_colon(colon_4.value[0]) != 1 || colon_4.isOk != 1) {
                error(71);
                return;
            }

            int expr_end_6 = get_expression(expr_end_5 + 2, 6);
            if (expr_end_6 == -1) {
                error(72);
                return;
            }
            if (expression(&tokens, expr_end_5 + 2, expr_end_6, 19) != 1 && expression(&tokens, expr_end_5 + 2, expr_end_6, 25) != 1 && expression(&tokens, expr_end_5 + 2, expr_end_6, 26) != 1 && expression(&tokens, expr_end_5 + 2, expr_end_6, 27) != 1) {
                error(73);
                return;
            }

            right_paranthesis_index = expr_end_6 + 1;
        }

        Token right_paranthesis = tokens.pGet(&tokens, right_paranthesis_index);
        if (is_single_character(right_paranthesis.value) != 1) {
            error(74);
            return;
        }
        if (is_right_paranthesis(right_paranthesis.value[0]) != 1 || right_paranthesis.isOk != 1) {
            error(75);
            return;
        }
        tokens.p_update_type(&tokens, right_paranthesis_index, 6);

        Token left_curly_brace = tokens.pGet(&tokens, right_paranthesis_index + 1);
        if (is_single_character(right_paranthesis.value) != 1) {
            error(76);
            return;
        }
        if (is_left_curly_brace(left_curly_brace.value[0]) != 1 || left_curly_brace.isOk != 1) {
            error(77);
            return;
        }
        tokens.p_update_type(&tokens, right_paranthesis_index + 1, 9);
        in_for_loop = 1;

        if (is_ok_ending(right_paranthesis_index + 2) != 1) {
            error(78);
            return;
        }

    } else if (is_variable(token.value) == 1) { // VARIABLE PARSER (ASSIGNMENT STATEMENT)
        assign_type(&token, 0);
        token = tokens.pGet(&tokens, 0);
        int variable_index = 0;

        int index = 0;
        Token left_brace_check = tokens.pGet(&tokens, index + 1);
        if (is_single_left_brace(left_brace_check.value) == 1) {                
            if (token.type == 20) {
                index++;
                Token left_brace = tokens.pGet(&tokens, index);
                if (is_single_character(left_brace.value) != 1) {
                    error(150);
                    return;
                }
                if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
                    error(151);
                    return;
                }
                tokens.p_update_type(&tokens, index, 7);
                left_brace.type = 7;

                index++;
                int expr_end_index = get_expression(index, 8); // RIGHT BRACE DELIMITER
                if (expr_end_index == -1) {
                    error(152);
                    return;
                }
                if (expression(&tokens, index, expr_end_index, 19) != 1 && expression(&tokens, index, expr_end_index, 26) != 1 && expression(&tokens, index, expr_end_index, 25) != 1 && expression(&tokens, index, expr_end_index, 27) != 1) {
                    error(153);
                    return;
                }   
                
                Token right_brace = tokens.pGet(&tokens, expr_end_index + 1);
                if (is_single_character(right_brace.value) != 1) {
                    error(154);
                    return;
                }
                if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
                    error(155);
                    return;
                }
                tokens.p_update_type(&tokens, expr_end_index + 1, 8);
                right_brace.type = 8;

                index = expr_end_index + 1; //check index // CHECK INDEXXXXX
                tokens.p_update_type(&tokens, variable_index, 26);
                token = tokens.pGet(&tokens, variable_index);

            } else if (token.type == 21) {
                index++;
                Token left_brace = tokens.pGet(&tokens, index);
                if (is_single_character(left_brace.value) != 1) {
                    error(156);
                    return;
                }
                if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
                    error(157);
                    return;
                }
                tokens.p_update_type(&tokens, index, 7);
                left_brace.type = 7;

                index++;
                int expr_end_index_1 = get_expression(index, 15); //                                                              COMMA COMMA COMMA DELIMITER
                if (expr_end_index_1 == -1) {
                    error(158);
                    return;
                }
                //if (expression(&tokens, index, expr_end_index_1, 19) != 1) 
                if (expression(&tokens, index, expr_end_index_1, 19) != 1 && expression(&tokens, index, expr_end_index_1, 25) != 1 && expression(&tokens, index, expr_end_index_1, 26) != 1 && expression(&tokens, index, expr_end_index_1, 27) != 1) {
                    error(159);
                    return;
                }
            
                Token comma = tokens.pGet(&tokens, expr_end_index_1 + 1);
                if (is_single_character(comma.value) != 1) {
                    error(160);
                    return;
                }
                if (is_comma(comma.value[0]) != 1 || comma.isOk != 1) {
                    error(170);
                    return;
                }
                tokens.p_update_type(&tokens, expr_end_index_1 + 1, 15);                       //CURRENTLY SENDING RIGHT AND LEFT SQUARE BRACES TO EXPRESSION
                comma.type = 15;

                int expr_end_index_2 = get_expression(expr_end_index_1 + 2, 8); //                                                              COMMA COMMA COMMA DELIMITER
                if (expr_end_index_2 == -1) {
                    error(171);
                    return;
                }
                if (expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 19) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 25) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 26) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 27) != 1) {
                    error(172);
                    return;
                }
                Token right_brace = tokens.pGet(&tokens, expr_end_index_2 + 1);
                if (is_single_character(right_brace.value) != 1) {
                    error(173);
                    return;
                }
                if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
                    error(174);
                    return;
                }
                tokens.p_update_type(&tokens, expr_end_index_2 + 1, 8);
                right_brace.type = 8;

                index = expr_end_index_2 + 1; // CHECK INDEX
                tokens.p_update_type(&tokens, variable_index, 25);
                token = tokens.pGet(&tokens, variable_index);


            } else {
                error(175);
                return;
            }
        }

        Token equals = tokens.pGet(&tokens, index + 1);
        if (is_single_character(equals.value) != 1) {
            error(79);
            return;
        }
        if (is_equals(equals.value[0]) != 1 || equals.isOk != 1) {
            error(80);
            return;
        }
        tokens.p_update_type(&tokens, index + 1, 11);

        index = index + 2;
        int ending_index;

        Token left_curly_brace = tokens.pGet(&tokens, index);
        if (is_single_left_curly_brace(left_curly_brace.value) == 1) {
            tokens.p_update_type(&tokens, index, 9);
            if (token.type != 20 && token.type != 21) {
                error(106);
                return;
            }

            int right_curly_brace_index = parse_vector_matrix_initialization(index + 1, &token); // should give expected type
            if (right_curly_brace_index == -1) {
                error(81);
                return;
            }

            Token right_curly_brace = tokens.pGet(&tokens, right_curly_brace_index);
            if (is_single_character(right_curly_brace.value) != 1) {
                error(82);
                return;
            }
            if (is_right_curly_brace(right_curly_brace.value[0]) != 1 || right_curly_brace.isOk != 1) {
                error(83);
                return;
            }
            tokens.p_update_type(&tokens, right_curly_brace_index, 10);
            ending_index = right_curly_brace_index + 1;

        } else {
            int expr_end_index = get_expression(index, 18);
            if (expr_end_index == -1) {
                error(124);
                return;
            }
            Token result = expression_type(&tokens, index, expr_end_index); //checks type manually

            if (token.type == 19) {
                if (!(result.type == 19 || result.type == 25 || result.type == 26 || result.type == 27)) {
                    error(180);
                    return;
                }
            } else if (token.type == 20) {
                if (!(result.type == 20)) {
                    if (!(result.type == 21 || result.type == 28)) {
                        error(181);
                        return;
                    }
                    if (result.matrix_j != 1) {
                        error(182);
                        return;
                    }
                } else {
                    if (result.vector != token.vector) {
                        error(183);
                        return;
                    }
                }
            } else if (token.type == 21) {
                if (!(result.type == 21 || result.type == 28)) {
                    if (result.type == 20) {
                        if (result.vector != token.matrix_i || token.matrix_j != 1) {
                            error(186);
                            return;
                        }
                    } else {
                        error(184); 
                        return;
                    }
                } else {
                    if (result.matrix_i != token.matrix_i || result.matrix_j != token.matrix_j) {
                        error(185);
                        return;
                    }
                }

            } else if (token.type == 25 || token.type == 26) {
                if (!(result.type == 19 || result.type == 25 || result.type == 26 || result.type == 27)) {
                    error(187);
                    return;
                }
            }
            ending_index = expr_end_index + 1;
        }
        if (is_ok_ending(ending_index) != 1) {
            error(126);
        }
    } else if (strcmp("print", token.value) == 0) {
        tokens.p_update_type(&tokens, 0, 23);

        Token left_paranthesis = tokens.pGet(&tokens, 1);
        if (is_single_character(left_paranthesis.value) != 1) {
            error(87);
            return;
        }
        if (is_left_paranthesis(left_paranthesis.value[0]) != 1 || left_paranthesis.isOk != 1) {
            error(88);
            return;
        }
        tokens.p_update_type(&tokens, 1, 5);

        Token variable = tokens.pGet(&tokens, 2);
        if (is_variable(variable.value) != 1) {
            error(89);
            return;
        } 
        assign_type(&variable, 2);
        variable =  tokens.pGet(&tokens, 2);
        int variable_index = 2;
        int right_paranthesis_index = 3;
        Token left_brace_check = tokens.pGet(&tokens, 3);
        if (is_single_left_brace(left_brace_check.value) == 1) { //0: print 1:( 2:id 3:[ 4:<expr> expr_end:] expr_end+1:)
            if (variable.type == 20) {
                Token left_brace = tokens.pGet(&tokens, 3);
                if (is_single_character(left_brace.value) != 1) {
                    error(107);
                    return;
                }
                if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
                    error(108);
                    return;
                }
                tokens.p_update_type(&tokens, 3, 7);
                left_brace.type = 7;
                
                int expr_end_index = get_expression(4, 8); // RIGHT BRACE DELIMITER
                if (expr_end_index == -1) {
                    error(109);
                    return;
                }
                if (expression(&tokens, 4, expr_end_index, 19) != 1 && expression(&tokens, 4, expr_end_index, 20) != 1 && expression(&tokens, 4, expr_end_index, 21) != 1 && expression(&tokens, 4, expr_end_index, 25) != 1 && expression(&tokens, 4, expr_end_index, 26) != 1 && expression(&tokens, 4, expr_end_index, 27) != 1 && expression(&tokens, 4, expr_end_index, 28) != 1) {
                    error(110);
                    return;
                }   
                
                Token right_brace = tokens.pGet(&tokens, expr_end_index + 1);
                if (is_single_character(right_brace.value) != 1) {
                    error(111);
                    return;
                }
                if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
                    error(112);
                    return;
                }
                tokens.p_update_type(&tokens, expr_end_index + 1, 8);
                right_brace.type = 8;
                right_paranthesis_index = expr_end_index + 2;
                tokens.p_update_type(&tokens, variable_index, 26);
            } else if (variable.type == 21) { //print(id[expr1, expr2])
                Token left_brace = tokens.pGet(&tokens, 3);
                if (is_single_character(left_brace.value) != 1) {
                    error(113);
                    return;
                }
                if (is_left_brace(left_brace.value[0]) != 1 || left_brace.isOk != 1) {
                    error(114);
                    return;
                }
                tokens.p_update_type(&tokens, 3, 7);
                left_brace.type = 7;

                int expr_end_index_1 = get_expression(4, 15); 
                if (expr_end_index_1 == -1) {
                    error(115);
                    return;
                }
                if (expression(&tokens, 4, expr_end_index_1, 19) != 1 && expression(&tokens, 4, expr_end_index_1, 20) != 1 && expression(&tokens, 4, expr_end_index_1, 21) != 1 && expression(&tokens, 4, expr_end_index_1, 25) != 1 && expression(&tokens, 4, expr_end_index_1, 26) != 1 && expression(&tokens, 4, expr_end_index_1, 27) != 1 && expression(&tokens, 4, expr_end_index_1, 28) != 1) {
                    error(116);
                    return;
                }
            
                Token comma = tokens.pGet(&tokens, expr_end_index_1 + 1);
                if (is_single_character(comma.value) != 1) {
                    error(117);
                    return;
                }
                if (is_comma(comma.value[0]) != 1 || comma.isOk != 1) {
                    error(118);
                    return;
                }
                tokens.p_update_type(&tokens, expr_end_index_1 + 1, 15);                       //CURRENTLY SENDING RIGHT AND LEFT SQUARE BRACES TO EXPRESSION
                comma.type = 15;

                int expr_end_index_2 = get_expression(expr_end_index_1 + 2, 8); //                                                              COMMA COMMA COMMA DELIMITER
                if (expr_end_index_2 == -1) {
                    error(119);
                    return;
                }
                if (expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 19) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 20) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 21) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 25) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 26) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 27) != 1 && expression(&tokens, expr_end_index_1 + 2, expr_end_index_2, 28) != 1 ) {
                    error(120);
                    return;
                } 
                //right brace on id = 20 not on id = 21 /// NEED TO CHECK THIS A.S.A.P.!!!!!!
                Token right_brace = tokens.pGet(&tokens, expr_end_index_2 + 1);
                if (is_single_character(right_brace.value) != 1) {
                    error(121);
                    return;
                }
                if (is_right_brace(right_brace.value[0]) != 1 || right_brace.isOk != 1) {
                    error(122);
                    return;
                }
                tokens.p_update_type(&tokens, expr_end_index_2 + 1, 8);
                right_brace.type = 8;
                right_paranthesis_index = expr_end_index_2 + 2;
                tokens.p_update_type(&tokens, variable_index, 25);
            } else {
                error(123);
                return;

            }
        }
        Token right_paranthesis = tokens.pGet(&tokens, right_paranthesis_index);
        if (is_single_character(right_paranthesis.value) != 1) {
            error(90);
            return;
        }
        if (is_right_paranthesis(right_paranthesis.value[0]) != 1 || right_paranthesis.isOk != 1) {
            error(91);
            return;
        }
        tokens.p_update_type(&tokens, right_paranthesis_index, 6);

        if (is_ok_ending(right_paranthesis_index + 1) != 1) {
            error(92);
            return;
        }
    } else if (strcmp("printsep", token.value) == 0) {
        tokens.p_update_type(&tokens, 0, 23);

        Token left_paranthesis = tokens.pGet(&tokens, 1);
        if (is_single_character(left_paranthesis.value) != 1) {
            error(93);
            return;
        }
        if (is_left_paranthesis(left_paranthesis.value[0]) != 1 || left_paranthesis.isOk != 1) {
            error(94);
            return;
        }
        tokens.p_update_type(&tokens, 1, 5);
      
        Token right_paranthesis = tokens.pGet(&tokens, 2);
        if (is_single_character(right_paranthesis.value) != 1) {
            error(95);
            return;
        }
        if (is_right_paranthesis(right_paranthesis.value[0]) != 1 || right_paranthesis.isOk != 1) {
            error(96);
            return;
        }
        tokens.p_update_type(&tokens, 2, 6);

        if (is_ok_ending(3) != 1) {
            error(97);
            return;
        }
    } else if (is_comment(token.value[0]) == 1) {
        if (token.isOk != 1) {
            error(98);
            return;
        }
        tokens.p_update_type(&tokens, 0, 18);

        if (is_ok_ending(1) != 1) {
            error(99);
            return;
        }

    } else {
        error(100);
    }
}

/*
    Helper function for the tokenizer() function. extracts the token and puts it
    inside a struct Token.
    @param char* token_start
    @param int token_len 
    @param Vector* tokens
*/
void extract_token(char *token_start, int token_len, int token_type) {
    Token token;
    token.isOk = 1;
    memset(token.value, '\0', sizeof(token.value));
    strncpy(token.value, token_start, token_len);
    token.type = token_type;
    tokens.pAdd(&tokens, token);
}

/*
    Initializes the attributes and adds scalar to Vector variables.
    @param char [] name - name of the variable
*/
void initialize_for_loop_variable(char name[]) {
    Token for_loop_variable;
    for_loop_variable.isOk = 1;
    memset(for_loop_variable.value, '\0', sizeof(for_loop_variable.value));
    strcpy(for_loop_variable.value, name);
    for_loop_variable.type = 19;
    variables.pAdd(&for_variables, for_loop_variable);

}

/*
    Initializes the attributes and adds scalar to Vector variables.
    @param char[] name - name of the variable.
*/
void initialize_scalar(char name[]) {
    Token scalar;
    scalar.isOk = 1;
    memset(scalar.value, '\0', sizeof(scalar.value));
    strcpy(scalar.value, name);
    scalar.type = 19;
    variables.pAdd(&variables, scalar);

}

/*
    Initializes the attributes and adds vector to Vector variables.
    @param char[] name - name of the variable.
    @param int vector_size - lenght of the vector
*/
void initialize_vector(char name[], int vector_size) {
    Token vector_token;
    vector_token.isOk = 1;
    memset(vector_token.value, '\0', sizeof(vector_token.value));
    strcpy(vector_token.value, name);
    vector_token.vector = vector_size;
    vector_token.type = 20;
    variables.pAdd(&variables, vector_token);

}

/*
    Initializes the attributes and adds matrix to Vector variables.
    @param char[] name - name of the variable.
    @param int matrix_i - i size of the matrix
    @param int matrix_j - j size of the matrix
*/
void initialize_matrix(char name[], int matrix_i, int matrix_j) {
    Token matrix;
    matrix.isOk = 1;
    memset(matrix.value, '\0', sizeof(matrix.value));
    strcpy(matrix.value, name);
    matrix.matrix_i = matrix_i;
    matrix.matrix_j = matrix_j;
    matrix.type = 21;
    variables.pAdd(&variables, matrix);
}

int is_for_loop_variable(char name[]) {
    for (int i = 0; i < for_variables.pSize(&for_variables); i++) {
        Token variable = for_variables.pGet(&for_variables, i);
        if (strcmp(name, variable.value) == 0) {
            return 1;
        }
    }
    return 0;
}

void remove_for_variables() {
    for (int i = 0; i < variables.pSize(&variables); i++) {
        Token variable = variables.pGet(&variables, i);
        if (is_for_loop_variable(variable.value) == 1) {
            variables.p_update_isok(&variables, i, 0);
        }
    }
}

/*
    Returns the index of the right curly brace which closes the initialization.
*/
int parse_vector_matrix_initialization(int start_index, Token *expected_type) {
    Token token;
    int to_return;
    int index = 0;
    int tokens_size = tokens.pSize(&tokens);
    while (start_index + index < tokens_size) {
        token = tokens.pGet(&tokens, start_index + index);
        if (is_single_right_curly_brace(token.value) == 1) {
            to_return = start_index + index;
            tokens.p_update_type(&tokens, start_index + index, 10);
            break;
        }
        if (is_number_literal(token.value) != 1 && is_float(token.value) != 1) {
            return -1;
        }
        tokens.p_update_type(&tokens, start_index + index, 19);
        index++;
    }
    
    int array_lenght;
    if (expected_type->type == 20) {
        array_lenght = expected_type->vector;
    }  else if (expected_type->type == 21) {
        array_lenght = expected_type->matrix_i * expected_type->matrix_j;
    }

    if (start_index + index == tokens_size) {
        return -1;
    } else if (array_lenght != index) {
        return -1;
    } else {
        return to_return;
    }
}

/*
    Checks if there is something left in the remaining part of the vector. 
    @returns 
    0 if there is something left -which is not a comment-
    1 if there is nothing left
    @param Vector* tokens
    @param int start_index  
*/
int is_ok_ending(int start_index) { //works
    Token token = tokens.pGet(&tokens, start_index);
    int vector_size = tokens.pSize(&tokens);
    if (vector_size == start_index) {
        return 1;
    } else {
        if (is_comment(token.value[0]) != 1 || token.isOk != 1) {
            return 0;
        } else {
            return 1;
        }
    }
}

/*
    Checks if a given alphanumeric string is a variable name.
    @param char[] name - value )as in the value field inside the token) of the suspected variable.
    @returns int - 
    1 if there is a variable with name name[]
    0 if there isn't a variable with name name[]
*/
int is_variable(char name[]) {
    for (int i = 0; i < variables.pSize(&variables); i++) {
        Token variable = variables.pGet(&variables, i);
        if (strcmp(name, variable.value) == 0 && variable.isOk == 1) {
            return 1;
        }
    }
    return 0;
}

/*
    Gets the fully attributed token for the given variable name.
    @param char[] name - value (as in the value field inside the token) of the variable.
    @returns Token token - struct Token of the variable
*/
Token get_variable(char name[]) { 
    Token variable;
    for (int i = 0; i < variables.pSize(&variables); i++) {
        variable = variables.pGet(&variables, i);
        if (strcmp(name, variable.value) == 0) {    
            break;
        }
    }
    return variable;
}

/*
    Returns the type of that variable. Type values are assigned in the lookup table
    inside the token.h file
*/
void assign_type(Token *variable, int index) {
    Token state_variable;
    for (int i = 0; i < variables.pSize(&variables); i++) {
        state_variable = variables.pGet(&variables, i);
        if (strcmp(variable->value, state_variable.value) == 0) {    
            break;
        }
    }
    
    if (state_variable.type == 19) {
        tokens.p_update_type(&tokens, index, 19);

    } else  if (state_variable.type == 20) {
        tokens.p_update_type(&tokens, index, 20);
        tokens.p_update_vector(&tokens, index, state_variable.vector);

    } else if (state_variable.type == 21) {
        tokens.p_update_type(&tokens, index, 21);
        tokens.p_update_matrix(&tokens, index, state_variable.matrix_i, state_variable.matrix_j);

    }
}

void tokens_status() {
    printf("============ LINE: %d ============\n", line_number);
    for (int i = 0; i < tokens.pSize(&tokens); i++) {
        Token token = tokens.pGet(&tokens, i);
        printf("Value: \"%s\", Type: %d\n", token.value, token.type);
    }
    printf("============ END ============\n");
}

void postfix_status(Vector *postfix_vector) {
    printf("\t\t============ POSTFIX LINE: %d ============\n", line_number);
    for (int ij = 0; ij < postfix_vector->pSize(postfix_vector); ij++) {
        Token token = postfix_vector->pGet(postfix_vector, ij);
        printf("\t\tValue: \"%s\", Type: %d\n", token.value, token.type);
    }
    printf("\t\t============ POSTFIX END ============\n");
}


int main(int argc, char *argv[]) {
    file = fopen(argv[1], "r");
    out = fopen("file.c","w");
    in_for_loop = 0;
    num_error = 0;
    extra_curly = 0;
    char line[256];
    CreateVector(&variables);
    CreateVector(&for_variables);

    line_number = 1;
    while(fgets(line,256,file) != NULL ) {

        tokenizer(line);
        parser(); 
        output_generator(out);
        line_number++;

        if (num_error > 0) { //breaks when there is an error
            fclose(file);
            remove("file.c");
            break;                      // uncomment this part if you want the program to stop running after the first error
        }
    }
    fprintf(out, "\n}");
    fclose(file);
}  

void output_generator(FILE *out) {
    int flag_is_declaration = 0;
    int flag_is_comment = 0;
    int flag_is_assignment_vector = 0;
    int flag_is_assignment_matrix = 0;
    int vector_index = 0;   // used to store the index of the elent of the vector which is being assigned
    int matrix_index_i = 0; // used to track the index of the matrix inside an assignment statement
    int current_matrix_i = 0;
    int current_matrix_j = 0;
    int matrix_index_j = 0; // used to track the index of the matrix inside an assignment statement
    int flag_opened_par = 0;
    int flag_is_for = 0;
    int operation_flag = 0;
    int paran_count = 0;
    int flag_i_am_bored = 0;
    int flag_curly_closed = 0;
    char current_variable[256];
    // print the required include header statements
    if (flag == 0){
        fprintf(out, "%s", "#include <stdio.h>\n#include \"matlangc_functions.h\"\nint main(){\n");
        flag = 1;
    }
    // for each token in the line print a statement in C
    for (int i = 0; i < tokens.pSize(&tokens); i++) {        
        Token current_token;
        current_token = tokens.pGet(&tokens,i);
        // if the line is an assignment of a vector
        if (flag_is_assignment_vector == 1) {
            if (current_token.type == 9 || current_token.type == 10) { // skip the curly braces
                continue;
            }
            fprintf(out, "%s.arr[%d][0] = %s;\n", current_variable, vector_index, current_token.value);
            vector_index++;
            continue;
        }
        // if the line is an assignment of a matrix
        if (flag_is_assignment_matrix == 1) {
            if (current_token.type == 9 || current_token.type == 10) {  // skip the curly braces
                continue;
            }
            fprintf(out, "%s.arr[%d][%d] = %s;\n", current_variable, matrix_index_i, matrix_index_j, current_token.value);
            matrix_index_j++;
            if (matrix_index_j == current_matrix_j) {
                matrix_index_i++;
                matrix_index_j = 0;
            }
            continue;
        }

        if (current_token.type == 0) {          // scalar keyword
            flag_is_declaration = 1;
            fprintf(out, "%s", "float ");
        }
        else if (current_token.type == 1) {     // vector keyword
            flag_is_declaration = 1;
            fprintf(out, "%s", "Matrix ");
        }
        else if (current_token.type == 2) {     // matrix keyword
            flag_is_declaration = 1;
            fprintf(out, "%s", "Matrix ");
        }
        else if (current_token.type == 4) {     // for keyword
            flag_is_for = 1;
            flag_for_loop = 1;
            Token check;
            check = tokens.pGet(&tokens, i+3);
            if (check.type != 15) {
                Token id;
                id = tokens.pGet(&tokens, i+2);
                Token exp1;
                Token exp2;
                Token exp3;
                int expr_end_index = get_expression_output(i+4, 16);
                exp1 = expression_type_2(&tokens, i+4, expr_end_index);
                int expr_end_index_2 = get_expression_output(expr_end_index+2, 16);
                exp2 = expression_type_2(&tokens, expr_end_index+2, expr_end_index_2);
                int expr_end_index_3 = get_expression_output(expr_end_index_2+2, 6);                
                exp3 = expression_type_2(&tokens, expr_end_index_2+2, expr_end_index_3);
                fprintf(out, "for (%s = %s; %s <= %s; %s+=%s) ", id.value, exp1.value, id.value, exp2.value, id.value, exp3.value);
                i = expr_end_index_3 + 1;
            }
            else {
                extra_curly = 1;
                Token id1;
                id1 = tokens.pGet(&tokens, i+2);
                Token id2;
                id2 = tokens.pGet(&tokens, i+4);
                Token exp1;
                Token exp2;
                Token exp3;
                int expr_end_index = get_expression_output(i+4, 16);
                exp1 = expression_type_2(&tokens, i+6, expr_end_index);
                int expr_end_index_2 = get_expression_output(expr_end_index+2, 16);
                exp2 = expression_type_2(&tokens, expr_end_index+2, expr_end_index_2);
                int expr_end_index_3 = get_expression_output(expr_end_index_2+2, 15);
                exp3 = expression_type_2(&tokens, expr_end_index_2+2, expr_end_index_3);
                Token exp4;
                Token exp5;
                Token exp6;
                int expr_end_index_4 = get_expression_output(expr_end_index_3+2, 16);
                exp4 = expression_type_2(&tokens, expr_end_index_3+2, expr_end_index_4);
                int expr_end_index_5 = get_expression_output(expr_end_index_4+2, 16);
                exp5 = expression_type_2(&tokens, expr_end_index_4+2, expr_end_index_5);
                int expr_end_index_6 = get_expression_output(expr_end_index_5+2, 6);
                exp6 = expression_type_2(&tokens, expr_end_index_5+2, expr_end_index_6);
                fprintf(out, "for (%s = %s; %s <= %s; %s+=%s) {\n\t", id1.value, exp1.value, id1.value, exp2.value, id1.value, exp3.value);
                fprintf(out, "for (%s = %s; %s <= %s; %s+=%s) ", id2.value, exp4.value, id2.value, exp5.value, id2.value, exp6.value);
                i = expr_end_index_6+1;
            }
        }
        else if (current_token.type == 5) {     // left paranthesis
            fprintf(out, "%s","(");
        }
        else if (current_token.type == 6) {     // right paranthesis
            fprintf(out, "%s",")");
        }
        else if (current_token.type == 7) {     // left square brace
            continue;
        }
        else if (current_token.type == 8) {     // right square brace
            fprintf(out, "%s",")");
        }
        else if (current_token.type == 9) {     // left curly brace
            fprintf(out, "%s","{");
            flag_curly_closed = 1;
        }
        else if (current_token.type == 10) {     // right curly brace
            if (extra_curly == 1) {
                fprintf(out, "\n}");
                extra_curly = 0;
            }
            fprintf(out, "%s","}");
            flag_curly_closed = 1;
            flag_for_loop = 0;
        }
        else if (current_token.type == 11) {     // equals
            continue;
        }
        else if (current_token.type == 12) {     // multiplication
            continue;
        }
        else if (current_token.type == 13) {     // addition
            continue;
        }
        else if (current_token.type == 14) {     // subtraction
            continue;
        }
        else if (current_token.type == 15) {     // comma
            continue;
        }
        else if (current_token.type == 16) {     // colon
            continue;
        }
        else if (current_token.type == 17) {     // undetermined
            continue;
        }
        else if (current_token.type == 18) {     // comment
            fprintf(out,"// %s", current_token.value);
            flag_is_comment = 1;
        }
        else if (current_token.type == 19) {    // scalar variable
            Token next_token;
            next_token = tokens.pGet(&tokens,i+1);
            fprintf(out, "%s", current_token.value);
            if (flag_is_declaration == 1){
                flag_is_declaration = 0;
            }
            if (next_token.type == 11) {
                fprintf(out," = ");
                int expr_end_index = get_expression_output(i+2, 18);
                Token expression = expression_type_2(&tokens, i+2, expr_end_index);
                fprintf(out, "%s", expression.value);
                break;
            }
        }
        else if (current_token.type == 20) {     // vector variable
            strcpy(current_variable, current_token.value);
            Token next_token;
            next_token = tokens.pGet(&tokens, i+1);
            Token next_token_2;
            next_token_2 = tokens.pGet(&tokens, i+2);
            if (next_token.type == 11 && next_token_2.type == 9) {
                flag_is_assignment_vector = 1;
                i++;
                continue;
            }
            else if (next_token.type == 11) {
                int expr_end_index = get_expression_output(i+2, 18);
                Token expression = expression_type_2(&tokens, i+2, expr_end_index);
                fprintf(out, "assign_matrix(%s,", current_variable);
                fprintf(out, "%s", expression.value);
                fprintf(out,")");
                paran_count++;
                i++;
                break;
            }
            else if (flag_is_declaration == 1){
                flag_is_declaration = 0;
                Token size;
                size = tokens.pGet(&tokens,i+2);
                char *size_of_vector = size.value;
                fprintf(out,"%s%s\ncreate_matrix(&%s,%s,1)", current_variable, ";", current_token.value, size_of_vector);
                i+=3;
            }
            else{
                fprintf(out, "%s", current_token.value);
            }
        }
        else if (current_token.type == 21) {     // matrix variable
            strcpy(current_variable, current_token.value);
            Token next_token;
            next_token = tokens.pGet(&tokens, i+1);
            Token next_token_2;
            next_token_2 = tokens.pGet(&tokens, i+2);
            if (next_token.type == 11 && next_token_2.type == 9) {
                flag_is_assignment_matrix = 1;
                i++;
                current_matrix_i = current_token.matrix_i;
                current_matrix_j = current_token.matrix_j;
                continue;
            }
            else if (next_token.type == 11) {
                int expr_end_index = get_expression_output(i+2, 18);
                Token expression = expression_type_2(&tokens, i+2, expr_end_index);
                fprintf(out, "assign_matrix(%s,", current_variable);
                fprintf(out, "%s", expression.value);
                fprintf(out,")");
                paran_count++;
                i++;
                break;
            }

            else if (flag_is_declaration == 1){
                flag_is_declaration = 0;
                Token size_i;
                Token size_j;
                size_i = tokens.pGet(&tokens,i+2);
                size_j = tokens.pGet(&tokens,i+4);
                char *size_of_i = size_i.value;
                char *size_of_j = size_j.value;
                fprintf(out,"%s%s\ncreate_matrix(&%s,%s,%s)", current_variable, ";", current_token.value, size_of_i,size_of_j);
                i+=5;
            }
            else {
                fprintf(out,"%s", current_token.value);
            }
        }
        else if (current_token.type == 23) {     // print function
            if (strcmp(current_token.value, "print") == 0){
                Token var = tokens.pGet(&tokens,i+2);
                if (var.type == 19){
                    fprintf(out, "print_integer(%s", var.value);
                }
                else if (var.type == 20 || var.type == 21) {
                    fprintf(out, "print_matrix(&%s", var.value);

                }
                else if (var.type == 25) {
                    int expr_end_index = get_expression_output(i+2, 6);
                    Token expression_1 = expression_type_2(&tokens, i+2, expr_end_index);
                    fprintf(out, "print_integer(%s", expression_1.value);
                    fprintf(out, ")");
                    break;
                }
                else if (var.type == 26) {
                    int expr_end_index = get_expression_output(i+2, 6);
                    Token expression = expression_type_2(&tokens, i+2, expr_end_index);
                    fprintf(out, "print_integer(%s", expression.value);
                    fprintf(out, ")");                    
                    break;
                }
                i+=2;
            }
            else if (strcmp(current_token.value, "printsep") == 0){
                fprintf(out,"%s","printf(\"%s\\n\",\"----------\")");
                i+=2;
            }
        }
        else if (current_token.type == 25) {     // indexed matrix
            fprintf(out, "assign_matrix_index(%s," , current_token.value);
            int expr_end_index = get_expression_output(i+2, 15);
            Token expression_1 = expression_type_2(&tokens, i+2, expr_end_index);
            int expr_end_index_1 = get_expression_output(expr_end_index+2, 8);
            Token expression_2 = expression_type_2(&tokens, expr_end_index+2, expr_end_index_1);
            fprintf(out, "%s,%s,", expression_1.value, expression_2.value);
            int expr_end_index_after_equals = get_expression_output(expr_end_index_1+3, 18);
            Token experssion_toassign = expression_type_2(&tokens, expr_end_index_1+3, expr_end_index_after_equals);
            fprintf(out, "%s)", experssion_toassign.value);
            break;

        }
        else if (current_token.type == 26) {     // indexed vector
            fprintf(out, "assign_vector_index(%s," , current_token.value);
            int expr_end_index = get_expression_output(i+2, 8);
            Token expression_1 = expression_type_2(&tokens, i+2, expr_end_index);
            fprintf(out, "%s,", expression_1.value);
            int expr_end_index_after_equals = get_expression_output(expr_end_index+3, 18);
            Token experssion_toassign = expression_type_2(&tokens, expr_end_index+3, expr_end_index_after_equals);
            fprintf(out, "%s)", experssion_toassign.value);
            break;
        }
        else if (current_token.type == 27) {     
            continue;
        }
        else if (current_token.type == 28) {     
            continue;
        }
        
    }

    if (flag_is_comment == 0 && flag_is_assignment_matrix == 0 && flag_is_assignment_vector == 0){
        if (flag_curly_closed == 1) {
            fprintf(out,"%s", "\n");
        }
        else {
            fprintf(out,"%s", ";\n");
        }
    }
    if (flag_for_loop == 1) {
        fprintf(out, "\t");

    }
}


// NEEDS AN EXPRESSION FUNCTION WHICH WILL GET THE VECTOR AND THE DESIRED TYPE THAN RETURN 1 OR 0; IF 1 EXPRESSION IS VALID IF 0 EXPRESSION IS INVALID. 
// OUTPUT BOOLEAN WILL BE AFFECTED FROM BOTH SYNTAX AND EXPECTED OUTPUT
int expression(Vector *tokens, int start_index, int end_index, int expected_type) {
    Token expr;
    
    expr = infix_to_postfix(tokens, start_index, end_index);
    if (expr.isOk == 0){
        return 0;
    } else {
        if (expr.type == expected_type) {
            return 1;
        } else {
            return 0;
        }
    }
}

Token expression_type(Vector *tokens, int start_index, int end_index) {
    Token expr;
    expr = infix_to_postfix(tokens, start_index, end_index);
    return expr;
}

/*
returns the pseudo-token needed for the expression function
converts the infix expression to postfix
*/
Token infix_to_postfix(Vector *subtokens, int start, int end) {
    Vector postfix_vector;
    CreateVector(&postfix_vector);
    Stack postfix_stack;
    CreateStack(&postfix_stack);
    Token return_token;
    int i = start;
    int flag = 1;
    int leftbrace_num = 0;
    int inside_function = 0;
    int function_paranthesis = 0;

    while(i <= end) {
        Token next_token = subtokens->pGet(subtokens, i);
        if (next_token.type == 7) {
            leftbrace_num++;
            i++;
            continue;
        }
        if (next_token.type == 8) {
            leftbrace_num--;
            i++;
            continue;
        }
        if (leftbrace_num > 0) {
            i++;
            continue;
        }
        
        if (inside_function == 1) {
            if (next_token.type == 5) {
                function_paranthesis++;
                i++;
                continue;
            }
            if (next_token.type == 6) {
                if (function_paranthesis == 1) {
                    function_paranthesis--;
                    inside_function = 0;
                    i++;
                    continue;
                } else {
                    function_paranthesis--;
                    i++;
                    continue;
                }
            }
            i++;
            continue;
        }
        if (next_token.type == 3 || next_token.type == 27 || next_token.type == 28) {
            inside_function = 1;

        }

        if (next_token.type == 19 || next_token.type == 20 || next_token.type == 21 || next_token.type == 25 || next_token.type == 26 || next_token.type == 27 || next_token.type == 28 ) { // if the next token is an operand
            postfix_vector.pAdd(&postfix_vector, next_token);
        } else if (next_token.type == 5) { // if the next token is a left paranthesis
            postfix_stack.pPush(&postfix_stack,next_token);
        } else if (next_token.type == 6) { // if the next token is a right paranthesis
            while (postfix_stack.pPeek(&postfix_stack).type != 5) { // while we don't encounter a right paranthesis
                if (postfix_stack.pIsEmpty(&postfix_stack)) {
                    flag = 0;
                    // error(101);
                    break;
                } 
                Token tokenPoped;
                tokenPoped = postfix_stack.pPop(&postfix_stack);
                postfix_vector.pAdd(&postfix_vector,tokenPoped);
            }
            if (postfix_stack.pPeek(&postfix_stack).type == 5) {
                postfix_stack.pPop(&postfix_stack);
            }
        } else if (next_token.type == 12 || next_token.type == 13 || next_token.type == 14) { // if the next token is an operator
            if (postfix_stack.pIsEmpty(&postfix_stack) == 1) { // if the stack is empty push the operator into the stack
                postfix_stack.pPush(&postfix_stack,next_token);
            } else if (postfix_stack.pPeek(&postfix_stack).type == 5) { // if the top element is a left paranthesis push the operator into the stack
                postfix_stack.pPush(&postfix_stack,next_token);
            } else if (has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type) == 1) { // if the operator has higher precedence than the top of the stack
                postfix_stack.pPush(&postfix_stack,next_token);
            } else if (has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type) == 0) { // if the operator has lower precedence than the top of the stack
                while(has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type) == 0) {
                    Token tokenPoped;
                    tokenPoped = postfix_stack.pPop(&postfix_stack);
                    postfix_vector.pAdd(&postfix_vector,tokenPoped);
                    if (postfix_stack.pIsEmpty(&postfix_stack)){
                        break;
                    }
                }
                postfix_stack.pPush(&postfix_stack,next_token);
            }
            // if they have the same precedence assume left associativity
            else if (has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type) == 2) {
                Token tokenPoped;
                tokenPoped = postfix_stack.pPop(&postfix_stack);
                postfix_vector.pAdd(&postfix_vector,tokenPoped);
                postfix_stack.pPush(&postfix_stack, next_token);
            }
        } 
        i++;
    }
    
    if (postfix_stack.pIsEmpty(&postfix_stack) == 0) {      // if the postfix_stack is not empty
        while (postfix_stack.pIsEmpty(&postfix_stack) != 1) {
            if (postfix_stack.pPeek(&postfix_stack).type != 5) {        // if the top of the stack is not a left paranthesis
                postfix_vector.pAdd(&postfix_vector,postfix_stack.pPop(&postfix_stack));    // add the top of the stack to the vector
            } else {
                flag = 0;                                               // if there is a left paranthesis in the stack there is an error --> flag
                // error(102);
                break;
            }
        }
    }
    if (flag != 0 && leftbrace_num == 0){                               // if no error spotted
        return_token = evaluate_postfix(&postfix_vector);               // evaluate the postfix expression
        postfix_vector.p_free(&postfix_vector);                         // free the allocated memory
        postfix_stack.p_free(&postfix_stack);                           
        return return_token;
    }
    else{
        return_token.isOk = 0;                                          // if there is an error
        postfix_vector.p_free(&postfix_vector);                         // return to null token (i.e. token.isOK = 0)
        postfix_stack.p_free(&postfix_stack);
        return return_token;
    }
}

/*
    evaluates the postfix expression and checks for syntax errors
    returns the pseudo-token needed by the expression function
*/
Token evaluate_postfix(Vector *postfix) {
    int i = 0;
    int flag = 0;
    Stack evaluation_stack;
    CreateStack(&evaluation_stack);
    Token last_result;


    while(i < postfix->pSize(postfix)){
        Token next_token = postfix->pGet(postfix, i);
        if (next_token.type == 19 || next_token.type == 20 || next_token.type == 21 || next_token.type == 25 || next_token.type == 26 || next_token.type == 27 || next_token.type == 28 ) { // if the next token is an operand
            evaluation_stack.pPush(&evaluation_stack,next_token);
        } else if (next_token.type == 12 || next_token.type == 13 || next_token.type == 14) { // if the next token is an operator
            if (evaluation_stack.pSize(&evaluation_stack) >= 2) {
                Token op2 = evaluation_stack.pPop(&evaluation_stack);
                Token op1 = evaluation_stack.pPop(&evaluation_stack);
                Token result;
                result = type_check(op1,op2,next_token);
                if (result.isOk == 0) {
                    // error(103);
                    flag = 1;
                    break;
                } else {
                    evaluation_stack.pPush(&evaluation_stack,result);
                }
            } else {
                // error(104);
                flag = 1; 
                break;
            }  
        }
        else{
            last_result.isOk = 0;
            evaluation_stack.p_free(&evaluation_stack);
            return last_result;
        }
        i++;
    }

    if (evaluation_stack.pSize(&evaluation_stack) != 1) {
        // error(105);
        last_result.isOk = 0;
        evaluation_stack.p_free(&evaluation_stack);
        return last_result;
        flag = 1;
    } else if (flag == 0) {
        last_result = evaluation_stack.pPop(&evaluation_stack);
        evaluation_stack.p_free(&evaluation_stack);
        return last_result;
    }
    last_result.isOk = 0;
    evaluation_stack.p_free(&evaluation_stack);
    return last_result;
}

/*
checks if an operation is possible
compares the types according to syntax rules
retuns the resulting token
*/
Token type_check(Token op1, Token op2, Token operator) {
   Token result_token;

   if (operator.type == 12) { // if the operator is *
       if (op1.type == 19 || op1.type == 25 || op1.type == 26 || op1.type == 27) { // if op1 is scalar
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                // the resulting token is a scalar
                result_token.type = 19;
                result_token.isOk = 1;
                return result_token;
            } else if (op2.type == 20) { // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op2.vector;
                return result_token;
            } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op2.matrix_i;
                result_token.matrix_j = op2.matrix_j; 
                return result_token;
            }
        } else if (op1.type == 20) { // if op1 is vector
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op1.vector;
                return result_token;
           } else if (op2.type == 20) { // if op2 is vector
                if (op2.vector == 1) {
                    result_token.type = 20;
                    result_token.isOk = 1;
                    result_token.vector = op1.vector;
                    return result_token;
                } else {
                    result_token.isOk = 0;
                    return result_token;
                }
            } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                if(op2.matrix_i == 1) {
                    result_token.type = 21;
                    result_token.isOk = 1;
                    result_token.matrix_i = op1.vector;
                    result_token.matrix_j = op2.matrix_j;
                    return result_token;
                } else {
                    result_token.isOk = 0;
                    return result_token;
                }
            }
        } else if (op1.type == 21 || op1.type == 28) { // if op1 is a matrix
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op1.matrix_i;
                result_token.matrix_j = op1.matrix_j;
                return result_token;
            } else if (op2.type == 20){ // if op2 is vector
                if (op1.matrix_j == op2.vector) {
                    result_token.type = 20;
                    result_token.isOk = 1;
                    result_token.vector  = op1.matrix_i;
                    return result_token;
                } else {
                    result_token.isOk = 0;
                    return result_token;
                }
            } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                if (op1.matrix_j == op2.matrix_i) {
                    result_token.type = 21;
                    result_token.isOk = 1;
                    result_token.matrix_i = op1.matrix_i;
                    result_token.matrix_j = op2.matrix_j;
                    return result_token;
                }
                else {
                    result_token.isOk = 0;
                    return result_token;
                }
            }
        }
   } if (operator.type == 13 || operator.type == 14) { // if the operator is + or - 
       if (op1.type == 19 || op1.type == 25 || op1.type == 26 || op1.type == 27) { // if op1 is scalar
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.type = 19;
                result_token.isOk = 1;
                return result_token;
           } else if (op2.type == 20) { // if op2 is vector
                result_token.isOk = 0;
                return result_token;
           } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                result_token.isOk = 0;
                return result_token;
           }
       } else if (op1.type == 20) {  // if op1 is vector
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.isOk = 0;
                return result_token;
            } else if (op2.type == 20) { // if op2 is vector
                if (op1.vector == op2.vector) {
                    result_token.type = 20;
                    result_token.isOk = 1;
                    result_token.vector = op1.vector;
                    return result_token;
                } else {
                    result_token.isOk = 0;
                    return result_token;
                }
           } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                if (op1.vector == op2.matrix_i && op2.matrix_j == 1) {
                    result_token.type = 20;
                    result_token.isOk  = 1;
                    result_token.vector = op1.vector;
                    return result_token;
                } else {
                    result_token.isOk = 0;
                    return result_token;
                }
           }
        } else if (op1.type == 21 || op1.type == 28) { // if op1 is a matrix
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.isOk = 0;
                return result_token;
            } else if (op2.type == 20) { // if op2 is vector
                if (op1.matrix_i == op2.vector && op1.matrix_j == 1) {
                    result_token.type = 20;
                    result_token.isOk = 1;
                    result_token.vector = op2.vector;
                    return result_token;
                } else {
                    result_token.isOk = 0;
                    return result_token;
                }
            } else if (op2.type == 21 || op2.type == 28) { // if op3 is matrix
                if (op1.matrix_i == op2.matrix_i && op1.matrix_j == op2.matrix_j) {
                    result_token.type = 21;
                    result_token.isOk = 1;
                    result_token.matrix_i = op1.matrix_i;
                    result_token.matrix_j = op1.matrix_j;
                    return result_token;
                } else {
                    result_token.isOk = 0;
                    return result_token;
                }
            }
        }
    }

    result_token.isOk = 0;
    return result_token;
}

/*
    checks which operator has higher precedence
    @returns
    @param int incoming
    @param int top
*/
int has_higher_precedence(int incoming, int top) {
    // if the given operator is *
    if (incoming == 12) {
        if (top == 13 || top == 14){
            return 1;
        }
        else if (top == 12) {
            return 2;
        }
    } else if (incoming == 13 || incoming == 14) {
        if (top == 12) {
            return 0;
        }
        else if (top == 13 || top == 14) {
            return 2;
        }
    }

    return 3;
}


//=======================================================================================================================================================
// NEEDS AN EXPRESSION FUNCTION WHICH WILL GET THE VECTOR AND THE DESIRED TYPE THAN RETURN 1 OR 0; IF 1 EXPRESSION IS VALID IF 0 EXPRESSION IS INVALID. 
// OUTPUT BOOLEAN WILL BE AFFECTED FROM BOTH SYNTAX AND EXPECTED OUTPUT
int expression_2(Vector *tokens, int start_index, int end_index, int expected_type) {
    Token expr;
    
    expr = infix_to_postfix_2(tokens, start_index, end_index);
    if (expr.isOk == 0){
        return 0;
    } else {
        if (expr.type == expected_type) {
            return 1;
        } else {
            return 0;
        }
    }
}

Token expression_type_2(Vector *tokens, int start_index, int end_index) {
    Token expr;
    expr = infix_to_postfix_2(tokens, start_index, end_index);
    return expr;
}

/*
returns the pseudo-token needed for the expression function
converts the infix expression to postfix
*/
Token infix_to_postfix_2(Vector *subtokens, int start, int end) {
    Vector postfix_vector;
    CreateVector(&postfix_vector);
    Stack postfix_stack;
    CreateStack(&postfix_stack);
    Token return_token;
    int i = start;
    int flag = 1;
    int leftbrace_num = 0;
    int inside_function = 0;
    int function_paranthesis = 0;

    while(i <= end) {
        Token next_token = subtokens->pGet(subtokens, i);
        if (next_token.type == 7) {
            leftbrace_num++;
            i++;
            continue;
        }
        if (next_token.type == 8) {
            leftbrace_num--;
            i++;
            continue;
        }
        if (leftbrace_num > 0) {
            i++;
            continue;
        }
        
        if (inside_function == 1) {
            if (next_token.type == 5) {
                function_paranthesis++;
                i++;
                continue;
            }
            if (next_token.type == 6) {
                if (function_paranthesis == 1) {
                    function_paranthesis--;
                    inside_function = 0;
                    i++;
                    continue;
                } else {
                    function_paranthesis--;
                    i++;
                    continue;
                }
            }
            i++;
            continue;
        }
        if (next_token.type == 3 || next_token.type == 27 || next_token.type == 28) {
            inside_function = 1;
        }
        if (next_token.type == 19 || next_token.type == 20 || next_token.type == 21 || next_token.type == 25 || next_token.type == 26 || next_token.type == 27 || next_token.type == 28 ) { // if the next token is an operand
            postfix_vector.pAdd(&postfix_vector, next_token);
        } else if (next_token.type == 5) { // if the next token is a left paranthesis
            postfix_stack.pPush(&postfix_stack,next_token);
        } else if (next_token.type == 6) { // if the next token is a right paranthesis
            while (postfix_stack.pPeek(&postfix_stack).type != 5) { // while we don't encounter a right paranthesis
                if (postfix_stack.pIsEmpty(&postfix_stack)) {
                    flag = 0;
                    break;
                }
                Token tokenPoped;
                tokenPoped = postfix_stack.pPop(&postfix_stack);
                postfix_vector.pAdd(&postfix_vector,tokenPoped);
            }
            if (postfix_stack.pPeek(&postfix_stack).type == 5) {
                postfix_stack.pPop(&postfix_stack);
            }
        } else if (next_token.type == 12 || next_token.type == 13 || next_token.type == 14) { // if the next token is an operator
            if (postfix_stack.pIsEmpty(&postfix_stack) == 1) { // if the stack is empty push the operator into the stack
                postfix_stack.pPush(&postfix_stack,next_token);
            } else if (postfix_stack.pPeek(&postfix_stack).type == 5) { // if the top element is a left paranthesis push the operator into the stack
                postfix_stack.pPush(&postfix_stack,next_token);
            } else if (has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type) == 1) { // if the operator has higher precedence than the top of the stack
                postfix_stack.pPush(&postfix_stack,next_token);
            } else if (has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type) == 0) { // if the operator has lower precedence than the top of the stack
                while(has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type == 0)) {
                    Token tokenPoped;
                    tokenPoped = postfix_stack.pPop(&postfix_stack);
                    postfix_vector.pAdd(&postfix_vector,tokenPoped);
                    if (postfix_stack.pIsEmpty(&postfix_stack)){
                        break;
                    }
                }
                postfix_stack.pPush(&postfix_stack,next_token);
            }
            // if they have the same precedence assume left associativity
            else if (has_higher_precedence(next_token.type,postfix_stack.pPeek(&postfix_stack).type) == 2) {
                Token tokenPoped;
                tokenPoped = postfix_stack.pPop(&postfix_stack);
                postfix_vector.pAdd(&postfix_vector,tokenPoped);
                postfix_stack.pPush(&postfix_stack, next_token);
            }
        } 
        i++;
    }
    
    if (postfix_stack.pIsEmpty(&postfix_stack) == 0) {      // if the postfix_stack is not empty
        while (postfix_stack.pIsEmpty(&postfix_stack) != 1) {
            if (postfix_stack.pPeek(&postfix_stack).type != 5) {        // if the top of the stack is not a left paranthesis
                postfix_vector.pAdd(&postfix_vector,postfix_stack.pPop(&postfix_stack));    // add the top of the stack to the vector
            } else {
                flag = 0;                                               // if there is a left paranthesis in the stack there is an error --> flag
                break;
            }
        }
    }
    if (flag != 0 && leftbrace_num == 0){                               // if no error spotted
        return_token = evaluate_postfix_2(&postfix_vector);               // evaluate the postfix expression
        postfix_vector.p_free(&postfix_vector);                         // free the allocated memory
        postfix_stack.p_free(&postfix_stack);                           
        return return_token;
    }
    else{
        return_token.isOk = 0;                                          // if there is an error
        postfix_vector.p_free(&postfix_vector);                         // return to null token (i.e. token.isOK = 0)
        postfix_stack.p_free(&postfix_stack);
        return return_token;
    }
}

/*
    evaluates the postfix expression and checks for syntax errors
    returns the pseudo-token needed by the expression function
*/
Token evaluate_postfix_2(Vector *postfix) {
    int i = 0;
    int flag = 0;
    Stack evaluation_stack;
    CreateStack(&evaluation_stack);
    Token last_result;
    

    while(i < postfix->pSize(postfix)){
        Token next_token = postfix->pGet(postfix, i);
        if (next_token.type == 19 || next_token.type == 20 || next_token.type == 21 || next_token.type == 25 || next_token.type == 26 || next_token.type == 27 || next_token.type == 28 ) { // if the next token is an operand
            evaluation_stack.pPush(&evaluation_stack,next_token);
        } else if (next_token.type == 12 || next_token.type == 13 || next_token.type == 14) { // if the next token is an operator
            if (evaluation_stack.pSize(&evaluation_stack) >= 2) {
                Token op2 = evaluation_stack.pPop(&evaluation_stack);
                Token op1 = evaluation_stack.pPop(&evaluation_stack);
                Token result;
                result = type_check_2(op1,op2,next_token);
                if (result.isOk == 0) {
                    flag = 1;
                    break;
                } else {
                    evaluation_stack.pPush(&evaluation_stack,result);
                }
            } else {
                flag = 1; 
                break;
            }  
        }
        else{
            last_result.isOk = 0;
            evaluation_stack.p_free(&evaluation_stack);
            return last_result;
        }
        i++;
    }
    if (evaluation_stack.pSize(&evaluation_stack) != 1) {
        last_result.isOk = 0;
        evaluation_stack.p_free(&evaluation_stack);
        return last_result;
        flag = 1;
    } else if (flag == 0) {
        last_result = evaluation_stack.pPop(&evaluation_stack);
        evaluation_stack.p_free(&evaluation_stack);
        return last_result;
    }
    last_result.isOk = 0;
    evaluation_stack.p_free(&evaluation_stack);
    return last_result;
}

/*
checks if an operation is possible
compares the types according to syntax rules
retuns the resulting token
*/
Token type_check_2(Token op1, Token op2, Token operator) {
   Token result_token;
   char result_token_val[300] = "";

   if (operator.type == 12) { // if the operator is *
       if (op1.type == 19 || op1.type == 25 || op1.type == 26 || op1.type == 27) { // if op1 is scalar
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                // the resulting token is a scalar
                result_token.type = 19;
                result_token.isOk = 1;
                strcat(result_token_val, "(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, operator.value);
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            } else if (op2.type == 20) { // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op2.vector;
                strcat(result_token_val, "multiply_scalar(");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op2.matrix_i;
                result_token.matrix_j = op2.matrix_j;
                strcat(result_token_val, "multiply_scalar(");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val); 
                return result_token;
            }
        } else if (op1.type == 20) { // if op1 is vector
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.type = 20;
                result_token.isOk = 1;
                strcat(result_token_val, "multiply_scalar(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                result_token.vector = op1.vector;
                return result_token;
           } else if (op2.type == 20) { // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op1.vector;
                strcat(result_token_val, "multiply(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op1.vector;
                result_token.matrix_j = op2.matrix_j;
                strcat(result_token_val, "multiply(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            }
        } else if (op1.type == 21 || op1.type == 28) { // if op1 is a matrix
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op1.matrix_i;
                result_token.matrix_j = op1.matrix_j;
                strcat(result_token_val, "multiply_scalar(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            } else if (op2.type == 20){ // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector  = op1.matrix_i;
                strcat(result_token_val, "multiply(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op1.matrix_i;
                result_token.matrix_j = op2.matrix_j;
                strcat(result_token_val, "multiply(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            }
        }
   } if (operator.type == 13) { // if the operator is + 
       if (op1.type == 19 || op1.type == 25 || op1.type == 26 || op1.type == 27) { // if op1 is scalar
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.type = 19;
                result_token.isOk = 1;
                strcat(result_token_val, "(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, operator.value);
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
           }
       } else if (op1.type == 20) {  // if op1 is vector
            if (op2.type == 20) { // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op1.vector;
                strcat(result_token_val, "add(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
           } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                result_token.type = 20;
                result_token.isOk  = 1;
                result_token.vector = op1.vector;
                strcat(result_token_val, "add(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
           }
        } else if (op1.type == 21 || op1.type == 28) { // if op1 is a matrix
            if (op2.type == 20) { // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op2.vector;
                strcat(result_token_val, "add(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            } else if (op2.type == 21 || op2.type == 28) { // if op3 is matrix
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op1.matrix_i;
                result_token.matrix_j = op1.matrix_j;
                strcat(result_token_val, "add(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            }
        }
    } if (operator.type == 14) { // if the operator is - 
       if (op1.type == 19 || op1.type == 25 || op1.type == 26 || op1.type == 27) { // if op1 is scalar
            if (op2.type == 19 || op2.type == 25 || op2.type == 26 || op2.type == 27) { // if op2 is scalar
                result_token.type = 19;
                result_token.isOk = 1;
                strcat(result_token_val, "(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, operator.value);
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
           }
       } else if (op1.type == 20) {  // if op1 is vector
            if (op2.type == 20) { // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op1.vector;
                strcat(result_token_val, "subtraction(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
           } else if (op2.type == 21 || op2.type == 28) { // if op2 is matrix
                result_token.type = 20;
                result_token.isOk  = 1;
                result_token.vector = op1.vector;
                strcat(result_token_val, "subtraction(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
           }
        } else if (op1.type == 21 || op1.type == 28) { // if op1 is a matrix
            if (op2.type == 20) { // if op2 is vector
                result_token.type = 20;
                result_token.isOk = 1;
                result_token.vector = op2.vector;
                strcat(result_token_val, "subtraction(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            } else if (op2.type == 21 || op2.type == 28) { // if op3 is matrix
                result_token.type = 21;
                result_token.isOk = 1;
                result_token.matrix_i = op1.matrix_i;
                result_token.matrix_j = op1.matrix_j;
                strcat(result_token_val, "subtraction(");
                strcat(result_token_val, op1.value);
                strcat(result_token_val, ",");
                strcat(result_token_val, op2.value);
                strcat(result_token_val, ")");
                strcpy(result_token.value, result_token_val);
                return result_token;
            }
        }
    }

    result_token.isOk = 0;
    return result_token;
}

/*
    checks which operator has higher precedence
    @returns
    @param int incoming
    @param int top
*/
int has_higher_precedence_2(int incoming, int top) {
    // if the given operator is *
    if (incoming == 12) {
        if (top == 13 || top == 14){
            return 1;
        }
        else if (top == 12) {
            return 2;
        }
    } else if (incoming == 13 || incoming == 14) {
        if (top == 12) {
            return 0;
        }
        else if (top == 13 || top == 14) {
            return 2;
        }
    }

    return 3;
}
