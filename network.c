#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "network.h"

SSL_CTX *ctx;
SSL *ssl;


SSL_CTX* initCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}


short connecTotHost(NetworkParams *netParam)
{
    int sock = -1;
    struct sockaddr_in server_addr;
    struct hostent *hostEnv;
    int ret;

    if (netParam->isSsl) {
        SSL_library_init();
        ctx = initCTX();
    }

    hostEnv = gethostbyname(netParam->hostName);
    if (hostEnv == NULL){
       herror("Invalid host name");
       return -1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
#ifdef DEBUG
        perror("create socket error");
#endif
        return -1;
    }

    server_addr.sin_family = AF_INET;     
    server_addr.sin_port = htons(netParam->port);
    server_addr.sin_addr = *((struct in_addr *)hostEnv->h_addr);
    bzero(&(server_addr.sin_zero),8); 

    ret = connect(sock, (struct sockaddr *)&server_addr,sizeof(struct sockaddr));
    if (ret < 0) {
#ifdef DEBUG
        printf("Connection error");
#endif
        return -1;
    }

    if (netParam->isSsl) {
        ssl = SSL_new(ctx);      /* create new SSL connection state */
        SSL_set_fd(ssl, sock);    /* attach the socket descriptor */
        if ( SSL_connect(ssl) == -1 ) {   /* perform the connection */
            ERR_print_errors_fp(stderr);
        }
#ifdef DEBUG 
        printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
#endif
    }

    return sock;

}

short sendRequest(NetworkParams *netParams, int sock)
{
    int ret = -1;

    if (netParams->isSsl) {
        ret = SSL_write(ssl, netParams->request, netParams->requestLen);
    } else {
        ret = send(sock, netParams->request, netParams->requestLen, 0);
    }

    if(ret < 0){
#ifdef DEBUG 
        printf("sending error\n");
#endif
        SSL_free(ssl);
        close(sock);      
        SSL_CTX_free(ctx);
        return -1;
    }

#ifdef DEBUG
        printf("Request successfully sent\n");
#endif

    return ret;

}

int receiveResponse(NetworkParams *netParams, int sock)
{
    size_t ret = -1;
    char responseBuffer[4500] = { '\0' };

    netParams->responseLen = 0;

    if (netParams->isSsl) {
        while(SSL_read_ex(ssl, responseBuffer, sizeof(responseBuffer), &ret) > 0){

            if(ret == -1) {
#ifdef DEBUG
                perror("Error receiving response");
#endif
                SSL_free(ssl);     
                SSL_CTX_free(ctx);
                return -1;
            }

            memcpy(&netParams->response[netParams->responseLen], responseBuffer, ret);
            netParams->responseLen += ret;
        }

    } else if (!netParams->isSsl) {

        while(ret = recv(sock, responseBuffer, 1, 0) > 0){
            if(ret == -1){
#ifdef DEBUG
                perror("Error receiving response");
#endif
                close(sock);      
                return -1;
            }

            memcpy(&netParams->response[netParams->responseLen], responseBuffer, ret);
            netParams->responseLen += ret;
        }
    }

#ifdef DEBUG
    printf("Response : %s\n", netParams->response);
#endif

    SSL_free(ssl);
    close(sock);      
    SSL_CTX_free(ctx);

    return netParams->responseLen;
}


//Function that create connection, send request and receive response
int sendAndReceive(NetworkParams *netParams)
{
    int sock = -1;
    int ret = -1;

#ifdef DEBUG
    printf("Request: %s\n", netParams->request);
#endif
    sock = connecTotHost(netParams);
    if (sock < 0) {
        printf("Error... Connection to server failed\n");
        return -1;
    }
    printf("Connected to host\n");

    ret = sendRequest(netParams, sock);
    if (ret < 0) {
        printf("Error sending data\n");
        return -1;
    }
    printf("Data sent successfully\n");

    ret = receiveResponse(netParams, sock);

    if (ret <= 0) {
        printf("No response received\n");
        return -1;
    }


    return 0;
}
