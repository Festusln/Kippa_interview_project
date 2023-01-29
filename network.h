#ifndef _NETWORK_H_
#define _NETWORK_H_

// #define DEBUG

#define HOST_NAME "linuxpos.000webhostapp.com"
typedef struct {
    char hostName[52];
    int port;
    char endPoint[52];
    char request[1024];
    char response[1024];
    int requestLen;
    int responseLen;
    int isSsl;
} NetworkParams;

int sendAndReceive(NetworkParams *netParams);
short connecTotHost(NetworkParams *netParam);
short sendRequest(NetworkParams *netParams, int sock);
int receiveResponse(NetworkParams *netParams, int sock);


#endif