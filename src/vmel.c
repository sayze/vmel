#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "parser.h"
#include "utils.h"

#define CLI_BUFFER_LIMIT 50

int main(int argc, char *argv[]) {
		
	// Input stream used for file or cli.
	char *buff_in = NULL;

	// Source file has been provided then write into buffer.
	// Will fail and exit if file invaid.
	if (argc == 2)
		buff_in = file_to_buffer(argv[1]);

	// Instantiate new token manager.
	TokenMgr *tok_mgr = TokenMgr_new(); 
	// Store error status.
	int err = 0;

	// Buffer is null then switch to cli mode.
	// Otherwise build token with file stream;
	if (buff_in == NULL) {
		buff_in = calloc(CLI_BUFFER_LIMIT, sizeof(char)); // TODO: bad ! A line might consist more than 100 chars.
		printf("Enter quit to terminate cli\n");
		while (1) {
			fputs("> ", stdout);
			fgets(buff_in, CLI_BUFFER_LIMIT, stdin);
			if (!strncmp(buff_in, "quit", 4)) {
				break;
			}
			err = build_tokens(buff_in, tok_mgr);

			if (!err) {
				int ret = parse_expr(tok_mgr);
				if (ret  >= 0)
					printf("Output is %d\n", ret);
				else
					printf("Error calculating expression\n");
			}

			// discard the built up collection of tokens.
			TokenMgr_clear_tokens(tok_mgr);
		}
	}
	else {
		err = build_tokens(buff_in, tok_mgr);
		parse_expr(tok_mgr);
	}

	#ifdef DEBUG
		TokenMgr_print_tokens(tok_mgr);
	#endif

	// Free resources.
	TokenMgr_free(tok_mgr);
	free(buff_in);
}


