#include "net.h"

#include <stdio.h>
#include <string.h>

int
main(int argc, const char** argv)
{
	int fd;
	int i;
	const char* str = "shit man";
	char blah[25] = {"\0"};

	fd = network_open ();

	if (fd < 0) {
		printf ("failure\n");
		return (-1);
	}
	i = network_write (fd, str, strlen(str));
	if (i < 0) {
		printf ("failure to write\n");
		return (-1);
	}
	i = network_read (fd, blah, sizeof(blah));
	if (i <= 0) {
		printf ("failure to read\n");
		return (-1);
	}
	printf ("'%s' was the string read\n", blah);
	network_close (fd);
	printf ("success\n");
	return (0);
}
