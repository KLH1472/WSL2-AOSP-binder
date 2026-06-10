#include <stdio.h>
#include <string.h>
#include "binder_class.h"

#define LOG_TAG "Client"
#include "log.h"

int main(int argc, char **argv) {
    if (argc < 3) { printf("usage: %s <string> <upper|lower>\n", argv[0]); return 1; }

    ProcessState::self();

    BpServiceManager sm;
    int32_t handle = sm.getService("UpperService");
    if (!handle) { LOG("service not found"); return 1; }
    LOG("found 'UpperService' at handle %d", handle);

    BpStringService svc(handle);
    const char *result = (strcmp(argv[2], "upper") == 0)
        ? svc.upper(argv[1]) : svc.lower(argv[1]);

    LOG("result: '%s'", result);
    return 0;
}
