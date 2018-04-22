#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Custom includes.
#include "tokenizer.h"
#include "parser.h"
#include "node.h"
#include "nexec.h"
#include "errors.h"
#include "utils.h"
#include "conf.h"

int main(int argc, char *argv[]) {

	// Input stream used for file.
	char *buff_in = NULL;
	// Hold parsed tokens/lexeme.
	TokenMgr *tok_mgr = NULL;
	// Resulting ast from parsing.
	NodeMgr *node_mgr = NULL;
	// Symbol table.
	SyTable *sy_table = NULL;
	// Parser Manager.
	ParserMgr *par_mgr = NULL;
	// Error handler.
	Error *err_handle = NULL;
	// Executioner of AST.
	NexecMgr *nexec_mgr = NULL;
	// Error status.
	int err = 0;
	// Run type.
	int run_mode = 0;

	// Determine which mode. CLI or Source file. 
	if (argc >= 2) {
		run_mode = string_compare(argv[1], "-c") ? PROG_CLI_MODE : PROG_SRC_MODE;
	} else {
		print_usage();
		return 0;
	}

	buff_in = file_to_buffer(argv[1]);
	tok_mgr = TokenMgr_new();		
	err = TokenMgr_build_tokens(buff_in, tok_mgr);
	free(buff_in);
	
	// Tokenizer was successfull.
	if (!err) {
		
		// Instantiate required structs.
		sy_table = SyTable_new();
		node_mgr = NodeMgr_new();
		err_handle = Error_new();

		// Initialise Parser with correct structs.
		par_mgr = ParseMgr_init(tok_mgr, sy_table, node_mgr, err_handle);

		Parser_parse(par_mgr);

		// Free since its no longer needed.
		ParserMgr_free(par_mgr);

		// No errors then proceed to execute nodes.
		if (err_handle->error_ctr == 0) {
			
			// Initialise NexecMgr.
			nexec_mgr = Nexec_init(sy_table, node_mgr, err_handle);

			#ifdef DEBUG
				printf("--------------------------------------\n");
				printf("** Program Output **\n");
				printf("--------------------------------------\n");
			#endif

			// Iterate through nodes in generated ast and execute.
			for (size_t i = 0; i < node_mgr->nodes_ctr; i++) {
				Nexec_exec(nexec_mgr, node_mgr->nodes[i]);
			}
		}
	}

	#ifdef DEBUG
		SyTable_print_symbols(sy_table);
		TokenMgr_print_tokens(tok_mgr);
	#endif

	// Free all resources.
	NexecMgr_free(nexec_mgr);
	Error_free(err_handle);
	SyTable_free(sy_table);
	NodeMgr_free(node_mgr);
	TokenMgr_free(tok_mgr);

	return 0;
}
