/**
 * @file vmel.h
 * @author Sayed Sadeed
 * @brief File containing definition for vmel source.
 */

#ifndef VMEL_H
#define VMEL_H

/**
 * @brief Read contents of file to buffer.
 *
 * This function will read the contents of a passed source file (*.vml).
 * It will strip out the inline and external comments from the source and
 * save the contents to a buffer. The buffer is created on heap so must be managed
 * by caller. See below example
 *
 * @code
 * char *filename = "~/Desktio/run.vml";
 * char *buffer = file_to_buffer(filename);
 * free(buffer) // when done.
 * @endcode
 *
 * @param filename Path to source file.
 * @return Pointer to buffer containing contents of file.
 */
char *file_to_buffer(char filename[]);

/**
 * @brief Print a custom error message with formatting.
 * @param msg The contents of error message
 */
void print_err(char msg[]);

/**
 * @brief Print help to cli.
 */
void print_usage(void);

#endif
