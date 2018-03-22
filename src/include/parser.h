/**
 * @file parser.h
 * @author Sayed Sadeed
 * @brief File containing all definitions for parser module.
 */

#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"

/**
 * Global access to the current token within parsing module.
 */
static Token *tok_curr_ptr;

/**
 * Helper pointer to help "peek" token.
 * TODO: implement peek function in token manager.
 */
static Token *tok_peek_ptr;

/**
 * Global access to passed token manager.
 */
static TokenMgr *tok_mgr_ptr;

/**
 * @brief Can token be consumed based on expected type.
 * 
 * This function is used to determine if a token matches expected type.
 * 
 * @param tok is a pointer to the token being compared.
 * @param type the type we are expecting it to be. 
 */
int can_consume(char *tok_type, char *type);

void parser_init(TokenMgr *tok_mgr);

#endif