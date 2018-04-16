/**
 * @file conf.h
 * @author Sayed Sadeed
 * @brief Module contains all common constants used throughout application. 
 */

#ifndef CONF_H
#define CONF_H

/**
 * Application level config.
 * 
 * PROG_CLI_MODE program is in cli mode.
 * PROG_SRC_MODE program executing script.
 */
#define PROG_CLI_MODE 1
#define PROG_SRC_MODE 2

/**
 * Default/Initial structure sizing.
 * 
 * INIT_SYTABLE_SIZE initial size of symbol table.
 * INIT_NODEMGR_SIZE initial number of nodes that can be stored inside NodeMgr class.
 * INIT_TOKMGR_TOKS_SIZE initial number of tokens that can be stored inside TokenMgr class.
 */
#define INIT_SYTABLE_SIZE 7
#define INIT_NODEMGR_SIZE 100
#define INIT_TOKMGR_TOKS_SIZE 40

/**
 * Fixed structure sizing.
 * 
 *  TOKENTYPE_SIZE maximum length a token type maybe.
 *  
 */
#define TOKENTYPE_SIZE 20

#endif
