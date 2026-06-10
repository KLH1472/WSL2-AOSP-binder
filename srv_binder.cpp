#include <stdio.h>
#include "binder_class.h"

#define LOG_TAG "Server"
#include "log.h"

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    ProcessState::self();

    BnStringService *svc = new BnStringService();
    BpServiceManager sm;
    sm.addService("UpperService", svc);
    LOG("registered 'UpperService'");

    IPCThreadState::self()->joinThreadPool(true);
    return 0;
}
