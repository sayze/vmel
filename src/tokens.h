/**
 * @file tokens.h
 * @author Sayed Sadeed
 * @brief File containing token definitions.
 */

#ifndef TOKENS_H
#define TOKENS_H

/**
 * @brief Represent a single token read from input.
 *
 * Struct will hold every identified token meta data. Is needed for parsing.
 */
typedef struct{
	char type[20]; //TODO: safe to assume types will only ever be less than 20 ???
	char value[100]; //TODO: needs optimizing BAD. It is possible to have STRING more than 100 chars
} Token;

/**
 * @brief Token manager represents a pool of tokens.
 *
 * This provides a high level interfacing for token management. It is preferred to use this
 * for anything token related as it manages internal memory allocs and deallocs.
 * Struct will mantain all tokens and responsible for methods.
 */
typedef struct{
	Token **curr_tok;
	Token *toks[1000]; // TODO: needs optimizing BAD. Use realloc here
	size_t tok_ctr;
} TokenMgr;

/**
 * @brief Build tokens from buffer in.
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
 * @param tok_mgr Pointer to token manager.
 * @param tok_type Type of token.
 * @param tok_value Value of token.
 * @return int signifying status.
 */
int TokenMgr_add_token(TokenMgr *tok_mgr, char tok_type[50], char tok_val[100]);

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
 * Function will return the next token stored inside Token Mgr.
 * Note that it returns based on the current token. Once the last 
 * token is reached the internal state must be reset. Otherwise the value
 * returned will continue to be null. See TokenMgr_reset_token().
 * 
 * @param tok_mgr Pointer to token manager instance.
 * @return next Token in manager.
 */
Token *TokenMgr_next_token(TokenMgr *tok_mgr);

/**
 * @brief Retrieve the previous token from Tokens.
 * 
 * Function will return the previous token stored inside Token Mgr.
 * Note that it returns based on the current token. Once the first 
 * token is reached the internal state must be reset. Otherwise the value
 * returned will continue to be null. See TokenMgr_reset_token().
 *
 * @param tok_mgr Pointer to token manager instance.
 * @return previous Token in manager.
 */
Token *TokenMgr_prev_token(TokenMgr *tok_mgr);

/**
 * @brief Reset the internal token to NULL.
 *
 * This function operates on the internal token incrementer
 * of the token manager. It will nullify the internal pointer so that
 * collection can be incremented through again.
 * See TokenMgr_next_token() and TokenMgr_prev_token().
 *
 * @param tok_mgr Pointer to token manager instance.
 */
void TokenMgr_reset_token(TokenMgr *tok_mgr);

/**
 * @brief Get last token in token manager.
 * 
 * Unlike the "TokenMgr" prefixed functions this doesn't operate
 * on the internal pointer incrementer. This simply returns a pointer
 * to the last token in the collection.
 *
 * @param tok_mgr Pointer to token manager instance.
 * @return Token pointer to final token.
 */
Token *get_last_token(TokenMgr *tok_mgr);

/**
 * @brief Get first token in token manager.

 * Unlike the "TokenMgr" prefixed functions this doesn't operate
 * on the internal pointer incrementer. This simply returns a pointer
 * to the first token in the collection.

 * @param tok_mgr Pointer to token manager instance.
 * @return Token pointer to first token.
 */
Token *get_first_token(TokenMgr *tok_mgr);

#endif
