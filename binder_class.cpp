#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/android/binder.h>
#include "binder_class.h"

// ===================== ProcessState =====================
ProcessState::ProcessState() {
    mFD = open("/dev/binderfs/triple", O_RDWR|O_CLOEXEC);
    if (mFD < 0) { perror("ProcessState open"); exit(1); }
    mVM = mmap(NULL, 1024*1024, PROT_READ, MAP_PRIVATE, mFD, 0);
    if (mVM == MAP_FAILED) { perror("ProcessState mmap"); exit(1); }
}

ProcessState* ProcessState::self() {
    static ProcessState gProc;
    return &gProc;
}

// ===================== IPCThreadState =====================
static IPCThreadState gIPC;
IPCThreadState* IPCThreadState::self() { return &gIPC; }

void IPCThreadState::talkWithDriver(bool doReceive) {
    struct binder_write_read bwr;
    memset(&bwr, 0, sizeof(bwr));
    bwr.write_size  = mOut.cursor;
    bwr.write_buffer = (uintptr_t)mOut.data;
    bwr.read_size   = doReceive ? sizeof(mIn.data) : 0;
    bwr.read_buffer = (uintptr_t)mIn.data;
    if (ioctl(ProcessState::self()->fd(), BINDER_WRITE_READ, &bwr) < 0)
        { perror("ioctl"); exit(1); }
    mOut.cursor = 0;                     // data sent, clear out buffer
    mIn.cursor  = bwr.read_consumed;
    mIn.setReadPos(0);
}

int IPCThreadState::waitForResponse(Parcel *reply) {
    while (1) {
        talkWithDriver(true);
        while (mIn.readPos() < mIn.cursor) {
            uint32_t cmd = mIn.readInt32();
            switch (cmd) {
            case BR_REPLY: {
                struct binder_transaction_data tr;
                memcpy(&tr, mIn.data + mIn.readPos(), sizeof(tr));
                mIn.setReadPos(mIn.readPos() + sizeof(tr));
                if (reply) {
                    BinderBuf tmp;
                    memcpy(tmp.data, (void*)(uintptr_t)tr.data.ptr.buffer, tr.data_size);
                    tmp.cursor = tr.data_size;
                    tmp.n_offsets = tr.offsets_size / sizeof(binder_size_t);
                    if (tr.offsets_size)
                        memcpy(tmp.offsets, (void*)(uintptr_t)tr.data.ptr.offsets, tr.offsets_size);
                    reply->setData(tmp);
                }
                return 0;
            }
            case BR_DEAD_REPLY:  return -1;
            case BR_FAILED_REPLY: return -2;
            case BR_TRANSACTION_COMPLETE: break;
            case BR_RELEASE:
            case BR_DECREFS:
                mIn.setReadPos(mIn.readPos() + sizeof(binder_uintptr_t) * 2);
                break;
            default: executeCommand(cmd); break;
            }
        }
    }
}

int32_t IPCThreadState::transact(int32_t handle, uint32_t code,
                                  const Parcel &data, Parcel *reply) {
    bc_txn_t wr;
    wr.cmd = BC_TRANSACTION;
    memset(&wr.txn, 0, sizeof(wr.txn));
    wr.txn.target.handle = handle;
    wr.txn.code   = code;
    wr.txn.data.ptr.buffer  = (binder_uintptr_t)data.data;
    wr.txn.data_size        = data.cursor;
    wr.txn.data.ptr.offsets = (binder_uintptr_t)data.offsets;
    wr.txn.offsets_size     = data.n_offsets * sizeof(binder_size_t);

    memcpy(mOut.data, &wr, sizeof(wr));
    mOut.cursor = sizeof(wr);
    return waitForResponse(reply);
}

void IPCThreadState::executeCommand(uint32_t cmd) {
    switch (cmd) {
    case BR_TRANSACTION: {
        struct binder_transaction_data tr;
        memcpy(&tr, mIn.data + mIn.readPos(), sizeof(tr));
        mIn.setReadPos(mIn.readPos() + sizeof(tr));

        Parcel data, reply;
        BinderBuf tmp;
        memcpy(tmp.data, (void*)(uintptr_t)tr.data.ptr.buffer, tr.data_size);
        tmp.cursor = tr.data_size;
        tmp.n_offsets = tr.offsets_size / sizeof(binder_size_t);
        if (tr.offsets_size)
            memcpy(tmp.offsets, (void*)(uintptr_t)tr.data.ptr.offsets, tr.offsets_size);
        data.setData(tmp);

        BBinder *target = tr.target.ptr
            ? (BBinder*)(uintptr_t)tr.cookie
            : mContextObj;

        if (target)
            target->onTransact(tr.code, data, &reply);

        if (!(tr.flags & 1))   // TF_ONE_WAY = 1
            sendReply(reply);
        break;
    }
    case BR_NOOP:
    case BR_SPAWN_LOOPER:
    case BR_TRANSACTION_COMPLETE:
    case BR_DEAD_BINDER:
    case BR_CLEAR_DEATH_NOTIFICATION_DONE:
        break;
    case BR_RELEASE:
    case BR_DECREFS:
        mIn.setReadPos(mIn.readPos() + sizeof(binder_uintptr_t) * 2);
        break;
    default:
        mIn.setReadPos(mIn.cursor);  // skip unknown cmd + payload
        break;
    }
}

void IPCThreadState::sendReply(Parcel &reply) {
    bc_txn_t wr;
    wr.cmd = BC_REPLY;
    memset(&wr.txn, 0, sizeof(wr.txn));
    wr.txn.data.ptr.buffer  = (binder_uintptr_t)reply.data;
    wr.txn.data_size        = reply.cursor;
    wr.txn.data.ptr.offsets = (binder_uintptr_t)reply.offsets;
    wr.txn.offsets_size     = reply.n_offsets * sizeof(binder_size_t);

    memcpy(mOut.data, &wr, sizeof(wr));
    mOut.cursor = sizeof(wr);
    talkWithDriver(true);
}

void IPCThreadState::joinThreadPool(bool isMain) {
    {
        uint32_t cmd = isMain ? BC_ENTER_LOOPER : BC_REGISTER_LOOPER;
        memcpy(mOut.data, &cmd, sizeof(cmd));
        mOut.cursor = sizeof(cmd);
        talkWithDriver(false);
    }
    while (1) {
        talkWithDriver(true);
        while (mIn.readPos() < mIn.cursor) {
            uint32_t cmd = mIn.readInt32();
            executeCommand(cmd);
        }
    }
}

// ===================== BpBinder =====================
int32_t BpBinder::transact(uint32_t code, const Parcel &data, Parcel *reply) {
    return IPCThreadState::self()->transact(mHandle, code, data, reply);
}

// ===================== BpServiceManager =====================
enum { CMD_REGISTER = 1, CMD_GET_SERVICE = 2 };

int32_t BpServiceManager::getService(const char *name) {
    Parcel data, reply;
    data.writeString(name);
    IPCThreadState::self()->transact(0, CMD_GET_SERVICE, data, &reply);
    if (reply.n_offsets > 0 && reply.cursor > 0) {
        struct flat_binder_object *fbo =
            (struct flat_binder_object*)(reply.data + *reply.offsets);
        return fbo->handle;
    }
    return 0;
}

void BpServiceManager::addService(const char *name, BBinder *service) {
    Parcel data;
    data.writeString(name);
    data.write_binder((binder_uintptr_t)service);
    IPCThreadState::self()->transact(0, CMD_REGISTER, data, nullptr);
}

// ===================== BnStringService =====================
int32_t BnStringService::onTransact(uint32_t code, const Parcel &data, Parcel *reply) {
    const char *input = (const char*)(data.data);
    char result[256] = {0};
    int len = data.cursor;
    if (len > 255) len = 255;
    memcpy(result, input, len);

    if (code == IStringService::UPPER) {
        for (int i = 0; result[i]; i++)
            if (result[i] >= 'a' && result[i] <= 'z') result[i] -= 32;
    } else if (code == IStringService::LOWER) {
        for (int i = 0; result[i]; i++)
            if (result[i] >= 'A' && result[i] <= 'Z') result[i] += 32;
    }

    reply->writeString(result);
    return 0;
}

// ===================== BpStringService =====================
const char* BpStringService::upper(const char *s) {
    static char buf[256];
    Parcel data, reply;
    data.writeString(s);
    transact(IStringService::UPPER, data, &reply);
    strncpy(buf, (const char*)reply.data, sizeof(buf)-1);
    return buf;
}

const char* BpStringService::lower(const char *s) {
    static char buf[256];
    Parcel data, reply;
    data.writeString(s);
    transact(IStringService::LOWER, data, &reply);
    strncpy(buf, (const char*)reply.data, sizeof(buf)-1);
    return buf;
}
