#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "control_net.h"

const char* ANIDBSERVER = "localhost";
const int REMOTEPORT = 9000;
const int LOCALPORT = 9000;
const char* SAFETYFILE = "./tm";

int
main(int argc, const char** argv)
{
	int fd;
	int i, j;
	const char* str = "shit man";
	char blah[25] = {"\0"};

	fd = copen ();

	if (fd < 0) {
		printf ("failure\n");
		return (-1);
	}
	for (j = 0; j < 5; j++) {
		i = cwrite (fd, str, strlen(str));
		if (i < 0) {
			printf ("failure to write\n");
			return (-1);
		}
		i = cread (fd, blah, sizeof(blah));
		if (i <= 0) {
			printf ("failure to read\n");
			return (-1);
		}
		printf ("'%s' was the string read\n", blah);
	}
	network_close (fd);
	printf ("success\n");
	return (0);
}
