#include <string.h>
#include <stdio.h>

#include "network.h"
#include "services.h"

#include "cJSON.h"
#include "cJSON_Utils.h"


static short parseServicesResp(const char *response, Services *services)
{
    cJSON *json = NULL;
    cJSON *status = NULL;
    cJSON *message = NULL;
    cJSON *data = NULL;
    cJSON *element = NULL;
    cJSON *code = NULL;
    cJSON *name = NULL;
    int size, i;
    int ret = -1;

    json = cJSON_Parse(strchr(response, '{'));
    if (!json || cJSON_IsNull(json)) {
        printf("Uexpected response\n");
        return -1;
    }
    
    status = cJSON_GetObjectItemCaseSensitive(json, "status");
    if (!status || cJSON_IsNull(status) || !cJSON_IsString(status)) {
        printf("Unknown status\n");
        goto clean_exit;
    }

    message = cJSON_GetObjectItemCaseSensitive(json, "message");
    if (!message || cJSON_IsNull(message) || !cJSON_IsString(message)) {
        printf("Message not found\n");
        goto clean_exit;
    }

    if (strncmp(status->valuestring, "success", strlen("success"))) {
        printf("Unable to get token: %s", message->valuestring);
        goto clean_exit;
    }

    data = cJSON_GetObjectItemCaseSensitive(json, "data");
    if (!data || cJSON_IsNull(data) || !cJSON_IsArray(data)) {
        printf("Data Array not found\n");
        goto clean_exit;
    }

    //Getting Services
    size = cJSON_GetArraySize(data);
    if (size <= 0) {
        printf("No service found\n");
        goto clean_exit;
    }
    
    services->size = size;

    for (i = 0; i < size; i++) {

        element = cJSON_GetArrayItem(data, i);

        code = cJSON_GetObjectItemCaseSensitive(element, "code");
        name = cJSON_GetObjectItemCaseSensitive(element, "name");

        if (!code || !cJSON_IsString(code)) {
            printf("servvice code not found\n");
            goto clean_exit;
        } else if (!name || !cJSON_IsString(name)) {
            printf("Service name not found\n");
            goto clean_exit;
        }
            strncpy(services->name[i], name->valuestring, sizeof(services->name[0]) -1 );
    }

    ret = 0;

    clean_exit:
    cJSON_Delete(json);
    return ret;
}

short getServices(char *token, Services *services)
{
    NetworkParams netParam = { 0 };
    int ret = -1;

    strncpy(netParam.endPoint, "api/services", sizeof(netParam.endPoint)-1);
    strncpy(netParam.hostName, HOST_NAME, sizeof(netParam.hostName));
    netParam.port = 443;
    netParam.isSsl = 1;

    netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "%s /%s HTTP/1.1\r\n", "GET", netParam.endPoint);
 	netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "Host: %s\r\n", netParam.hostName);
 	netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "Authorization: Bearer %s\r\n", token);
	netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "\r\n");

    if(sendAndReceive(&netParam)) {
        return -1;
    }

    if (parseServicesResp(netParam.response, services)) {
        return -1;
    }
    return 0;
}

void displayServices(Services *services)
{
    int i;
    for (i = 0; i < services->size; i++) {
        printf("%d. %s\n", i+1, services->name[i]);
    }

    return;
}