# Changelog

## v0.0.1
* Initial release only consisting of the ability to perform arithmetic operations.
* Ability to run from source file or cli.
* Will successfully tokenize majority of standard language literals.
* Print errors when unrecognized character formations are encountered.

### Issues
* Still need to clean up fixed memory allocations for `char[]` to use realloc's.
* As a result of above there are limitations on how long a line of input can be.

## v0.0.2
* Replaced most fixed `char[]` allocations with dynamic arrays.
* Introduced AST module.
* Created more Managers for Node, Parser and Errors.
* Created error module for storing parse errors.
* Integrated valgrind to help fix memory leaks.

### Issues
* Function which builds tokens still uses a `char[100]` this limits string to be 100 chars long.

## v0.1.2
* Introduced VString module to offer better string management
* Binary arithmetic operations functional 
* Comparison operators for string, integers and mixstring with ascii encoding for string
* Introduced print function
* Node execution module cleaned up
  - Uses a single VString to temp string storage (in most places)
  - Handles more operators with different types
* Tokenizer refactored to use VString in build token process
* Changed syntactic structure of groups to be more consistent with functions
* Group commands can be denoted as strings or simply word sequence, delimited by newline
* Created separate test directory for language sanity testing
