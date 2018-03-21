# Changelog

## v0.0.1
* Initial release only consisting of the ability to perform arithmetic operations.
* Ability to run from source file or cli.
* Will successfully tokenize majority of standard language literals.

### Issues
* Still need to clean up fixed memory allocations for `char[]` to use realloc's.
* As a result of above there are limitations on how long a line of input can be.
