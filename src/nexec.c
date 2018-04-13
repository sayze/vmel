#include "nexec.h"

NexecMgr *NexecMgr_new(void) {
	NexecMgr *n = malloc(sizeof(NexecMgr));
	n->err_handle = NULL;
	n->node_mgr = NULL;
	n->scope = 0;
	n->sy_table = NULL;
	return n;
}

int NexecMgr_free(NexecMgr *nexec_mgr) {
	if (!nexec_mgr)
		return -1;

	free(nexec_mgr);
	return 0;
}

int Nexec_func_node(NexecMgr *nexec_mgr) {
	if (!nexec_mgr)
		return -1;

	return 0;
}

int Nexec_expr_node(NexecMgr *nexec_mgr) {
	if (!nexec_mgr)
		return -1;

	return 0;
}

int Nexec_group_node(NexecMgr *nexec_mgr) {
	if (!nexec_mgr)
		return -1;

	return 0;
}
