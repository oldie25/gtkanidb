#ifndef __net_file_h_included__
#define __net_file_h_included__

#include <sys/types.h>

/* returns a fd */
int network_open();
/* closes the fd */
int network_close(int);
/* writes to the fd */
int network_write(int, const char*, size_t);
/* reads from the fd */
size_t network_read(int, char*, size_t);

#endif /* __net_file_h_included */
