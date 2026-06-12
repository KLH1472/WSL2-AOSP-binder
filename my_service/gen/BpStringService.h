// ═══════════════════════════════════════════════════════════════
// 此文件由 AIDL 编译器从 IStringService.aidl 自动生成，只读，勿改
// ═══════════════════════════════════════════════════════════════
#pragma once

#include <binder/IInterface.h>
#include <utils/Errors.h>
#include "IStringService.h"

namespace com {
namespace example {
namespace binder {

class BpStringService : public ::android::BpInterface<IStringService> {
public:
    explicit BpStringService(const ::android::sp<::android::IBinder>& impl);
    virtual ~BpStringService() = default;

    ::android::String16 upper(const ::android::String16& input) override;
    ::android::String16 lower(const ::android::String16& input) override;
};

}  // namespace binder
}  // namespace example
}  // namespace com
