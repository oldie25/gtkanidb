#include <stdio.h>
#include <string.h>

#include "anidb.h"

int
main(int argc, const char** argv)
{
	if (anidb_init() < 0) {
		fprintf (stderr, "couldn't initialize\n");
		return (-1);
	}
	if (anidb_ping() != 300) {
		fprintf (stderr, "couldn't ping server\n");
		return (-1);
	}
	if (anidb_version() < 0) {
		fprintf (stderr, "couldn't get version\n");
	}
	printf ("success\n");
	return (0);
}
