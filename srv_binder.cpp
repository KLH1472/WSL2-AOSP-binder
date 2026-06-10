#include <stdio.h>
#include "binder_class.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    ProcessState::self();

    BnStringService *svc = new BnStringService();
    BpServiceManager sm;
    sm.addService("UpperService", svc);
    printf("[server] registered 'UpperService'\n");

    IPCThreadState::self()->joinThreadPool(true);
    return 0;
}
