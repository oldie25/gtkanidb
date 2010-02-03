#include "net.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "defs.h" /* local program definitions */

static struct sockaddr_in g_socket; /* main socket */
static struct sockaddr_in l_socket; /* local socket */

/* if it fails to open a socket, returns -1
 * else, returns a valid file descriptor
 */
int
network_open()
{
	struct hostent* hostinfo;
	struct in_addr address;
	const struct timeval tmo = { 30, 0 }; /* 30s timeout */
	int fd = -1;
	const int flag = 1;
	int i;
	
	memset ((char*)&g_socket, 0, sizeof(g_socket)); /* zero socket */

	address.s_addr = inet_addr (ANIDBSERVER); /* find address of server */
	errno = 0;
	if (address.s_addr != INADDR_NONE) {
		hostinfo = gethostbyaddr ((char*)&address,
						sizeof(address), AF_INET);
		if (errno != 0) {
			perror ("couldn't resolve");
			return (fd);
		}
	} else {
		hostinfo = gethostbyname (ANIDBSERVER);
		if (!hostinfo) {
			perror ("couldn't resolve");
			return (fd);
		}
		if ((size_t)hostinfo->h_length > sizeof(g_socket.sin_addr) ||
				hostinfo->h_length < 0) {
			perror ("illegal address");
			return (fd);
		}
	}
	/* we have an address */
	g_socket.sin_family = hostinfo->h_addrtype;
#ifndef linux
	g_socket.sin_len = sizeof (g_socket);
#endif
	memcpy (&g_socket.sin_addr, hostinfo->h_addr_list[0],
			(size_t)hostinfo->h_length);
	g_socket.sin_port = htons (REMOTEPORT);

	/* now, lets actually get a socket! */
	fd = socket (g_socket.sin_family, SOCK_DGRAM, IPPROTO_UDP);

	if (fd < 0) {
		perror ("couldn't open a socket");
		return (fd);
	}

	errno = 0;
	i = setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &tmo, sizeof(tmo));
	if (i < 0) {
		perror ("couldn't set timeout");
		return (-1);
	}

	i = setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
	if (i < 0) {
		perror ("couldn't set address");
		return (-1);
	}

	memset ((char*)&l_socket, 0, sizeof(l_socket));
#ifndef linux
	l_socket.sin_len = sizeof (l_socket);
#endif

	l_socket.sin_family = g_socket.sin_family;
	l_socket.sin_port = htons (LOCALPORT);

	i = bind (fd, (struct sockaddr*)(&l_socket), sizeof(l_socket));
	if (i < 0) {
		perror ("couldn't bind");
		return (-1);
	}
	i = connect (fd, (struct sockaddr*)(&g_socket), sizeof(g_socket));
	if (i < 0) {
		perror ("couldn't connect");
		return (-1);
	}
	return (fd);
}

/* closes the file descriptor
 * returns 0
 */
int
network_close(int fd)
{
	int i;

	if (fd > 0) {
		i = shutdown (fd, SHUT_RDWR);
	}
	return (0);
}

/* writes len of data to fd
 * returns # of bytes written
 */
int
network_write(int fd, const char* data, size_t len)
{
	int i;

	if (fd < 0) {
		return (-1);
	}

	errno = 0;
	i = write (fd, (const void*)data, len);
	if (i < 0) {
		/* check error */
		perror (strerror(errno));
	}
	return (i);
}

/* reads len of data from network to data
 * returns # of bytes read
 */
ssize_t
network_read(int fd, char* data, size_t len)
{
	ssize_t i;

	if (fd < 0) {
		return (-1);
	}

	errno = 0;
	memset (data, 0, len);
	i = read (fd, data, len);

	if (i < 0) {
		perror (strerror(errno));
		return (-1);
	}
	return (i);
}
