#include "string_functions.h"
#include <string.h>


/*
    Checks if ch is an alphanumeric a character
    @returns 
    1 if ch is an alphanumeric character
    0 if ch is not an alphanumeric character
    @param ch character to check
*/
int is_alphanumeric(char ch) {
    if (is_letter(ch) == 1 || is_number(ch) == 1) {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if a string is comprised of only alphanumeric characters
    @returns 
    1 if the string ch[] has only alphanumeric characters
    0 if the string ch[] does not have only alphanumeric characters
    @param ch character to check
*/
int is_alphanumeric_literal(char ch[]) {
    for (int i = 0; i < strlen(ch); i++) {
        if (is_alphanumeric(ch[i]) == 0) {
            return 0;
        }
    }
    return 1;
}


/*
    Checks if ch is a letter
    @returns 
    1 if ch is a letter
    0 if ch is not a letter
    @param ch character to check

*/
int is_letter(char ch) {
    if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z')) {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if a string is comprised of only letters
    @returns 
    1 if the string ch[] has only letters
    0 if the string ch[] does not have only letters
    @param ch character to check
*/
int is_letter_literal(char ch[]) {
    for (int i = 0; i < strlen(ch); i++) {
        if (is_letter(ch[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

/*
    Checks if ch is a number
    @returns 
    1 if ch is a number
    0 if ch is not a number
    @param ch character to check
*/
int is_number(char ch) {
    if ('0' <= ch && ch <= '9') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if a string is comprised of only numbers
    @returns 
    1 if the string ch[] has only numbers
    0 if the string ch[] does not have only numbers
    @param ch character to check
*/
int is_number_literal(char ch[]) {
    for (int i = 0; i < strlen(ch); i++) {
        if (is_number(ch[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

/*
    Checks if ch is a paranthesis
    @returns 
    1 if ch is a left paranthesis
    2 if ch is a right paranthesis
    0 if ch is not a paranthesis
    @param ch character to check
*/
int is_paranthesis(char ch) {
    if (ch == '(' || ch == ')') {
        if (ch == '(') {
            return 1;
        } else {
            return 2;
        }
    } else {
        return 0;
    }
}

/*
    Checks if ch is a special character which were predetermined
    @returns 
    1 if ch is a special character
    0 is ch is not a special character
    @param ch character to check
*/
int is_special_character(char ch) {
    if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '=' || ch == '+' || ch == '*' || ch == '-' || ch == ',' || ch == ':') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a white-space
    @returns
    1 if ch is a white-space
    0 if ch is not a white-space
    @param ch character to check
*/
int is_whitespace(char ch) {
    if (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\n') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a dot
    @returns
    1 if ch is a dot
    0 if ch is not a dot
    @param ch character to check
*/
int is_dot(char ch) {
    if (ch == '.') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a charater for comments, in matlang this is #
    @returns
    1 if ch is a character for comments
    0 if ch is not a character for comments
    @param ch character to check
*/
int is_comment(char ch) {
    if (ch == '#') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a left brace
    @returns
    1 if ch is a left brace
    0 if ch is not a left brace
    @param ch character to check
*/
int is_left_brace(char ch) {
    if (ch == '[') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a right brace
    @returns
    1 if ch is a right brace
    0 if ch is not a right brace
    @param ch character to check
*/
int is_right_brace(char ch) {
    if (ch == ']') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a left paranthesis
    @returns
    1 if ch is a left paranthesis
    0 if ch is not a left paranthesis
    @param ch character to check
*/
int is_left_paranthesis(char ch) {
    if (ch == '(') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a right paranthesis
    @returns
    1 if ch is a right paranthesis
    0 if ch is not a right paranthesis
    @param ch character to check
*/
int is_right_paranthesis(char ch) {
    if (ch == ')') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a left curly brace
    @returns
    1 if ch is a left curly brace
    0 if ch is not a left curly brace
    @param ch character to check
*/
int is_left_curly_brace(char ch) {
    if (ch == '{') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a right curly brace
    @returns
    1 if ch is a right curly brace
    0 if ch is not a right curly brace
    @param ch character to check
*/
int is_right_curly_brace(char ch) {
    if (ch == '}') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a star character
    @returns
    1 if ch is a star character
    0 if ch is not a star character
    @param ch character to check
*/
int is_star(char ch) {
    if (ch == '*') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a minus character
    @returns
    1 if ch is a minus character
    0 if ch is not a minus character
    @param ch character to check
*/
int is_minus(char ch) {
    if (ch == '-') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a plus character
    @returns
    1 if ch is a plus character
    0 if ch is not a plus character
    @param ch character to check
*/
int is_plus(char ch) {
    if (ch == '+') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is a colon
    @returns
    1 if ch is a colon
    0 if ch is not a colon
    @param ch character to check
*/
int is_colon(char ch) {
    if (ch == ':') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is an equals character
    @returns
    1 if ch is an equals character
    0 if ch is not an equals character
    @param ch character to check
*/
int is_equals(char ch) {
    if (ch == '=') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if ch is an comma
    @returns
    1 if ch is an comma
    0 if ch is not an comma
    @param ch character to check
*/
int is_comma(char ch) {
    if (ch == ',') {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if the char array is a floating point number.
    @returns 
    1 if ch is a floating point number 
    0 if ch is not a floating point number
    @param char[] ch character array to check
*/
int is_float(char ch[]) {
    int dot = 0;
    for (int i = 0; i < strlen(ch); i++) {
        if (is_dot(ch[i]) == 1) {
            dot += 1;
            continue;
        }
        if (is_number(ch[i]) != 1) {
            return 0;
        }
    }
    if (dot == 1) {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if the char array is a function keyword for function that are not print functions.
    @returns 
    1 if ch is a function keyword
    0 if ch is not a function keyword
    @param char[] ch character array to check
*/
int is_function_keyword(char ch[]) {
    if (strcmp(ch, "choose") == 0 || strcmp(ch, "tr") == 0 || strcmp(ch, "sqrt") == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*
    Checks if a character array is made up of a single character, this function is used for
    special characters. 
    @returns 
    1 if ch[] is a single character
    0 if ch[] is not a single character
    @param char[] ch character array to check 
*/
int is_single_character(char ch[]) {
    if (strlen(ch) == 1) {
        return 1;
    } else {
        return 0;
    }
}


/*
    Checks if a character array is made up of a single right curly brace character, this function is used for
    matrix and vector initialization. 
    @returns 
    1 if ch[] is a single right curly brace character
    0 if ch[] is not a single right curly brace character
    @param char[] ch character array to check 
*/
int is_single_right_curly_brace(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_right_curly_brace(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single left curly brace character, this function is used for
    matrix and vector initialization. 
    @returns 
    1 if ch[] is a single left curly brace character
    0 if ch[] is not a single left curly brace character
    @param char[] ch character array to check 
*/
int is_single_left_curly_brace(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_left_curly_brace(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single left paranthesis character, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single left paranthesis character
    0 if ch[] is not a single left paranthesis character
    @param char[] ch character array to check 
*/
int is_single_left_paranthesis(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_left_paranthesis(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single right paranthesis character, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single right paranthesis character
    0 if ch[] is not a single right paranthesis character
    @param char[] ch character array to check 
*/
int is_single_right_paranthesis(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_right_paranthesis(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single plus, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single plus
    0 if ch[] is not a single plus
    @param char[] ch character array to check 
*/
int is_single_plus(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_plus(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single minus, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single minus
    0 if ch[] is not a single minus
    @param char[] ch character array to check 
*/
int is_single_minus(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_minus(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single star, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single star
    0 if ch[] is not a single star
    @param char[] ch character array to check 
*/
int is_single_star(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_star(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single colon, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single colon
    0 if ch[] is not a single colon
    @param char[] ch character array to check 
*/
int is_single_colon(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_colon(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single right brace, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single right brace
    0 if ch[] is not a single right brace
    @param char[] ch character array to check 
*/
int is_single_right_brace(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_right_brace(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single left brace, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single left brace
    0 if ch[] is not a single left brace
    @param char[] ch character array to check 
*/
int is_single_left_brace(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_left_brace(ch[0]) != 1) {
        return 0;
    }
    return 1;
}

/*
    Checks if a character array is made up of a single comma, this function is used in get_expression(). 
    @returns 
    1 if ch[] is a single comma
    0 if ch[] is not a single comma
    @param char[] ch character array to check 
*/
int is_single_comma(char ch[]) {
    if (is_single_character(ch) != 1) {
        return 0;
    }
    if (is_comma(ch[0]) != 1) {
        return 0;
    }
    return 1;
}