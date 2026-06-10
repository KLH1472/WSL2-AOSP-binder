#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/android/binder.h>
#include <linux/android/binderfs.h>
#include "binder_class.h"

struct svc_entry { char name[64]; uint32_t handle; } g_services[8];
int g_nsvc = 0;

class ServiceManager : public BBinder {
public:
    int32_t onTransact(uint32_t code, const Parcel &data, Parcel *reply) override {
        if (code == 1) { // CMD_REGISTER
            const char *name = (const char*)data.data;
            struct flat_binder_object *fbo =
                (struct flat_binder_object*)(data.data + data.offsets[0]);
            strcpy(g_services[g_nsvc].name, name);
            g_services[g_nsvc].handle = fbo->handle;
            printf("[sm] registered '%s' at handle %u\n", name, fbo->handle);
            g_nsvc++;
            reply->writeInt32(fbo->handle);
        }
        else if (code == 2) { // CMD_GET_SERVICE
            const char *name = (const char*)data.data;
            for (int i = 0; i < g_nsvc; i++) {
                if (strcmp(g_services[i].name, name) == 0) {
                    printf("[sm] query '%s' -> handle %u\n", name, g_services[i].handle);
                    reply->write_translated_handle(g_services[i].handle);
                    return 0;
                }
            }
            printf("[sm] query '%s' -> not found\n", name);
        }
        return 0;
    }
};

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);

    int ctl = open("/dev/binderfs/binder-control", O_RDWR|O_CLOEXEC);
    struct binderfs_device dev = {0};
    strcpy(dev.name, "triple");
    if (ioctl(ctl, BINDER_CTL_ADD, &dev) < 0) {
        if (errno != EEXIST) { perror("BINDER_CTL_ADD"); return 1; }
    }
    close(ctl);

    ProcessState::self();
    if (ioctl(ProcessState::self()->fd(), BINDER_SET_CONTEXT_MGR, 0) < 0)
        { perror("BINDER_SET_CONTEXT_MGR"); return 1; }

    ServiceManager sm;
    IPCThreadState::self()->setContextObject(&sm);

    printf("[sm] ready\n");
    IPCThreadState::self()->joinThreadPool(true);
    return 0;
}
