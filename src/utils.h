/**
 * @file utils.h
 * @author Sayed Sadeed
 * @brief File containing collection of helper functions.
 */

#ifndef UTILS_H
#define UTILS_H

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


#endif