#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"	
#include "parser.h"
#include "tokenizer.h"   
#include "node.h"
#include "sytable.h"
#include "errors.h"

// Below are the errors which map to Error_Templates.
#define ERR_UNEXPECTED 0
#define ERR_GROUP_EXIST 1
#define ERR_UNCLOSED_GROUP 2
#define ERR_EMPTY_STMT 3
#define ERR_ARRAY_EMPTY 4
#define ERR_MISSING_BRACKET 5
#define ERR_MISSING_BRACE 6
#define ERR_MISSING_PAREN 7
#define ERR_NAKED_VARIABLE 8
#define ERR_INVALID_TYPES 9
#define ERR_EMPTY_GROUP 10

// These are the errors a parser may generate. They are mapped to the #DEFINE above.
static const char *Error_Templates[] = {
	"Parsing error : unexpected @0 found in line @1",
	"Parsing Error : Duplicate definition {@0} already defined in line @1",
	"Syntax error : Group {@0} missing closing tag in line @1",
	"Parsing error: Statement '@0' missing arguments in line @1",
	"Parsing error : Expected array item after comma but found NULL near @0 in line @1",
	"Syntax error: Missing closing ']' near '@0' in line @1",
	"Syntax error: Missing closing '}' near '@0' in line @1",
	"Syntax error: Missing closing ')' near '@0' in line @1",
	"Parsing error: '$@0' declaraion must be followed by valid assignment in line @1",
	"Parsing error: Operation on incompatible types near '@0' in line @1",
	"Parsing error: Group {@0} must contain commands, in line @1",
};

// Sync ParserMgr internal token to be current token held by TokenMgr.
static void par_mgr_sync(ParserMgr *par_mgr) {
	par_mgr->curr_token = TokenMgr_current_token(par_mgr->tok_mgr);
}

// Increment the token within TokenMgr and assign it to par_mgr.
static void par_mgr_next(ParserMgr *par_mgr) {
	par_mgr->curr_token = TokenMgr_next_token(par_mgr->tok_mgr);
}

// Check to make sure operation is one of (== != <= >= < >)
static int is_compare_operator(TokenType op) {
	return (op == E_EQUAL_TOKEN
			|| op == E_EEQUAL_TOKEN
			|| op == E_NEQUAL_TOKEN 
			|| op == E_LESSTHAN_TOKEN
			|| op == E_LESSTHANEQ_TOKEN
			|| op == E_GREATERTHAN_TOKEN
			|| op == E_GREATERTHANEQ_TOKEN
			|| op == E_BETWEEN_TOKEN);
}

// Return the type of compare node based on token.
static enum NodeType get_compare_type(char *op) {
	if (string_compare(op, "==")) {
		return E_EEQUAL_NODE;
	}
	else if (string_compare(op, "!=")) {
		return E_NEQUAL_NODE;
	}
	else if (string_compare(op, "<")) {
		return E_LESSTHAN_NODE;
	}
	else if (string_compare(op, "<=")) {
		return E_LESSTHANEQ_NODE;
	}
	else if (string_compare(op, ">")) {
		return E_GREATERTHAN_NODE;
	}
	else if (string_compare(op, ">=")) {
		return E_GREATERTHANEQ_NODE;
	}
	else if (string_compare(op, "><")) {
		return E_BETWEEN_NODE;
	}

	return E_EOF_NODE;
}

// Allocate more memory for array node items.
static Node **grow_arr_nodes(Node *arr_node) {
	if (null_check(arr_node, "grow array nodes")) return NULL;
	Node **new_items = NULL;
	arr_node->data->ArrayNode.dcap = arr_node->data->ArrayNode.dcap * (arr_node->data->ArrayNode.dcap / 2);
	new_items = realloc(arr_node->data->ArrayNode.items, arr_node->data->ArrayNode.dcap * sizeof(Node *));
	return new_items;
}

// Shorthand for malloc'ed array node and items.
static Node *node_new_array() {
	Node *arr = NULL;
	arr = Node_new(1);
	arr->type = E_ARRAY_NODE;
	arr->data->ArrayNode.dctr = 0;
	arr->data->ArrayNode.dcap = 15;
	arr->value = NULL;
	arr->data->ArrayNode.items = malloc(arr->data->ArrayNode.dcap * sizeof(Node *));
	return arr;
}

static int parser_expects(ParserMgr *par_mgr, int err_code, int ct, ...) {
	va_list ap;
	int ret = 1;
	int arg;
	va_start(ap, ct);

	for( int i = 0 ; i < ct; i++ ) {
		arg = va_arg( ap, int);
		if (par_mgr->curr_token->type != arg) {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, err_code);
			par_mgr_next(par_mgr);
			ret = 0;
		}
	}

	va_end(ap);
	return ret;
}

ParserMgr *ParserMgr_new() {
	ParserMgr *ps = malloc(sizeof(ParserMgr));
	ps->curr_token = NULL;
	ps->node_mgr = NULL;
	ps->tok_mgr = NULL;
	ps->err_handle = NULL;
	return ps;
}

int ParserMgr_free(ParserMgr *par_mgr) {
	if (null_check(par_mgr, "parsermgr free")) return -1;
		
	par_mgr->curr_token = NULL;
	par_mgr->err_handle = NULL;
	par_mgr->tok_mgr = NULL;
	free(par_mgr);

	return 0;
}

void ParserMgr_add_error(Error *err_handle, Token *offender, int err_type) {
	// Don't exceed error limit.
	if (err_handle->error_cap == err_handle->error_ctr)
		return;
	 
	char off_lineno[16]; // Assume that a source file won't exceed 9999,9999,9999,9999 lines ?
	char *off_value = offender->value;
	const char *template = Error_Templates[err_type];
	char *template_values[] = {off_value, off_lineno};
	char *template_fmt = NULL;

	// We can't convert line number to string so replace with undefined.	
	if (int_to_string(off_lineno, offender->lineno) < 0)
		strncpy(off_lineno, "undefined", 10);

	template_fmt = string_map_vars(template, template_values, strlen(template), 2);
	
	// Add the final template string to error handler.
	err_handle->errors[err_handle->error_ctr] = template_fmt;
	err_handle->error_ctr++;
}

void ParserMgr_skip_to(ParserMgr *par_mgr, TokenType type) {
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && par_mgr->curr_token->type != type) {
		par_mgr_next(par_mgr);
	}
}

Node *parse_string(ParserMgr *par_mgr) {
	Node *str = NULL;
	if (par_mgr->curr_token->type == E_STRING_TOKEN || par_mgr->curr_token->type == E_MIXSTR_TOKEN) {
		str = Node_new(0);
		str->type = E_STRING_NODE;
		
		// Change type if mix string.
		if (par_mgr->curr_token->type == E_MIXSTR_TOKEN)
			str->type = E_MIXSTR_NODE;
			
		str->value = par_mgr->curr_token->value;
		par_mgr_next(par_mgr);
	}
	return str;
}

Node *parse_factor(ParserMgr *par_mgr) {
	par_mgr_sync(par_mgr);
	Node *res = NULL;
	 if (par_mgr->curr_token->type == E_INTEGER_TOKEN) {
		 res = Node_new(0);
		 res->type = E_INTEGER_NODE;
		 res->value = par_mgr->curr_token->value; 
		 par_mgr_next(par_mgr);
	 }
	 else if (par_mgr->curr_token->type == E_IDENTIFIER_TOKEN) {
		 res = Node_new(0);
		 res->type = E_IDENTIFIER_NODE;
		 res->value = par_mgr->curr_token->value; 
		 par_mgr_next(par_mgr);
	 }
	 else if (par_mgr->curr_token->type == E_LPAREN_TOKEN) {
		 TokenMgr_next_token(par_mgr->tok_mgr);
		 res = parse_expr(par_mgr);
		 par_mgr_sync(par_mgr);
		 
		 // Should have closing paren.
		 if (par_mgr->curr_token->type != E_RPAREN_TOKEN)
			 ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_MISSING_PAREN);
	 }
	 else {
		 res = parse_string(par_mgr);
	 }
	 
	 return res;
}

Node *parse_term(ParserMgr *par_mgr) {
	Node *res = parse_factor(par_mgr);
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) 
		&& (par_mgr->curr_token->type == E_FSLASH_TOKEN 
		|| par_mgr->curr_token->type == E_ASTERISK_TOKEN)) {
		
		// Operation node.
		Node *bop = Node_new(1);

		if (par_mgr->curr_token->type == E_ASTERISK_TOKEN) {
			bop->type = E_TIMES_NODE;
		}
		else if (par_mgr->curr_token->type == E_FSLASH_TOKEN) {
			bop->type = E_DIV_NODE;
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			TokenMgr_next_token(par_mgr->tok_mgr);
		}

		TokenMgr_next_token(par_mgr->tok_mgr);
		bop->data->BinExpNode.left = res;
		bop->data->BinExpNode.right = parse_factor(par_mgr);

		if (!bop->data->BinExpNode.right || !bop->data->BinExpNode.left) {
			ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_INVALID_TYPES);
			TokenMgr_next_token(par_mgr->tok_mgr);
		}

		par_mgr->expr_depth++;
		res = bop;
	}
	
	return res;
}

Node *parse_expr(ParserMgr *par_mgr) {
	Node *res = parse_term(par_mgr);
	
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) 
		&&	(par_mgr->curr_token->type == E_PLUS_TOKEN 
		|| par_mgr->curr_token->type == E_MINUS_TOKEN
		|| is_compare_operator(par_mgr->curr_token->type))) {
		
		// Operation node.
		Node *bop = Node_new(1);

		if (par_mgr->curr_token->type == E_MINUS_TOKEN) {
			bop->type = E_MINUS_NODE;
		} else if (par_mgr->curr_token->type == E_PLUS_TOKEN) {
			bop->type = E_ADD_NODE;
		}
		else if (is_compare_operator(par_mgr->curr_token->type)) {
			bop->type = get_compare_type(par_mgr->curr_token->value);
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			TokenMgr_next_token(par_mgr->tok_mgr);
		}

		bop->data->BinExpNode.left = res;
		TokenMgr_next_token(par_mgr->tok_mgr);
		bop->data->BinExpNode.right = parse_term(par_mgr);

		if (!bop->data->BinExpNode.right || !bop->data->BinExpNode.left) {
			ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_INVALID_TYPES);
			TokenMgr_next_token(par_mgr->tok_mgr);
		}

		par_mgr->expr_depth++;
		res = bop;
	}
	
	return res;
}

Node *parse_array(ParserMgr *par_mgr) {
	
	// Store retrun node from each root level evaluation.
	Node *ret = NULL;
	// Store final array node.
	Node *arr = NULL;

	if (par_mgr->curr_token->type != E_LBRACKET_TOKEN)
		return NULL;

	// Instansiate array node.
	arr = node_new_array();
	
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && par_mgr->curr_token->type != E_RBRACKET_TOKEN) {
		
		// Next token.
		par_mgr_next(par_mgr);		

		switch(par_mgr->curr_token->type) {
			case E_INTEGER_TOKEN: 
				ret = parse_factor(par_mgr);
				break;
			case E_STRING_TOKEN: 
				ret = parse_string(par_mgr);
				break;
			case E_LBRACKET_TOKEN: 
				ret = parse_array(par_mgr);
				break;
			default:
				ret = NULL;
				break;
		}

		// Ignore empty values after comma.
		if (!ret) {
			continue;
		}

		// Resize if need be, prior to appending array node.
		if (arr->data->ArrayNode.dcap - arr->data->ArrayNode.dctr <= 5)
			arr->data->ArrayNode.items = grow_arr_nodes(arr);

		arr->data->ArrayNode.items[arr->data->ArrayNode.dctr++] = ret;	
	}
	
	if (par_mgr->curr_token->type != E_RBRACKET_TOKEN)
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_MISSING_BRACKET);
	else
		par_mgr_next(par_mgr);
	
	return arr;

}

Node *parse_assignment(ParserMgr *par_mgr) {
	// Store pointer to actual variable name.
	Token *tok_start_ptr = TokenMgr_current_token(par_mgr->tok_mgr);
	// Store pointer to subsequent tokens.
	par_mgr_next(par_mgr);
	// Reset expression depth.
	par_mgr->expr_depth = 0;
	
	// Final ast build by entire assignment.
	Node *ast = NULL;
	// Returned by each recursive handle.
	Node *expr = NULL;	
	// Leftmost node of root ast. 
	Node *lhand = NULL;

	// Check we can consume an EQUAL.
	if (par_mgr->curr_token && par_mgr->curr_token->type == E_EQUAL_TOKEN) {
		par_mgr_next(par_mgr);
		if ((expr = parse_expr(par_mgr)) || (expr = parse_array(par_mgr))) {

			// Add symbol if not exits.
			if (!SyTable_get_symbol(par_mgr->sy_table, tok_start_ptr->value))
				SyTable_add_symbol(par_mgr->sy_table, tok_start_ptr->value, NULL, tok_start_ptr->lineno ,E_IDN_TYPE);
			
			// Identifier.
			lhand = Node_new(0); 
			lhand->type = E_IDENTIFIER_NODE;
			lhand->value = tok_start_ptr->value;

			// Join to return ast from expression.
			ast = Node_new(1); 
			ast->type = E_EQUAL_NODE;
			ast->data->AsnStmtNode.left = lhand;
			ast->data->AsnStmtNode.right = expr;
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, TokenMgr_prev_token(par_mgr->tok_mgr), ERR_UNEXPECTED);
			par_mgr_next(par_mgr);
		}
	}
	else {
		ParserMgr_add_error(par_mgr->err_handle,TokenMgr_prev_token(par_mgr->tok_mgr), ERR_NAKED_VARIABLE);
		par_mgr_next(par_mgr);
	}
	return ast;
}

Node *parse_group(ParserMgr *par_mgr) {
	Token *grp = NULL;

	// Index group name token.
	grp = TokenMgr_prev_token(par_mgr->tok_mgr);

	if (!parser_expects(par_mgr, ERR_UNEXPECTED, 1, E_LBRACE_TOKEN)) return NULL;

	// Check if group already defined.
	if (SyTable_get_symbol(par_mgr->sy_table, par_mgr->curr_token->value)) {
		ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_GROUP_EXIST);
		par_mgr_next(par_mgr);
		return NULL;
	}

	par_mgr_next(par_mgr);

	// Group node itself. i.e {some_group}.
	Node *group = Node_new(1);
	// Previously read command.
	Node *prev = NULL;
	// Recently read command.
	Node *curr = NULL;
	// Setup group node data.
	group->value = grp->value;
	group->data->GroupNode.next = NULL;
	group->type = E_GROUP_NODE;

	// Create group entry.
	SyTable_add_symbol(par_mgr->sy_table, group->value, NULL, grp->lineno, E_GROUP_TYPE);
	
	// Iterate through commands and append to group.
	// Below will build a circular single linked list.
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && par_mgr->curr_token->type == E_STRING_TOKEN) {
		curr = parse_string(par_mgr);
		curr->data = malloc(sizeof(union SyntaxNode));
		
		if (!prev)
			group->data->GroupNode.next = curr;
		else
			prev->data->GroupNode.next = curr;
		prev = curr;
	}

	if (curr) curr->data->GroupNode.next = group;
	
	parser_expects(par_mgr, ERR_MISSING_BRACE, 1, E_RBRACE_TOKEN);
	
	par_mgr_next(par_mgr);
	
	return group;
}

Node *parse_keyword(ParserMgr *par_mgr) {
	// Peek into the next token in TokenMgr.
	Token *peek = TokenMgr_peek_token(par_mgr->tok_mgr);

	if (peek->type == E_LBRACE_TOKEN) {
		par_mgr_next(par_mgr);
		return parse_group(par_mgr);
	}

	// If valid arg isn't next then store error and move to next token.
	if (peek->type != E_STRING_TOKEN
		&& peek->type != E_MIXSTR_TOKEN
		&& peek->type != E_INTEGER_TOKEN
		&& peek->type != E_IDENTIFIER_TOKEN) {
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_current_token(par_mgr->tok_mgr), ERR_EMPTY_STMT);
		par_mgr_next(par_mgr);
		return NULL;
	}

	// Store keyword argument.
	Node *args = NULL;
	// Final statement node,
	Node *stmt = NULL;
	// Store function token.
	Token *name = par_mgr->curr_token;

	par_mgr_next(par_mgr);

	// If args is valid then store.
	// TODO: Consolidate below to one ?
	if ((args = parse_expr(par_mgr)) || (args = parse_string(par_mgr))) {
		stmt = Node_new(1);
		stmt->type = E_FUNC_NODE;
		stmt->value = name->value;
		stmt->data->FuncNode.args = args;
	}
	else {
		ParserMgr_add_error(par_mgr->err_handle, TokenMgr_current_token(par_mgr->tok_mgr), ERR_UNEXPECTED);
		par_mgr_next(par_mgr);
	}

	return stmt;
}

ParserMgr *ParseMgr_init(TokenMgr *tok_mgr, SyTable *sy_table, NodeMgr *node_mgr, Error *err) {
	if (!tok_mgr || !sy_table || !node_mgr || !err) return NULL;

	ParserMgr *par_mgr = ParserMgr_new();
	par_mgr->tok_mgr = tok_mgr;
	par_mgr->sy_table = sy_table;
	par_mgr->err_handle = err;
	par_mgr->node_mgr = node_mgr;
	par_mgr_sync(par_mgr);
	return  par_mgr;
}

Node *Parser_parse(ParserMgr *par_mgr) {
	if (null_check(par_mgr, "parser parse")) return NULL;

	// Store resulting tree.
	Node *ast = NULL;
	
	while (!TokenMgr_is_last_token(par_mgr->tok_mgr) && par_mgr->err_handle->error_ctr <= 1) {
		if (par_mgr->curr_token->type == E_IDENTIFIER_TOKEN) {
			ast = parse_assignment(par_mgr);
		}
		else if (par_mgr->curr_token->type == E_KEYWORD_TOKEN) {
			ast = parse_keyword(par_mgr);
		}
		else {
			ParserMgr_add_error(par_mgr->err_handle, par_mgr->curr_token, ERR_UNEXPECTED);
			par_mgr_next(par_mgr);
			continue;
		}

		if (ast != NULL) {
			ast->depth = par_mgr->expr_depth;
			NodeMgr_add_node(par_mgr->node_mgr, ast);
		}
		else {
			ParserMgr_skip_to(par_mgr, E_IDENTIFIER_TOKEN);
		}

		par_mgr_sync(par_mgr);
	}
	
	Error_print_all(par_mgr->err_handle);

	return ast;
}
