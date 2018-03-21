/**
 * @file node.h
 * @author Sayed Sadeed
 * @brief The Node module represents a "node" inside an AST.
 */

/**
 * @brief Node correlates to a node within a tree.
 * 
 * This is used to map tokens to an AST. Each node has a left value, right value and
 * its centre/root.
 */
struct Node {
    struct Node *left;
    char *value;
    struct Node *right;
};

/**
 * @brief NodeMgr manages the Nodes at a higher level. 
 * 
 * This provides a high level interfacing for the syntax tree. It is preferred to use this
 * for anything node related as it manages internal memory allocs and deallocs.
 * Struct will mantain all nodes and provide functions to interface with tree.
 */
typedef struct {
    struct Node **curr_node;
    struct Node nodes[100];
    int node_ctr;
} NodeMgr;

/**
 * @brief Free all resources creates by node manager. Including node manager itself.
 *
 * @param node_mgr Pointer to the NodeMgr instance.
 * @return 1 if anything went wrong other return 0.
 */
int NodeMgr_free(NodeMgr *node_mgr);

/**
 * @brief Create node manager malloc'ed.
 * 
 * This function acts as a constructor for the Node manager.
 * 
 * @return newly created NodeMgr pointer.
 */
NodeMgr *NodeMgr_new();
