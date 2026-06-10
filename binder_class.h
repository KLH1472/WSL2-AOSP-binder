#pragma once
#include <cstdint>
#include "binder_buf.h"

typedef struct __attribute__((packed)) {
    uint32_t cmd;
    struct binder_transaction_data txn;
} bc_txn_t;

// ---------------------------------------------------------------------------
// Parcel — minimal serialisation (extends BinderBuf with read support)
// ---------------------------------------------------------------------------
class Parcel : public BinderBuf {
    size_t mReadPos;
public:
    Parcel() : mReadPos(0) {}

    // wrap driver buffer for zero-copy read (used in executeCommand)
    void setData(const BinderBuf &src) {
        memcpy(data, src.data, src.cursor);
        cursor    = src.cursor;
        n_offsets = src.n_offsets;
        memcpy(offsets, src.offsets, n_offsets * sizeof(uint64_t));
        mReadPos  = 0;
    }

    size_t readPos() const { return mReadPos; }
    void   setReadPos(size_t p) { mReadPos = p; }

    int32_t readInt32() {
        int32_t v; memcpy(&v, data + mReadPos, 4); mReadPos += 4; return v;
    }

    void writeInt32(int32_t v) { memcpy(data + cursor, &v, 4); cursor += 4; }

    void writeString(const char *s) {
        int len = strlen(s) + 1;
        memcpy(data + cursor, s, len); cursor += len;
    }

    void writeString16(const char *s) { writeString(s); }  // simplified: no UTF-16

    const char *readString() {
        const char *s = (const char*)(data + mReadPos);
        mReadPos += strlen(s) + 1;
        return s;
    }
};

// ---------------------------------------------------------------------------
// IBinder — abstract base
// ---------------------------------------------------------------------------
class BBinder; class BpBinder;
class IBinder {
public:
    virtual BBinder* localBinder()  { return nullptr; }
    virtual int32_t   handle()      { return 0; }
};

// ---------------------------------------------------------------------------
// BBinder — server-side object (cookie in flat_binder_object)
// ---------------------------------------------------------------------------
class BBinder : public IBinder {
public:
    BBinder* localBinder() override { return this; }

    virtual int32_t onTransact(uint32_t code, const Parcel &data, Parcel *reply) {
        (void)code; (void)data; (void)reply; return 0;
    }
};

// ---------------------------------------------------------------------------
// BpBinder — client-side proxy (holds a handle)
// ---------------------------------------------------------------------------
class BpBinder : public IBinder {
public:
    int32_t mHandle;
    BpBinder(int32_t h) : mHandle(h) {}
    int32_t handle() override { return mHandle; }

    int32_t transact(uint32_t code, const Parcel &data, Parcel *reply);
};

// ---------------------------------------------------------------------------
// ProcessState — opens driver, mmap (per-process singleton)
// ---------------------------------------------------------------------------
class ProcessState {
    int  mFD;
    void *mVM;
    ProcessState();
public:
    static ProcessState* self();
    int  fd()     const { return mFD; }
    void* vbase() const { return mVM; }
};

// ---------------------------------------------------------------------------
// IPCThreadState — ioctl loop, command dispatch
// ---------------------------------------------------------------------------
class IPCThreadState {
    Parcel  mIn, mOut;
    BBinder *mContextObj;

    void talkWithDriver(bool doReceive);
    void executeCommand(uint32_t cmd);
    void sendReply(Parcel &reply);
    int  waitForResponse(Parcel *reply);

public:
    static IPCThreadState* self();
    void setContextObject(BBinder *obj) { mContextObj = obj; }

    int32_t transact(int32_t handle, uint32_t code,
                     const Parcel &data, Parcel *reply);
    void    joinThreadPool(bool isMain);
};

// ---------------------------------------------------------------------------
// BpServiceManager — proxy for handle 0 (getService / addService)
// ---------------------------------------------------------------------------
class BpServiceManager {
public:
    int32_t getService(const char *name);
    void    addService(const char *name, BBinder *service);
};

// ---------------------------------------------------------------------------
// Our demo interface: IStringService → upper / lower
// ---------------------------------------------------------------------------
class IStringService {
public:
    static const uint32_t UPPER = 100;
    static const uint32_t LOWER = 101;
};

class BnStringService : public BBinder {
public:
    int32_t onTransact(uint32_t code, const Parcel &data, Parcel *reply) override;
};

class BpStringService : public BpBinder {
public:
    BpStringService(int32_t handle) : BpBinder(handle) {}
    const char* upper(const char *s);
    const char* lower(const char *s);
};
