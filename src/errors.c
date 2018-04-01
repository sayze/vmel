#include <stdio.h>
#include "errors.h"

Error *Error_new(void) {
	Error *err_handle = malloc(sizeof(Error));
	err_handle->error_cap = INIT_MAX_ERRORS;
	err_handle->error_ctr = 0;
	err_handle->errors = malloc(sizeof(char *) * err_handle->error_cap);
	return err_handle;
}

void Error_free(Error *err_handle) {
	if (err_handle != NULL && err_handle->error_ctr != 0) {
		for (size_t in = 0; in < err_handle->error_ctr; in++) {
			free(err_handle->errors[in]);
		}
		free(err_handle);
	}
}

void Error_print_all(Error *err_handle) {
	if (err_handle != NULL && err_handle->error_ctr != 0) {
		for (size_t in = 0; in < err_handle->error_ctr; in++) {	
			printf("%s\n",err_handle->errors[in]);
		}
	}
}
