/**
 * @file utils.h
 * @author Sayed Sadeed
 * @brief File containing collection of helper functions.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include "conf.h"

/**
 * @brief Print help to cli.
 */
void print_usage(void);

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
 * @brief Convert a string of numbers to integer.
 * 
 * @param str string to be converted.
 * @param len the length of the string.
 * @return converted integer if successful or 0.
 */
int string_to_int(char *str, size_t len);

/**
 * @brief Wrapper around sprintf to convert an integer to a string.
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
 * @brief Shorthand for strcmp.
 * 
 * Function will remove the need to check for 0 on performing strcmp.
 * Assuming strings are null terminated.
 * 
 * @param str1 Pointer to first string.
 * @param str2 Pointer to second string.
 * @return 1 if strings match, 0 if not match.
 */
int string_compare(char *str1, char *str2);

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

/**
 * @brief Duplicate a string into malloc'ed space and returns pointer,
 * 
 * The passed string is assumed to be null terminated. Could
 * lead to undefined behaviour and/or leaks if terminator is not
 * present. Also memory is allocated on heap so caller is responsible 
 * for freeing resource.
 * 
 * @param src string to be duplicated.
 * @returns a pointer to the new string or NULL if failed.
 */
char *string_dup(char *src);

/**
 * Find all variables inside a c string.
 * 
 * Function will find all the variables present in
 * string and return a malloc'ed char ** (2d array).
 * It is responsibility of caller to free resources. Also
 * the number of rows will be defined in the passed vlen argument.
 * 
 * @code
 * char *phrase = "I have $num_pets pets";
 * size_t len = 0;
 * char **vars = string_find_var(phrase, '$', &len);
 * // above will return
 * len = 1
 * vars = [ ["num_pets"] ] 
 * 
 * @endcode
 * 
 * @param hstack source string which will be searched and incremented.
 * @param vprefix the prefix character which quailies string as variable.
 * @param rows number of rows after variable extraction.
 * @return Pointer to all strings found inside hstack.
 * 
 */
char *string_find_var(char *hstack, char vprefix);

/**
 * @brief Determine if char is one of accepted identifiers.
 * 
 * Check to see if a character is deemed valid in the language syntax.
 *
 * @param id The char to get validated.
 * @return 1 If it matches accepted identifiers or return 0 if it doesn't.
 */	
int is_valid_identifier(char id);

/**
 * @brief Print null object error with name.
 * 
 * This is helpful for debugging purposes and to ensure unexpected
 * NULL values are caught.
 * 
 * @param obj pointer to object.
 * @param hint optional text to provide additional information.
 * @return 0 if check passes otherwise 1.
 */
int null_check(void *obj,char *hint);

/**
 * @brief convert a string to a ascii representation.
 * 
 * Function will convert each individual character to their
 * respective ascii value aggregated. String is assumed to be null terminated.
 * 
 * @param str_rep String representation.
 * @return Aggregated ascii representation.
 */
unsigned int string_to_ascii(char *str_rep);

#endif
