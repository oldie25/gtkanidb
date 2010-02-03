#include "control_net.h"

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "net.h"
#include "defs.h"

/* this structure contains the information needed
 * to keep track of the timing of network sends
 * we can't send too fast, or we'll get banned.
 * the timing is 1 packet every .5s -- though
 * initially we technically have some leeway
 * with this timing.  We'll not use that
 * aspect and instead just keep a proper
 * timing aspect.
 */
struct safety_hat {
	FILE* fp;
	time_t last_accessed;
};

static void
init_safety_hat(struct safety_hat * const sh)
{
	if (!sh) {
		return;
	}
	sh->fp = 0;
	sh->last_accessed = 0;
}

static int
open_safety_hat(struct safety_hat * const sh, const char* fname)
{
	if (!sh) {
		return (-1);
	}
	if (!fname) {
		return (-1);
	}
	sh->fp = fopen (fname, "wr"); /* re-write the file every time */
	if (!sh->fp) {
		/* we can't actively keep track of our timing now */
		printf ("fopen failed\n");
		perror (strerror(errno));
		return (-1);
	}
	return (0);
}

static int
write_safety_hat(struct safety_hat * const sh)
{
	if (!sh) {
		return (-1);
	}
	if (!sh->fp) {
		return (-1);
	}
	fprintf (sh->fp, "%ld", time(&sh->last_accessed));
	fflush (sh->fp); // push the change to disk
}

static time_t
read_safety_hat(struct safety_hat * const sh)
{
	int rd, fs;
	if (!sh) {
		return (-1);
	}
	fs = fseek (sh->fp, 0L, SEEK_END);
	if (fs == 0) {
		return (0);
	}
	rd = fscanf (sh->fp, "%ld", &sh->last_accessed);
	if (rd < 0) {
		return (-1);
	}
	return (sh->last_accessed);
}

static void
deinit_safety_hat(struct safety_hat * const sh)
{
	if (!sh) {
		return;
	}
	fclose (sh->fp);
}

static struct safety_hat s_hat;

/* copen() literally is just a wrapper around
 * network_open()
 * it initializes the s_hat structure
 */
int
copen()
{
	init_safety_hat (&s_hat);
	return (network_open());
}

/* cclose(), like copen() is a basic wrapper
 * around its equivalent, network_close()
 * it de-initializes the s_hat structure
 */
int
cclose(int fd)
{
	deinit_safety_hat (&s_hat);
	return (network_close(fd));
}

/* cwrite()
 * this function will open up the s_hat structure
 * check to see if it can write, wait if it must,
 * then it will write the packet
 */
int
cwrite(int fd, const char* str, size_t sz)
{
	time_t last;
	int nw;

	if (open_safety_hat(&s_hat, SAFETYFILE) < 0) {
		/* we couldn't open the safety file
		 * so we should return -1 since we couldn't
		 * verify when we're supposed to send something
		 */
		return (-1);
	}
	last = read_safety_hat (&s_hat);
	if (last < 0) {
		/* couldn't read from the file
		 * so, return -1
		 */
		printf ("%ld\n", last);
		return (-1);
	}
	printf ("delaying writing....\n");
	last = time (NULL);
	while (last - s_hat.last_accessed < 1) { last = time (NULL); }
	/* we've now waited the allotted time */
	nw = network_write (fd, str, sz);
	/* no matter what, even if we failed to write anything
	 * we should save the last time we wrote to the socket
	 */
	write_safety_hat (&s_hat);
	return (nw);
}

/* cread is another wrapper
 * it doesn't need to worry about timing
 * since we could receive all we want
 */
ssize_t cread(int fd, char* data, size_t sz)
{
	return (network_read(fd, data, sz));
}
