#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ftp.h"
#include "network.h"


short userLogin(int sock, char *username, char *password, NetworkParams *netParam)
{
    int ret = -1;
    memset(netParam->request, 0x00, sizeof(netParam->request));

    netParam->requestLen = sprintf(&netParam->request[netParam->requestLen], "USER %s\r\nPASS %s\r\n", username, password);
#ifdef DEBUG
printf("\nFTP login request: %s\n", netParam->request);
printf("FTP request length: %d\n", netParam->requestLen);
    printf("Sending ftp login details...\n");
#endif
    
    if (sendRequest(netParam, sock) < 0) {
        printf("Send ftp login details failed\n");
        return -1;
    }

    printf("Receiving ftp login response...\n");
    if (receiveResponse(netParam, sock) < 0 ){
        printf("Receive ftp login details failed\n");
        return -1;
    }
    
    return 0;
}

short downloadFile(void)
{
    int userEnterPassword = 0;          //Set this for user to enter password or not
    NetworkParams netParam = { 0 };
    int sock = -1;
    char username[32] = {'\0'};
    char password[32] = {'\0'};
    char cmd[128] = {'\0'};
    char filename[] = "resources";

    struct stat fileInfo;

    strncpy(netParam.hostName, "files.000webhost.com", sizeof(netParam.hostName) - 1);
    netParam.port = 21;
    netParam.isSsl = 0;

    printf("Connecting to ftp server...\n");
    sock = connecTotHost(&netParam);
    if (sock < 0) {
        printf("Unable to connect to ftp server\n");
        return -1;
    }

    printf("Connected to ftp server\n");

    if(!userEnterPassword) { 
        strncpy(username, "linuxpos", sizeof(username) - 1);
        strncpy(password, "GS6@4Aa&ih*8kO*zDJTW", sizeof(password) - 1);
    } else if (userEnterPassword) {
        printf("LOGIN PAGE\n\n");

        printf("Enter Username: ");
        fgets(username, sizeof(username) - 1, stdin);

        printf("Enter Password: ");
        fgets(password, sizeof(password) - 1, stdin);
    }

    if (userLogin(sock, username, password, &netParam)) {
        printf("Login error\n");
        return -1;
    }
    
    
    memset(netParam.request, 0x00, sizeof(netParam.request));
    sprintf(netParam.request, "GET /%s\r\n", filename);

    printf("Sending file download request...\n");
    if (sendRequest(&netParam, sock) < 0) {
        printf("Send file download command failed\n");
        return -1;
    }

    printf("Receiving file download response...\n");
    if (receiveResponse(&netParam, sock) < 0 ){
        printf("Receive file download failed\n");
        return -1;
    }

    
    printf("Downloaded filename: %s\n", netParam.response);
    if(stat(netParam.response, &fileInfo)) {
        printf("Unable to get file name\n");
        return -1;
    }
    printf("Dowloaded file size: %ld\n", fileInfo.st_size);
    
    if (S_ISREG(fileInfo.st_mode)) {
        printf("The file type is : regular file\n");
    } else if (S_ISDIR(fileInfo.st_mode)) {
        printf("The file type is : directory\n");
    } else {
        printf("The file type is : other\n");
    }


    return 0;
}
