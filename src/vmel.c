#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "utils.h"

int main(int argc, char *argv[]) {
	// Input stream can be file content or stdin.
	char *buff_in = NULL;

	// Source file has been provided then write into buffer.
	// Will fail and exit if file invaid.
	if (argc == 2)
		buff_in = file_to_buffer(argv[1]);

	// Instantiate new token manager.
	TokenMgr *tok_mgr = TokenMgr_new(); 

	// buffer is null then switch to cli mode.
	if (buff_in == NULL) {
		char input[100]; // TODO bad ! A line might consist more than 100 chars.
		while () {

		}
	}



	// Attempt to fill token manager with tokens.
	build_tokens(buff_in, tok_mgr);

	// Parse tokens.
	TokenMgr_print_tokens(tok_mgr);

	// Free resources.
	TokenMgr_free(tok_mgr);
	free(buff_in);
}
