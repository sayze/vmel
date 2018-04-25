/**
 * @file tokens.h
 * @author Sayed Sadeed
 * @brief File containing token definitions and keywords.
 */

#ifndef TOKENS_H
#define TOKENS_H

// Below are language literalls.
#define COMMENT '#'
#define NEWLINE '\n'
#define DQUOTE '"'
#define EQUAL '='
#define LBRACE '{'
#define RBRACE '}'
#define VAR '$'
#define PLUS '+'
#define MINUS '-'
#define ASTERISK '*'
#define FSLASH '/'
#define LPAREN '('
#define RPAREN ')'
#define LESSTHAN '<'
#define GREATERTHAN '>'
#define COMMA ','
#define LBRACKET '['
#define RBRACKET ']'
#define BANG '!'
#define DOT '.'
#define BTICK '`'

#define KWORDS_SIZE 5

/**
 * @brief Determine if a string is a valid keyword.
 * 
 * This function checks against Keywords inside tokens.c
 * to determine if it is a valid keyword or not.
 * 
 * @param str the string to check for.
 * @return 1 if it is valid keyword otherwise return 0.
 */
int is_valid_keyword(char *str);

#endif
