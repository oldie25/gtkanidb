#ifndef __net_file_h_included__
#define __net_file_h_included__

int network_open(const char* address, int port);
int network_close();
unsigned long network_send(const void* buf, unsigned int len);
unsigned long network_recv(void* buf, unsigned int len);

#endif /* __net_file_h_included */
