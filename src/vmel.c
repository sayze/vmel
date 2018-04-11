#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "parser.h"
#include "node.h"
#include "utils.h"

#define CLI_BUFFER_LIMIT 50

int main(int argc, char *argv[]) {
	
	// Input stream used for file or cli.
	char *buff_in = NULL;

	// Source file has been provided then write into buffer.
	if (argc == 2)
		buff_in = file_to_buffer(argv[1]);

	// Don't bother building tokens empty file read.
	if (!buff_in)
		return 0;

	// Instantiate new token manager.
	TokenMgr *tok_mgr = TokenMgr_new(); 
	// Store ast returned from parser.
	NodeMgr *node_mgr = NULL;
	// Symbol table instance.
	SyTable *sy_table = NULL;
	// Store error status.
	int err = 0;

	err = TokenMgr_build_tokens(buff_in, tok_mgr);

	if (!err) {
		sy_table = SyTable_new();
		node_mgr = parser_init(tok_mgr, sy_table);
	}	

	#ifdef DEBUG
		SyTable_print_symbols(sy_table);
		TokenMgr_print_tokens(tok_mgr);
	#endif

	// Free resources.
	SyTable_free(sy_table);
	NodeMgr_free(node_mgr);
	TokenMgr_free(tok_mgr);
	free(buff_in);

	return 0;
}


	// Buffer is null then switch to cli mode.
	// Otherwise build token with file stream;
	// if (buff_in == NULL) {
	// 	buff_in = calloc(CLI_BUFFER_LIMIT, sizeof(char)); // TODO: bad ! A line might consist more than 100 chars.
	// 	printf("Enter quit to terminate cli\n");
	// 	while (1) {
	// 		fputs("> ", stdout);
	// 		fgets(buff_in, CLI_BUFFER_LIMIT, stdin);
	// 		if (!strncmp(buff_in, "quit", 4)) {
	// 			break;
	// 		}
	// 		err = TokenMgr_build_tokens(buff_in, tok_mgr);
	// 	}	
	// 	parser_init(tok_mgr);
	// }
	// else {
	// 	err = TokenMgr_build_tokens(buff_in, tok_mgr);
	// 	if (!err)
	// 		parser_init(tok_mgr);
	// }
	


