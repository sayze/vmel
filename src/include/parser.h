/**
 * @file parser.h
 * @author Sayed Sadeed
 * @brief File containing all definitions for parser module.
 */

#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "node.h"
#include "sytable.h"
#include "errors.h"

/**
 * @brief Maintain state within in the parsing process.
 * 
 * Instead of passing around excess paramters we simply pass a single
 * structure which holds all the necessary information per parse.
 */
typedef struct {
	Node *curr_expr;
	unsigned int expr_depth;
	Token *curr_token;
	SyTable *sy_table;
	TokenMgr *tok_mgr;
	Error *err_handle;
} ParserMgr;

/**
 * @brief Create new malloced ParserMgr instance.
 * 
 * @return new instance of ParserMgr or null if failed. 
 */
ParserMgr *ParserMgr_new();

/**
 * @brief Free ParserMgr instance.
 * 
 * @param Instance of ParserMgr.
 * @return 0 if free was successful. Otherwise 1 for errors.
 */
int ParserMgr_free(ParserMgr *par_mgr);

/**
 * @brief Add a error string to the list of errors stored in Error.
 * 
 * @param err_error Instance of Error.
 * @param offender Token which triggered the error.
 * @param err_type Index of error string in the array of errors in parser.c.
 */
void ParserMgr_add_error(Error *err_handle, Token *offender, int err_type);

/**
 * @brief Will consume a group based on grammar defintion.
 * 
 * Group = string | string_list
 * 
 * @param par_mgr ParserMgr instance.
 * @param err_handle Error handler to capture any parsing errors.
 * @return Node generated from production.
 */
Node *parse_group(ParserMgr *par_mgr);

/**
 * @brief Will consume assignment based on grammar.
 * 
 * assignment = expression | STRING 
 * 
 * @param par_mgr ParserMgr instance.
 * @return Node generated from production.
 */
Node *parse_assignment(ParserMgr *par_mgr);

/**
 * @brief Will consume expression based on grammar.
 * 
 * expression = factor | factor (PLUS | MINUS) factor
 * 
 * @param par_mgr ParserMgr instance.
 * @return Node generated from production.
 */
Node *parse_expr(ParserMgr *par_mgr);

/**
 * @brief Will consume factor based on grammar.
 * 
 * factor = INTEGER
 * 
 * @param par_mgr ParserMgr instance.
 * @return Node generated from production.
 */
Node *parse_factor(ParserMgr *par_mgr);

/**
 * @brief Initial entry for parser.
 * 
 * Function will perform parsing and construct an AST as output.
 * 
 * @param tok_mgr TokenMgr containing all token information.
 * @param sy_table SyTable instance to store resulting symbol information.
 * @return NodeMgr instance containing generated AST or NULL if error occurred.
 */
NodeMgr *parser_init(TokenMgr *tok_mgr, SyTable *sy_table);

/**
 * @brief Iterate through tokens until specific token type is encountered.
 * 
 * This function will skip tokens until the next token specified by type.
 * Useful for error recovery. 
 * 
 * @param par_mgr ParserMgr instance.
 * @param type The token type to skip to.
 */
void ParserMgr_skip_to(ParserMgr *par_mgr, char *type);

#endif
