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

#define KWORDS_SIZE 7

/**
 * brief Token type in conjunction to the derived types.
 */
typedef enum {
	E_EOF_TOKEN,
	E_KEYWORD_TOKEN,
	E_IDENTIFIER_TOKEN,
	E_STRING_TOKEN,
	E_INTEGER_TOKEN,
	E_MIXSTR_TOKEN,
  	E_EEQUAL_TOKEN,
	E_EQUAL_TOKEN,
	E_LBRACE_TOKEN,
	E_RBRACE_TOKEN,
	E_VAR_TOKEN,
	E_PLUS_TOKEN,
	E_MINUS_TOKEN,
	E_ASTERISK_TOKEN,
	E_FSLASH_TOKEN,
	E_LPAREN_TOKEN,
	E_RPAREN_TOKEN,
	E_LESSTHAN_TOKEN,
	E_LESSTHANEQ_TOKEN,
	E_GREATERTHAN_TOKEN,
	E_GREATERTHANEQ_TOKEN,
	E_BETWEEN_TOKEN,
	E_NOT_TOKEN,
	E_NEQUAL_TOKEN,
	E_COMMA_TOKEN,
	E_STMT_TOKEN,
	E_LBRACKET_TOKEN,
	E_RBRACKET_TOKEN
} TokenType;

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
