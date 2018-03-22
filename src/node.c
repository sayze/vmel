#include <stdio.h>
#include <stdlib.h>
#include "node.h"

NodeMgr *NodeMgr_new() {
    NodeMgr *node_mgr = malloc(sizeof(NodeMgr)) ;  
    node_mgr->curr_node = NULL;
    node_mgr->node_ctr = 0;
    return node_mgr;
}

int NodeMgr_free(NodeMgr *node_mgr) {
    if (node_mgr == NULL)
        return 1;
    
    free(node_mgr);
    return 0;
}
