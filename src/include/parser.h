/**
 * @file parser.h
 * @author Sayed Sadeed
 * @brief File containing all definitions for parser module.
 */

#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "node.h"

/**
 * @brief Store all the errors related to parsing process.
 */
typedef struct {
    char **errors;
    size_t error_ctr;
    int error_cap;
} PErrors;


/**
 * @brief Create new malloced PErrors instance.
 * 
 * @return New instance of PErrors or Null if failed.
 */
PErrors *parser_new_perrors(void);

/**
 * Instruct error handler to release all of its stored errors.
 * 
 * @param err_handle PErrors instance.
 */
void parser_free_perrors(PErrors *err_handle, int print_mode);

/**
 * @brief Add a error string to the list of errors stored in PErrors.
 * 
 * @param p_error Instance of PErrors.
 * @param offender Token which triggered the error.
 * @param err_type Index of error string in the array of errors in parser.c.
 */
void parser_add_perror(PErrors *err_handle, Token *offender, int err_type);

/**
 * @brief Can token be consumed based on expected type.
 * 
 * This function is used to determine if a token matches expected type.
 * 
 * @param tok is a pointer to the token being compared.
 * @param type the type we are expecting it to be. 
 */
int parser_can_consume(char *tok_type, char *type);

/**
 * @brief Will consume a group based on grammar defintion.
 * 
 * Group = string | string_list
 * 
 * @param tok_mgr TokenMgr instance.
 * @param err_handle Error handler to capture any parsing errors.
 * @return Node generated from production.
 */
struct Node *parse_group(TokenMgr *tok_mgr, PErrors *err_handle);


/**
 * @brief Will consume assignment based on grammar.
 * 
 * identifier = INTEGER | STRING 
 * 
 * @param tok_mgr TokenMgr instance.
 * @param err_handle Error handler to capture any parsing errors.
 * @return Node generated from production.
 */
struct Node *parse_assignment(TokenMgr *tok_mgr, PErrors *err_handle);


/**
 * @brief Initial entry for parser.
 * 
 * 
 * @param tok_mgr TokenMgr containing all token information.
 * @return 0 if parser was successful or return 1 if issue occurred.
 */
int parser_init(TokenMgr *tok_mgr);

#endif