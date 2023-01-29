#include <stdio.h>
#include <string.h>

#include "auth.h"
#include "services.h"
#include "ftp.h"


int main()
{
    char token[152] = {'\0'};
    Services services ={ 0 };
    int ret;

    // stage 1: Get token for authentication
    printf("=====stage 1: Get token online=======\n");

    if (downloadtoken(token, sizeof(token) - 1)) {
        printf("Error, unable to get token\n");
        return -1;
    }
    printf("\n             *****TOKEN*****          \n");
    printf("%s\n\n", token);

    //Stage 2:
    printf("=====Stage 2: Get and Display Services=====\n");

    ret = getServices(token, &services);
    if(ret != 0) {
        printf("Error getting services\n");
    } else if (ret == 0) {
        printf("\n==========================\n");
        printf("========SERVICES==========\n");
        printf("==========================\n");
        displayServices(&services);
        printf("\n");
    }


    //Stage 3:
    printf("=====Stage 3: Download file from FTP server=====\n");

    if(downloadFile()){
        printf("Downloading error\n");
    }


    return 0;
}