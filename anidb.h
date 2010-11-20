#ifndef __anidb_file_h_included__
#define __anidb_file_h_included__

int anidb_init();
int anidb_login(const char* usr, const char* pwd);
int anidb_logout();
int anidb_ping();
int anidb_version();

#endif
