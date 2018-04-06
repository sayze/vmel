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
 * @brief wrapper around sprintf to convert an integer to a string.
 * 
 * Function take a input source and saves its contents in string format to
 * specified out char.
 * 
 * @param out Pointer to char where converted int is stored.
 * @para src Integer being converted.
 * @returns number of chars (exl NULL) if went successfully otherwise return negative;
 */
int int_to_string(char *out, int src);

/**
 * @brief Replace all the variables in string with corresponding values.
 * 
 * Function is useful for a linear string replace. Note that the placeholder values will be replaced
 * based on position of string in vars. 
 * @code
 * char **replace = {"sayed", "24"};
 * char *src = "Hello my name is @0 and I am @1 years old";
 * 
 * char *new = string_map_vars(src, replace, strlen(src), 2);
 * // new will be equal to
 * "Hello my name is sayed and I am 24 years old"
 * @endcode 
 * 
 * @param src A constant string which contains the variable placeholders.
 * @param vars Array of all the string which serve as replacement values.
 * @param src_len Length of src.
 * @param vars_len Number of string inside vars.
 * @return A malloc'ed char pointer containing the final mapped string. Will return NULL if error occurred.
 */
char *string_map_vars(const char *src, char **vars, size_t src_len, size_t vars_len);

#endif
