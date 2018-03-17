/**
 * @file parser.h
 * @author Sayed Sadeed
 * @brief File containing all definitions for parser module.
 */

#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"

/**
 * @brief Can token be consumed based on expected type.
 * 
 * This function is used to determine if a token matches expected type.
 * 
 * @param tok is a pointer to the token being compared.
 * @param type the type we are expecting it to be. 
 */
int can_consume(Token *tok, char *type);

/**
 * @brief parse an expression.
 * 
 * Function is responsible for parsing mathematical expressions.
 * Such as 3 + 4 or 12 * 4
 * 
 * @param tok_mgr the TokenMgr which holds the tokens for the compilation.
 * @return the output of the calculation or -1.
 */
int parse_expr (TokenMgr *tok_mgr);

#endif