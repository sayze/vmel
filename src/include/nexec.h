/**
 * @file nexec.h
 * @author Sayed Sadeed
 * @brief The execution module implementation. This module described how each node in an AST is executed.
 */

#include "sytable.h"
#include "node.h"
#include "errors.h"

/**
 * @brief Maintain state between tree executions.
 */
typedef struct {
	SyTable *sy_table;
	Error *err_handle;
	NodeMgr *node_mgr;
	Node *curr_node;
	unsigned int scope;
} NexecMgr;

/**
 * @brief Create instance of NexecMgr.
 * 
 * @return Pointer to new NexecMgr or NULL.
 */
NexecMgr *NexecMgr_new(void);

/**
 * @brief Free instance of NexecMgr;
 * 
 * @param nexec_mgr Pointer to NexecMgr instance.
 * @returns 0 if successfully freed otherwise -1.
 */
int NexecMgr_free(NexecMgr *nexec_mgr);

/**
 * @brief Execute a function node.
 * 
 * Function will evaluate a function node such as print.
 * It will call subsequent translation methods or perform the command directly
 * depending on the function.
 * 
 * @param nexec_mgr Pointer to NexecMgr instance.
 * @return 0 if success otherwise returns -1.
 */
int Nexec_func_node(NexecMgr *nexec_mgr);

/**
 * @brief Execute an expression node.
 * 
 * Evaluate an expression and assign the return value
 * to the respective variable within symbol table.
 * 
 * @param nexec_mgr Pointer to NexecMgr instance.
 * @return 0 if success otherwise returns -1.
 */
int Nexec_expr_node(NexecMgr *nexec_mgr);

/**
 * @brief Execute a group node.
 * 
 * Evaluate the commands within a group and perform
 * the appropriate actions.
 * 
 * @param nexec_mgr Pointer to NexecMgr instance.
 * @return 0 if success otherwise returns -1.
 */
int Nexec_group_node(NexecMgr *nexec_mgr);

/**
 * @brief Entry point to Node Execution.
 * 
 * Will start iterating over all the nodes and performing
 * the appropriate command.
 */
void Nexec_init(SyTable *sy_table, NodeMgr *node_mgr);
