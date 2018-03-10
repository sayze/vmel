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
	char type[20]; //TODO safe to assume types will only ever be less than 20 ?
	char value[100]; //TODO needs optimizing BAD. It is possible to have STRING more than 100 chars
} Token;

/**
 * @brief Token manager represents a pool of tokens.
 *
 * This provides a high level interfacing for token management. It is preferred to use this
 * for anything token related as it manages internal memory allocs and deallocs.
 * Struct will mantain all tokens and responsible for methods.
 */
typedef struct{
	Token *toks[1000]; //TODO needs optimizing BAD.
	size_t tok_ctr;
} TokenMgr;

/**
 * @brief Read contents of file to buffer.
 *
 * This function will read the contents of a passed source file (*.vml).
 * It will strip out the inline and external comments from the source and
 * save the contents to a buffer. The buffer is created on heap so must be managed
 * by caller. See below example
 *
 * @code
 * char *filename = "~/Desktop/run.vml";
 * char *buffer = file_to_buffer(filename);
 * free(buffer) // when done.
 * @endcode
 *
 * @param filename Path to source file.
 * @return Pointer to buffer containing contents of file.
 */
char *file_to_buffer(const char *);

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
 * 
 * @return int signifying status.
 */
int TokenMgr_add(TokenMgr *tok_mgr, char tok_type[50], char tok_val[100]);

/**
 * @brief Free tokens stored by token manager as well as token manager.
 * 
 * @param tok_mgr Pointer to token manager.
 * 
 * @return int signifying status.
 */
int TokenMgr_free(TokenMgr *tok_mgr);

/**
 * @brief Print contents of token manager. Useful for debugging.
 * 
 * @param tok_mgr Pointer to token manager.
 * 
 */
void TokenMgr_print_tokens(TokenMgr *tok_mgr);

/**
 * @brief Utility function to determine if char is one of accepted identifiers.
 * 
 * @param id The char to get validated.
 * 
 * @return 1 If it matches accepted identifiers or return 0 if it doesn't.
 */	
int is_valid_identifier(char id);

#endif
