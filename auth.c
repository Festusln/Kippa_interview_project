#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "network.h"
#include "auth.h"

#include "cJSON.h"
#include "cJSON_Utils.h"

static short parseTokenResp(const char* response, char* tokenStr, const int tokenSize)
{
    cJSON *json = NULL;
    cJSON *status = NULL;
    cJSON *message = NULL;
    cJSON *data = NULL;
    cJSON *token = NULL;
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
    if (!data || cJSON_IsNull(data) || !cJSON_IsObject(data)) {
        printf("Data object not found\n");
        goto clean_exit;
    } 

    token = cJSON_GetObjectItemCaseSensitive(data, "token");
    if (!token || cJSON_IsNull(token) || !cJSON_IsString(token)) {
        printf("Token not found\n");
        goto clean_exit;
    }

    strncpy(tokenStr, token->valuestring, tokenSize);
    ret = 0;

clean_exit:
    cJSON_Delete(json);
    return ret;
}

short downloadtoken(char *token, size_t size)
{
    NetworkParams netParam = { 0 };
    int ret = -1;
    char requestBody[240] = {'\0'};

    strncpy(netParam.hostName, HOST_NAME, sizeof(netParam.hostName)-1);
    strncpy(netParam.endPoint, "api/login", sizeof(netParam.endPoint)-1);
    netParam.port = 443;
    netParam.isSsl = 1;


    sprintf(requestBody, "{\"username\":\"%s\",\"password\":\"%s\" }", "testuser", "test123@$2023");

    netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "%s /%s HTTP/1.1\r\n", "POST", netParam.endPoint);
 	netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "Host: %s\r\n", netParam.hostName);
    netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "Content-Type: application/json\r\n");
    netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "Content-Length: %ld\r\n", strlen(requestBody));
    netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "Connection: keep-alive\r\n\r\n");
	netParam.requestLen += sprintf(&netParam.request[netParam.requestLen], "%s", requestBody);

    if (sendAndReceive(&netParam)) {
        return -1;
    }

    if (parseTokenResp(netParam.response, token, size)) {
        return -1;
    }

    return 0;

}