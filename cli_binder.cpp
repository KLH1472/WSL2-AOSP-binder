#include <stdio.h>
#include <string.h>
#include "binder_class.h"

int main(int argc, char **argv) {
    if (argc < 3) { printf("usage: %s <string> <upper|lower>\n", argv[0]); return 1; }

    ProcessState::self();

    BpServiceManager sm;
    int32_t handle = sm.getService("UpperService");
    if (!handle) { printf("[client] service not found\n"); return 1; }
    printf("[client] found 'UpperService' at handle %d\n", handle);

    BpStringService svc(handle);
    const char *result = (strcmp(argv[2], "upper") == 0)
        ? svc.upper(argv[1]) : svc.lower(argv[1]);

    printf("[client] result: '%s'\n", result);
    return 0;
}
