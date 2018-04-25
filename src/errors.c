#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "utils.h"

Error *Error_new(void) {
	Error *err_handle = malloc(sizeof(Error));
	err_handle->error_cap = INIT_MAX_ERRORS;
	err_handle->error_ctr = 0;
	return err_handle;
}

int Error_add(Error *err_handle, char *err) {
	if (!err_handle || !err)
		return -1;

	if (err_handle->error_ctr == err_handle->error_cap)
		return 0;

	err_handle->errors[err_handle->error_ctr] = err;
	err_handle->error_ctr++;
	return 0;
}

int Error_free(Error *err_handle) {
	if (null_check(err_handle, "error free")) return -1;

	if (err_handle->error_ctr != 0) {
		for (size_t in = 0; in < err_handle->error_ctr; in++) {
			free(err_handle->errors[in]);
		}
	}
	free(err_handle);	
	return 0;
}

void Error_print_all(Error *err_handle) {
	if (null_check(err_handle, "error print all")) return;
	
	if (err_handle->error_ctr != 0) {
		for (size_t in = 0; in < err_handle->error_ctr; in++) {	
			printf("%s\n",err_handle->errors[in]);
		}
	}
}
