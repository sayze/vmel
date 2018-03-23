/**
 * @file tokens.h
 * @author Sayed Sadeed
 * @brief File containing token management.
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#define KWORDS_SIZE 4
#define TOKENTYPE_SIZE 20
#define TOKMGR_TOKS_INIT_SIZE 50

#include <string.h>
#include "tokens.h"

/**
 * @brief Represent a single token read from input.
 *
 * Struct will hold every identified token meta data. Is needed for parsing.
 */
typedef struct {
	char type[TOKENTYPE_SIZE];
	char *value;
	size_t val_length;
	int lineno;
} Token;

/**
 * @brief Token manager represents a pool of tokens.
 *
 * This provides a high level interfacing for token management. It is preferred to use this
 * for anything token related as it manages internal memory allocs and deallocs.
 * Struct will mantain all tokens and responsible for methods.
 */
typedef struct {
	Token *toks_head;
	Token **toks_curr;
	Token *toks_tail;
	size_t tok_ctr;
	size_t tok_cap;
} TokenMgr;

/**
 * @brief Allocate more resources for TokenMgr *toks array.
 */
Token **TokenMgr_realloc_toks();

/**
 * @brief Build tokens from steam of input.
 * 
 * Provides a decoupled implementation for building tokens from
 * any source stream. Can be contents of file or stdin.
 * 
 * @param buff the contents which should be tokenized.
 * @param tokmgr Token Manager to handle tokenization.
 * @return int signifying status.
 */
int build_tokens(char *buff, TokenMgr *tokmgr);

/**
 * @brief Create token manager malloc'ed.
 * 
 * This function acts as a constructor for the Token manager.
 * 
 * @return newly created TokenMgr pointer.
 */
TokenMgr *TokenMgr_new();

/**
 * @brief Add another token to token manager.
 * 
 * Important to note that the tok_val is expected to be null terminated string ptr.
 * 
 * @param tok_mgr Pointer to token manager.
 * @param tok_type Type of token.
 * @param tok_value Value of token.
 * @param tok_lineno Line number in source file where token occurs.
 * @return int signifying status.
 */
int TokenMgr_add_token(TokenMgr *tok_mgr, char tok_type[20], char *tok_val, int tok_lineno);

/**
 * @brief Free tokens stored by token manager as well as token manager.
 * 
 * @param tok_mgr Pointer to token manager.
 * @return int signifying status.
 */
int TokenMgr_free(TokenMgr *tok_mgr);

/**
 * @brief Print contents of token manager. Useful for debugging.
 * 
 * @param tok_mgr Pointer to token manager.
 */
void TokenMgr_print_tokens(TokenMgr *tok_mgr);

/**
 * @brief Retrieve the next token from Tokens.
 * 
 * Function will return the next token stored inside TokenMgr.
 * Note that it returns based on the current token. Once the last 
 * token is reached the internal state must be reset. Otherwise the value
 * returned will continue to be null. See TokenMgr_reset_token().
 * 
 * @param tok_mgr Pointer to token manager instance.
 * @return next Token in manager.
 */
Token *TokenMgr_next_token(TokenMgr *tok_mgr);

/**
 * @brief Check to see if current token in manager is the last.
 * 
 * This function provides high level interface for determining if the
 * current token stored inside the manager is the last one.
 * 
 * @param tok_mgr Pointer to token manager instance.
 * @return 0 if not last token otherwise return 1.
 */
int TokenMgr_is_last_token(TokenMgr *tok_mgr);

/**
 * @brief Retrieve the previous token from Tokens.
 * 
 * Function will return the previous token stored inside TokenMgr.
 * Note that it returns based on the current token. Once the first 
 * token is reached the internal state must be reset. Otherwise the value
 * returned will continue to be null. See TokenMgr_reset_token().
 *
 * @param tok_mgr Pointer to token manager instance.
 * @return previous Token in manager.
 */
Token *TokenMgr_prev_token(TokenMgr *tok_mgr);

/**
 * @brief Reset the internal token back to start of array.
 *
 * This function operates on the internal curr token
 * of the token manager. It will position the tokens pointer pointer back to the HEAD.
 * See TokenMgr_next_token() and TokenMgr_prev_token().
 *
 * @param tok_mgr Pointer to token manager instance.
 */
void TokenMgr_reset_curr(TokenMgr *tok_mgr);

/**
 * @brief Get the Token currently being pointed to by Token Manager 
 * internal token pointer.
 *
 * This function will give access to the token being stored by TokenMgr **curr_token.
 * It will save having to dereference the pointer each time. Also it will encapsulate
 * the internal structure of the TokenMgr struct.
 *
 * @param tok_mgr Pointer to token manager instance.
 * @return Token pointer currently pointed to by **tok_curr.
 */
Token *TokenMgr_current_token(TokenMgr *tok_mgr);

/**
 * @brief Determine if a string is a valid keyword.
 * 
 * This function checks against R_Keywords inside tokens.h
 * to determine if it is a valid keyword or not.
 * 
 * @param str the string to check for.
 * @return 1 if it is valid keyword otherwise return 0.
 */
int is_valid_keyword(char *str);

#endif
