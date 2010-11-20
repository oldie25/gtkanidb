#include "anidb.h"

#include "net.h"

#include <stdio.h>
#include <string.h>

#include <assert.h>

const int REPLY_LENGTH = 1400;
const int RETCODE = 5;
const int VERSIONLENGTH = 50;
const char* ANIDBSERVER = "api.anidb.net";
const int REMOTEPORT = 9000;
const int LOCALPORT = 9000;
const int CLIENTVERSION = 1;
const char* CLIENTNAME = "gtkanidb";
const int APIVERSION = 3;
static char SESSION[9];

enum cmds {
	PING = 0,
	VERSION,
	LAST
};

const char *cmd_strings[] = {
	"PING",
	"VERSION",
	NULL
};

struct anidb_reply {
	int code;
	char* msg;
	int msglen;
};

int
anidb_init() {
	static int once = 0;
	if (!once) {
		int r = 0;
		r = network_open (ANIDBSERVER, REMOTEPORT);
		if (r < 0) {
			fprintf (stderr, "failed to connect to %s\n", ANIDBSERVER);
			return (-1);
		}
	}
	return (0);
}

int
anidb_login(const char* usr, const char* pwd) {
	assert (usr);
	assert (pwd);
	char auth[REPLY_LENGTH];
	char reply[REPLY_LENGTH];
	char retcode[RETCODE];
	int ret = 0;

	memset (auth, 0, REPLY_LENGTH);

	snprintf (auth, REPLY_LENGTH-1,
					"AUTH user=%s&pass=%s&protover=%d&client=%s&clientver=%d",
					usr, pwd, APIVERSION, CLIENTNAME, CLIENTVERSION);
	ret = network_send (auth, strlen(auth));
	if (ret < 0) {
		return (-1);
	}
	memset (reply, 0, REPLY_LENGTH);
	ret = network_recv (reply, sizeof(reply));
	if (ret < 0) {
		return (-1);
	}
	memset (retcode, 0, RETCODE);
	fprintf (stdout, "full reply was %s\n", reply);
	/* only grab the retcode for right now */
	sscanf (reply, "%4[^ ]%s", retcode, reply);

	ret = atoi (retcode);
	memset (SESSION, 0, sizeof(SESSION));
	switch (ret) {
		case 200:
			/* login was OK, so grab the session variable */
			sscanf (reply, "%8[^ ]%s", SESSION, reply);
			break;
		case 201:
			fprintf (stdout, "New Version of %s is available\n", CLIENTNAME);
			sscanf (reply, "%8[^ ]%s", SESSION, reply);
			break;
		case 500:
			fprintf (stderr, "Login Failed\n");
			return (-1);
			break;
		case 503:
			fprintf (stderr, "You must upgrade %s to the latest version!\n", CLIENTNAME);
			return (-1);
			break;
		case 504:
			fprintf (stderr, "Client has been banned: %s\n", reply);
			return (-1);
			break;
		case 505:
			fprintf (stderr, "Invalid input to login!\n");
			return (-1);
			break;
		case 601:
			fprintf (stderr, "ANIDB is unavailable; try again later\n");
			return (-1);
			break;
	}
	return (ret);
}

int
anidb_logout() {
	char logout[REPLY_LENGTH];
	char reply[REPLY_LENGTH];
	char retcode[RETCODE];
	int ret = 0;

	memset (logout, 0, REPLY_LENGTH);
	snprintf (logout, REPLY_LENGTH - 1, "LOGOUT s=%s", SESSION);
	ret = network_send (logout, strlen(logout));
	if (ret < 0) {
		return (-1);
	}
	memset (reply, 0, REPLY_LENGTH);
	ret = network_recv (reply, REPLY_LENGTH-1);
	if (ret < 0) {
		return (-1);
	}
	memset (retcode, 0, RETCODE);
	sscanf (reply, "%4[^ ]%s", retcode, reply);

	ret = atoi (retcode);
	switch (ret) {
		case 203:
			fprintf (stdout, "logout succeeded\n");
			break;
		case 403:
			fprintf (stderr, "was not logged in\n");
			return (-1);
	}
	return (ret);
}

int
anidb_ping() {
	char reply[REPLY_LENGTH];
	char retcode[RETCODE];
	int ret = 0;

	ret = network_send (cmd_strings[PING], strlen(cmd_strings[PING]));
	if (ret < 0) {
		return (-1);
	}
	memset (reply, 0, REPLY_LENGTH);
	ret = network_recv (reply, sizeof(reply));
	if (ret < 0) {
		return (-1);
	}

	memset (retcode, 0, RETCODE);
	sscanf (reply, "%4[^ ]%s", retcode, reply);

	fprintf (stdout, "reply was: %s\n", reply);
	return (atoi(retcode));
}

int
anidb_version() {
	char reply[REPLY_LENGTH];
	char code[RETCODE];
	char command[8]; /*strlen("version")*/
	char version[VERSIONLENGTH];
	int r = 0;

	r = network_send (cmd_strings[VERSION], strlen(cmd_strings[VERSION]));
	if (r < 0) {
		return (-1);
	}
	memset (reply, 0, REPLY_LENGTH);
	memset (code, 0, RETCODE);
	memset (command, 0, 8);
	memset (version, 0, VERSIONLENGTH);
	r = network_recv (reply, sizeof(reply));
	if (r < 0) {
		return (-1);
	}
	sscanf (reply, "%4[^ ]%8s%s", code, command, version);
	fprintf (stdout, "version is: %s\n", version);
	return (0);
}
