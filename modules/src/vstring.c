#include <stdio.h>
#include <stdlib.h>
#include "vstring.h"

// Implicit function to allocate more memory for string.
static VString *VString_grow_str(VString *vstr, size_t factor) {
	size_t str_cap = sizeof(char) * factor;
	char *new_str = realloc(vstr->str, str_cap);
	vstr->str = new_str;
	vstr->str_cap = str_cap;
	return vstr;
}

// Determine if we need to grow/shrink.
static int VString_needs_grow(VString *vstr, size_t n_size) {
	return n_size >= vstr->str_cap;
}

VString VString_new(void) {
	VString vstr;
	vstr.str_cap = INIT_STRING_SIZE;
	vstr.str_size = 0;
	vstr.str = malloc(vstr.str_cap * sizeof(char) + 1);
	*vstr.str = '\0';
	return vstr;
}

VString VString_create(char *str, size_t cap) {
	VString vstr;
	vstr.str_cap = cap < 1 ? INIT_STRING_SIZE : cap;
	vstr.str_size = 0;
	vstr.str = malloc(vstr.str_cap * sizeof(char) + 1);
	*vstr.str = '\0';
	if (str) 
		VString_pushs(&vstr, str);
	return vstr;
}

VString *VString_set(VString *vstr, char *str) {
	if (!vstr || !str)
		return NULL;
	
	size_t n_size = strlen(str);
	
	if (VString_needs_grow(vstr, n_size)) {
		VString *n_vstr = VString_grow_str(vstr, n_size * 2);
		vstr = n_vstr;
	}

	strncpy(vstr->str, str, strlen(str)+1);
	vstr->str_size = n_size;
	return vstr;
}

VString *VString_pushc(VString *vstr, char c) {
	if (!vstr)
		return NULL;

	size_t n_size = vstr->str_size + sizeof(char);
	
	if (VString_needs_grow(vstr, n_size)) {
		VString *n_vstr = VString_grow_str(vstr, n_size * 8);
		vstr = n_vstr;
	}

	vstr->str[n_size-1] = c;
	vstr->str[n_size] = '\0';
	vstr->str_size = n_size;
	return vstr;
}

VString *VString_pushs(VString *vstr, char *str) {
	if (!vstr || !str)
		return NULL;
	
	size_t n_size = vstr->str_size + strlen(str);
	
	if (VString_needs_grow(vstr, n_size)) {
		VString *n_vstr = VString_grow_str(vstr, n_size * 2);
		vstr = n_vstr;
	}

	strncat(vstr->str, str, strlen(str));
	vstr->str_size = n_size;
	return vstr;
}

int VString_replace(VString *vstr, char *find, char *replace) {
	if (!vstr || !find || !replace)
		return -1;
	
	// Length of find value (needle).
	size_t len_find = strlen(find);

	if (len_find < 1)
		return 0;
	
	// Length of replace value.
	size_t len_rep = strlen(replace);
	// Number of occurrences in source/haystack.
	int num_finds = 0;
	// Iterator pointer.
	char *itr = vstr->str;
	// Pointer to original source.
	char *hstack = vstr->str;
	// Length of original source;
	size_t len_hstack = vstr->str_size;
	// Last word in string.
	char *lword = itr + len_hstack - len_find;

	for (num_finds = 0; (itr = strstr(itr, find)); num_finds++) {
		itr += len_find;
	}

	// Find new size of string.
	len_hstack = len_hstack - ((len_find - len_rep) * num_finds);

	while (num_finds--) {
		itr = strstr(hstack, find);

		// A shrink.
		if (len_find > len_rep) {
			
			// Only shift to the left if source is not last word.
			// "Hello source" <--- no left shit required.
			// "Hello source there" <--- left shift required.
			if (itr != lword)
				memmove((itr + len_find) - (len_find - len_rep), itr + len_find, vstr->str_size - len_find); 
			
			memmove(itr, replace, len_rep);
		}
		// A grow.
		else if (len_find < len_rep) {
			if (VString_needs_grow(vstr, len_hstack)) {
				VString *n_vstr = VString_grow_str(vstr, len_hstack * 2);
				
				if (!n_vstr)
					return -1;
				
				// After realloc reassign pointers.
				vstr = n_vstr;
				hstack = vstr->str;
				itr = strstr(hstack, find);
			}

			// Only shift to the right if source is not last word.
			// "Hello bigger source" <--- no right shit required.
			// "Hello bigger source there" <--- right shift required.
			if (itr != lword)
				memmove(itr + len_rep, itr + len_find, vstr->str_size - len_find); 
			
			memmove(itr, replace, len_rep);
		}
		// A direct copy.
		else if (len_find == len_rep) {
			memmove(itr, replace, len_rep);
		}
	}

	vstr->str_size = len_hstack;
	vstr->str[len_hstack] = '\0';

	return 0;
}

int VString_free(VString *vstr) {
	if (!vstr->str)
		return -1;

	free(vstr->str);
	return 0;
}
