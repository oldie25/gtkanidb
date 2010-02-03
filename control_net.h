#ifndef __control_net_file_h_included
#define __control_net_file_h_included

#include <sys/types.h>

/* returns the file descriptor */
int copen();
/* closes the file descriptor */
int cclose(int);
/* writes to the file descriptor */
int cwrite(int, const char*, size_t);
/* reads from the file descriptor */
ssize_t cread(int, char*, size_t);

#endif
