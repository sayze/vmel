/**
 * @file tokens.h
 * @author Sayed Sadeed
 * @brief File containing token definitions and enum types.
 */

#ifndef TOKENS_H
#define TOKENS_H

// Comment.
#define COMMENT '#'
// Newline.
#define NEWLINE '\n'
// Double quotes.
#define DQUOTE '"'
// Equals operator.
#define EQUAL '='
// Left brace.
#define LBRACE '{'
// Right brace.
#define RBRACE '}'
// Var identifier.
#define VAR '$'
// Plus symbol.
#define PLUS '+'
// Minus symbol.
#define MINUS '-'
// Asterisk symbol.
#define ASTERISK '*'
// Forward slash/divide.
#define FSLASH '/'
// Left paranthesis.
#define LPAREN '('
// Right paranthesis.
#define RPAREN ')'
// Less than.
#define LESSTHAN '<'
// Greater than.
#define GREATERTHAN '>'
// Exclamation.
#define BANG '!'

// Possible token types.
enum TokTypes {
    INTEGER, 
    STRING, 
    OPERATOR, 
    KEYWORD, 
    GROUP, 
    IDENTIFIER,
    LPARENS,
    RPARENS,
};

#endif
