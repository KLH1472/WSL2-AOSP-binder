// ═══════════════════════════════════════════════════════════════
// 此文件由 AIDL 编译器从 IStringService.aidl 自动生成，只读，勿改
// ═══════════════════════════════════════════════════════════════
#pragma once

#include <binder/IInterface.h>
#include "IStringService.h"

namespace com {
namespace example {
namespace binder {

class BnStringService : public ::android::BnInterface<IStringService> {
public:
    // 事务码 — 编译器按 .aidl 方法声明顺序自动分配
    static constexpr uint32_t UPPER = ::android::IBinder::FIRST_CALL_TRANSACTION + 0;
    static constexpr uint32_t LOWER = ::android::IBinder::FIRST_CALL_TRANSACTION + 1;

    ::android::status_t onTransact(
            uint32_t code,
            const ::android::Parcel& data,
            ::android::Parcel* reply,
            uint32_t flags = 0) override;
};

}  // namespace binder
}  // namespace example
}  // namespace com
