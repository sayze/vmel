#include <stdio.h>
#include <stdlib.h>
#include "vstring.h"

// Implicit function to allocate more memory for string.
static VString *VString_grow_str(VString *vstr, size_t factor) {
	size_t str_cap = sizeof(char) * factor;
	char *new_str = realloc(vstr->str, sizeof(char) * factor);
	vstr->str = new_str;
	vstr->str_cap = str_cap;
	return vstr;
}

// Determine if we need to grow/shrink.
static int VString_needs_grow(VString *vstr, size_t n_size) {
	return (n_size >= vstr->str_cap) 
			|| (n_size < vstr->str_cap 
			&& vstr->str_cap - n_size >= SHRINK_TRIG_SIZE);
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

	size_t n_size = vstr->str_size + sizeof(char) - 1;
	
	if (VString_needs_grow(vstr, n_size)) {
		VString *n_vstr = VString_grow_str(vstr, n_size * 2);
		vstr = n_vstr;
	}

	vstr->str[n_size] = c;
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

int VString_free(VString *vstr) {
	if (!vstr->str)
		return -1;

	free(vstr->str);
	return 0;
}
