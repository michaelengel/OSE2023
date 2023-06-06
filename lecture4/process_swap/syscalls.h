/* entweder...
#define SYS_PRINTSTRING 1
#define SYS_PUTACHAR 2
#define SYS_GETACHAR 3
*/

// oder...
enum { SYS_PRINTSTRING = 1, SYS_PUTACHAR, SYS_GETACHAR, SYS_EXIT = 42 };
