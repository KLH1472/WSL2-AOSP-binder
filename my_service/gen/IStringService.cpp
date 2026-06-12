// ═══════════════════════════════════════════════════════════════
// 此文件由 AIDL 编译器从 IStringService.aidl 自动生成，只读，勿改
//
// 包含全部实现:
//   IMPLEMENT_META_INTERFACE — DESCRIPTOR + getInterfaceDescriptor + asInterface
//   BpStringService 全部方法 — 编 Parcel → transact → 解 Parcel
//   BnStringService::onTransact — switch(code) 分发 + Parcel 编解码
//
// 这是 gen/ 目录下唯一的 .cpp 文件
// 编译进 libstringservice.so，Server 和 Client 都链接
// ═══════════════════════════════════════════════════════════════

#include "IStringService.h"
#include "BpStringService.h"
#include <binder/Parcel.h>

// IMPLEMENT_META_INTERFACE 展开为:
//   descriptor + getInterfaceDescriptor() + asInterface() + 构造/析构
//   asInterface() 内部 new BpStringService(obj)，因此需 BpStringService.h 可见
IMPLEMENT_META_INTERFACE(StringService, "com.example.binder.IStringService");

namespace com {
namespace example {
namespace binder {

// ═══════════════════════════════════════════════════════════════
// BpStringService — Client 代理方法实现
// ═══════════════════════════════════════════════════════════════

BpStringService::BpStringService(const ::android::sp<::android::IBinder>& impl)
    : ::android::BpInterface<IStringService>(impl) {
}

::android::String16 BpStringService::upper(const ::android::String16& input) {
    ::android::Parcel data;
    ::android::Parcel reply;

    // 1. 写入接口令牌（与 Bn::enforceInterface 成对）
    data.writeInterfaceToken(
            IStringService::getInterfaceDescriptor());
    // 2. 编码入参（与 Bn::readString16 严格对称）
    data.writeString16(input);
    // 3. transact → IPCThreadState → ioctl → kernel 投递到 Server
    remote()->transact(BnStringService::UPPER, data, &reply);
    // 4. 解码出参（与 Bn::writeString16 严格对称）
    ::android::String16 result;
    reply.readString16(&result);
    return result;
}

::android::String16 BpStringService::lower(const ::android::String16& input) {
    ::android::Parcel data;
    ::android::Parcel reply;

    data.writeInterfaceToken(
            IStringService::getInterfaceDescriptor());
    data.writeString16(input);
    remote()->transact(BnStringService::LOWER, data, &reply);

    ::android::String16 result;
    reply.readString16(&result);
    return result;
}

// ═══════════════════════════════════════════════════════════════
// BnStringService::onTransact — Server 端事务分发
//
// 与 BpStringService 的对称性（AIDL 编译器保证）:
//   Bp 写: writeInterfaceToken → writeString16(input) → transact
//   Bn 读: enforceInterface    → readString16(&input)
//   Bn 写: writeString16(result)
//   Bp 读: readString16(&result)
// ═══════════════════════════════════════════════════════════════

::android::status_t BnStringService::onTransact(
        uint32_t code, const ::android::Parcel& data,
        ::android::Parcel* reply, uint32_t /*flags*/) {

    switch (code) {

    case ::android::IBinder::INTERFACE_TRANSACTION: {
        reply->writeString16(
                IStringService::getInterfaceDescriptor());
        return ::android::NO_ERROR;
    }

    case UPPER: {
        data.enforceInterface(
                IStringService::getInterfaceDescriptor());

        ::android::String16 input;
        data.readString16(&input);

        ::android::String16 result = this->upper(input);

        reply->writeString16(result);
        return ::android::NO_ERROR;
    }

    case LOWER: {
        data.enforceInterface(
                IStringService::getInterfaceDescriptor());

        ::android::String16 input;
        data.readString16(&input);

        ::android::String16 result = this->lower(input);

        reply->writeString16(result);
        return ::android::NO_ERROR;
    }

    default:
        return ::android::BBinder::onTransact(code, data, reply, 0);
    }
}

}  // namespace binder
}  // namespace example
}  // namespace com
