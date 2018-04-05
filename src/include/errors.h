/**
 * @file errors.h
 * @author Sayed Sadeed
 * @brief File containing a generic error interface which can be consumed by modules.
 * 
 * Error module is simply a store of malloc'ed strings. It is upto an individual module to 
 * determing how they produce those errors and the nature of them. Error simply keeps track
 * of them in a organized structure so that they can be correct freed and accessed.
 * 
 */

#ifndef ERRORS_H
#define ERRORS_H

// Initial amount of errors stored.
#define INIT_MAX_ERRORS 20

#include <string.h>

/**
 * @brief Store all the errors related to parsing process.
 */
typedef struct {
    char *errors[INIT_MAX_ERRORS];
    size_t error_ctr;
    size_t error_cap;
} Error;

/**
 * @brief Create new malloced Error instance.
 * 
 * @return New instance of Error or Null if failed.
 */
Error *Error_new(void);

/**
 * @brief Instruct error handler to release all of its stored errors.
 * 
 * @param err_handle Error instance.
 * @return 0 if successful or 1 if issues occurred.
 */
int Error_free(Error *err_handle);

/**
 * @brief Print all the errors currently stored by Error.
 * 
 * @param err_handle Error instance.
 */
void Error_print_all(Error *err_handle);

#endif
