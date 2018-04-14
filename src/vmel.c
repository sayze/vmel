#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "parser.h"
#include "node.h"
#include "nexec.h"
#include "utils.h"

#define CLI_BUFFER_LIMIT 50
#define PROG_CLI_MODE 1
#define PROG_SRC_MODE 2

int main(int argc, char *argv[]) {
	
	// Input stream used for file.
	char *buff_in = NULL;
	// Hold parsed tokens/lexeme.
	TokenMgr *tok_mgr = NULL;
	// Resulting ast from parsing.
	NodeMgr *node_mgr = NULL;
	// Symbol table.
	SyTable *sy_table = NULL;
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

	if (run_mode == PROG_SRC_MODE) {
		buff_in = file_to_buffer(argv[1]);

		tok_mgr = TokenMgr_new();		
		err = TokenMgr_build_tokens(buff_in, tok_mgr);

		// tokenizer was successfull.
		if (!err) {
			sy_table = SyTable_new();
			node_mgr = parser_init(tok_mgr, sy_table);
			nexec_mgr = Nexec_init(sy_table, node_mgr);

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
		
		#ifdef DEBUG
			SyTable_print_symbols(sy_table);
			TokenMgr_print_tokens(tok_mgr);
		#endif

		NexecMgr_free(nexec_mgr);
		SyTable_free(sy_table);
		NodeMgr_free(node_mgr);
		TokenMgr_free(tok_mgr);
		free(buff_in);
	}
	else {
		// Run cli mode.
		printf("running cli mode\n");
	}

	return 0;
}
