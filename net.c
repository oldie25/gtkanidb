#include "net.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static int fd = -1;
static struct sockaddr_in out_sock;
static struct sockaddr_in in_sock;
static time_t last_sent = 0;

int create_socket(const char* address, int port);
int initialize_sockets();
int connect_socket();
void zero_socket(struct sockaddr_in* sock);
void delay_send();

int
network_open(const char* address, int port) {
	if (!address) {
		return (-1);
	}
	if (port <= 0 || port >= 65535) {
		return (-1);
	}
	if (fd > 0) {
		/* connection already established */
		return (0);
	}
	if (create_socket(address, port)) {
		return (-1);
	}
	if (initialize_sockets()) {
		return (-1);
	}
	if (connect_socket()) {
		return (-1);
	}
	return (0);
}

int
network_close() {
	int r = 0;
	if (fd != -1) {	
		r = close (fd);
		if (r != 0) {
			return (-1);
		}
	}
	zero_socket (&out_sock);
	zero_socket (&in_sock);
	return (0);
}

unsigned long
network_send(const void* buf, unsigned int len) {
	unsigned long sent = 0;
	if (!buf) {
		return (0);
	}
	delay_send ();
	sent = send (fd, buf, len, 0);
	if (sent == -1) {
		return (0);
	}
	return (sent);
}

unsigned long
network_recv(void* buf, unsigned int len) {
	unsigned long got = 0;
	if (!buf) {
		return (0);
	}
	memset (buf, 0, len);
	got = recv (fd, buf, len, 0);
	if (got == -1) {
		return (-1);
	}
	return (got);
}

int
create_socket(const char* address, int port) {
	struct hostent* hp;

	fd = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd == -1) {
		return (1);
	}
	hp = gethostbyname (address);
	if (hp == NULL) {
		return (1);
	}
	zero_socket (&out_sock);
	out_sock.sin_family = AF_INET;
	out_sock.sin_port = htons (port);
	memcpy (&out_sock.sin_addr, hp->h_addr_list[0], (unsigned)(hp->h_length));
	zero_socket (&in_sock);
	in_sock.sin_family = AF_INET;
	in_sock.sin_addr.s_addr = htonl (INADDR_ANY);
	in_sock.sin_port = htons (port);
	return (0);
}

int
initialize_sockets() {
	struct timeval timeout = {30,0}; /* 30s timeout */
	int dumby = 0;
	int r = 0;

#define CHECK_FAIL(val) \
	if (val != 0) return (1)

	r = setsockopt (fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	CHECK_FAIL (r);
	r = setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &dumby, sizeof(dumby));
	CHECK_FAIL (r);
#ifdef SO_REUSEPORT
	r = setsockopt (fd, SOL_SOCKET, SO_REUSEPORT, &dumby, sizeof(dumby));
	CHECK_FAIL (r);
#endif
	r = setsockopt (fd, SOL_SOCKET, SO_KEEPALIVE, &dumby, sizeof(dumby));
	CHECK_FAIL (r);
	return (0);
}

int
connect_socket() {
	int r = 0;
	r = bind (fd, (struct sockaddr*)(&in_sock), sizeof(in_sock));
	if (r != 0) {
		return (1);
	}
	r = connect (fd, (struct sockaddr*)(&out_sock), sizeof(out_sock));
	if (r != 0) {
		return (1);
	}
	return (0);
}

void
zero_socket(struct sockaddr_in* sock) {
	memset ((void*)sock, 0, sizeof(struct sockaddr_in));
}

void
delay_send() {
	int delay = last_sent - time(NULL);
	if (last_sent != 0 && delay > 0) {
		sleep ((unsigned int)delay);
	}
	last_sent = time(NULL) + 6;
}
