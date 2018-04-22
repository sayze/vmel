/**
 * @file vstring.h
 * @author Sayed Sadeed
 * @brief Implementation of a string module to allow better string handling.
 */

#ifndef VSTRING_H
#define VSTRING_H

#define INIT_STRING_SIZE 15

#include <string.h>

/**
 * @brief Struct representing a VString.
 * 
 */
typedef struct {
	size_t str_size;
	size_t str_cap;
	char *str;
} VString;

/**
 * @brief Instantiate a new empty VString string object.
 * 
 * Function will create an emptu VString object along with a
 * malloc'ed str based on default size.
 * 
 * @return VString object.
 */
VString VString_new(void); 

/**
 * @brief Set the entire VString object to new string value.
 * 
 * Will wipe out existing data. Note that if the current buffer size
 * isn't significantly bigger than no reallocation will be made..
 * 
 * @param vstr VString instance.
 * @param str String value.
 * @return Pointer to VString.
 */
VString *VString_set(VString *vstr, char *str);

/**
 * @brief Push a single character into a VString.
 * 
 * Function will append a single character to the passed
 * VString instance. Unlike VString_pushs() which operates on string.
 * 
 * @param vstr VString instance.
 * @param c char to append.
 * @return Pointer to VString.
 */
VString *VString_pushc(VString *vstr, char c);

/**
 * @brief Push a string into a VString.
 * 
 * Function will append a string to the passed
 * VString instance. Unlike VString_pushc() which operates on a single char.
 * 
 * @param vstr VString instance.
 * @param str String to append to instance.
 * @return Pointer to VString.
 */
VString *VString_pushs(VString *vstr, char *str);


/**
 * @brief Instantiate a new VString instance with a string parameter.
 * 
 * Create a VString object and set the string value 
 * as the passed str parameter. It is also possible to set the capcity using
 * the second paramter cap. This will allocate the buffer size to equate to cap.
 * Note that there is no implicit enforcement on fixed sizing however it will help
 * lessen memory wastage if known string sizes are instantiated with an explicit capacity.
 * 
 * @code
 * VString fixed_string = VString_create("I am a string", 12);
 * VString string = VString_create("I am a string too", 0);
 * @endcode
 * 
 * @param str Value to set str to.
 * @param cap a predefined size if constant.
 * @return Pointer to VString object.
 */
VString VString_create(char *str, size_t cap); 

int VString_replace(VString *vstr, char *find, char *replace);

/**
 * @brief Free resources.
 * 
 * @param vstr Pointer to VString object.
 */
int VString_free(VString *vstr);

#endif
