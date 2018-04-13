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
 * @param nexec_mgr Pointer to NexecMgr instance.
 */
int Nexec_func_node(NexecMgr *nexec_mgr);

/**
 * @brief Execute an expression node.
 * 
 * @param nexec_mgr Pointer to NexecMgr instance.
 */
int Nexec_expr_node(NexecMgr *nexec_mgr);

/**
 * @brief Execute a group node.
 * 
 * @param nexec_mgr Pointer to NexecMgr instance.
 */
int Nexec_group_node(NexecMgr *nexec_mgr);
