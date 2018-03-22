/**
 * @file utils.h
 * @author Sayed Sadeed
 * @brief File containing collection of helper functions.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string.h>

/**
 * @brief Print help to cli.
 */
void print_usage(void);

/**
 * @brief Utility function to determine if char is one of accepted identifiers.
 * 
 * Check to see if a character is deemed valid in the language syntax.
 *
 * @param id The char to get validated.
 * @return 1 If it matches accepted identifiers or return 0 if it doesn't.
 */	
int is_valid_identifier(char id);

/**
 * @brief Read contents of file to buffer.
 *
 * This function will read the contents of a passed source file (*.vml).
 * It will simply dump the contents of a file to a buffer.
 * The buffer is created on heap so must be managed by caller. See below example
 *
 * @code
 * char *filename = "~/Desktop/run.vml";
 * char *buffer = file_to_buffer(filename);
 * free(buffer) // when done.
 * @endcode
 *
 * @param filename Path to source file.
 * @return Pointer to buffer containing contents of file.
 */
char *file_to_buffer(const char *);

/**
 * @brief convert a string of numbers to integer.
 * 
 * @param str string to be converted.
 * @param len the length of the string.
 * @return converted integer if successful or 0.
 */
int string_to_int(char *str, size_t len);

/**
 * @brief Increase size of string by using realloc.
 * 
 * @param curr_size Existing size 
 * @param str pointer to the string.
 * @return newly created string.
 */
char  *increase_string(size_t curr_size, char *str);
#endif
