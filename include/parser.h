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
	unsigned int expr_depth;
	Token *curr_token;
	NodeMgr *node_mgr;
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
 * @return Node generated from production.
 */
Node *parse_group(ParserMgr *par_mgr);

/**
 * @brief Will consume a keyword based on grammar defintion.
 * 
 * Keyword = function (args)
 * 
 * @param par_mgr ParserMgr instance.
 * @return Node generated from production.
 */
Node *parse_keyword(ParserMgr *par_mgr);

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
 * @brief Initialise parser with required structs before parsing.
 * 
 * Function will initialise the components of ParserMgr to a ready state.
 * 
 * @param tok_mgr TokenMgr containing all token information.
 * @param sy_table SyTable instance to store resulting symbol information.
 * @param node_mgr NodeMgr instance to store resulting ast.
 * @param err Error instance where all errors will be logged.
 * @return ParserMgr instance.
 */
ParserMgr *ParseMgr_init(TokenMgr *tok_mgr, SyTable *sy_table, NodeMgr *node_mgr, Error *err);

/**
 * @brief Perform parse on stream of tokens.
 * 
 * Function will use internal structs to store information. The internal
 * structs may be filled using the init method see 
 * ParserMgr_init(ParserMgr *par_mgr). Or simply manually assigned.
 * 
 * @param par_mgr instance of ParserMgr.
 * return 
 */
Node *Parser_parse(ParserMgr *par_mgr);

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
