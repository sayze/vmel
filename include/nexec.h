/**
 * @file nexec.h
 * @author Sayed Sadeed
 * @brief The execution module implementation. This module described how each node in an AST is executed.
 */

#include "sytable.h"
#include "node.h"
#include "errors.h"
#include "vstring.h"

/**
 * @brief Maintain state between tree executions.
 */
typedef struct {
	SyTable *sy_table;
	Error *err_handle;
	NodeMgr *node_mgr;
	Node *curr_node;
	VString buff;
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
 * @brief Constructor for NexecMgr.
 * 
 * Create a new instance of NexecMgr and assign required 
 * structures.
 * 
 * @param sy_table instance of SyTable.
 * @param node_mgr instance of NodeMgr.
 * @param err_handle instance of Error.
 * @return instance of NexecMgr.
 */
NexecMgr *Nexec_init(SyTable *sy_table, NodeMgr *node_mgr, Error *err_handle);

/**
 * @brief Provides the ability to execute individual nodes independant
 * of the entire tree.
 * 
 * This function is useful for incremental executions such as CLI where
 * execution is performed on a predefined state such as pressing enter on CLI.
 *  
 */
int Nexec_exec(NexecMgr *nexec_mgr, Node *node);

/**
 * @brief Add a custom error string to the list of errors stored in Error.
 * 
 * @param err_error Instance of Error.
 * @param offender offending variable name.
 */
void NexecMgr_add_error(Error *err_handle, char *offender);
