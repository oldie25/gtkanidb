#include "anidb.h"

#include "net.h"

#include <stdio.h>
#include <string.h>

const int REPLY_LENGTH = 1400;
const int RETCODE = 5;
const int VERSIONLENGTH = 50;
const char* ANIDBSERVER = "api.anidb.net";
const int REMOTEPORT = 9000;
const int LOCALPORT = 9000;
const int CLIENTVERSION = 1;
const int APIVERSION = 2;

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
	sscanf (reply, "%4[^ ]%8s%s", code, command, version);
	fprintf (stdout, "version is: %s\n", version);
	return (0);
}
