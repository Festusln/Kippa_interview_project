#ifndef _SERVICES_H_
#define _SERVICES_H_

typedef struct {
    char name[25][35];
    int size;
}Services;


short getServices(char *token, Services *services);
void displayServices(Services *services);

#endif
