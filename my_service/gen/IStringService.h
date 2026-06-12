// ═══════════════════════════════════════════════════════════════
// 此文件由 AIDL 编译器从 IStringService.aidl 自动生成，只读，勿改
// ═══════════════════════════════════════════════════════════════
#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/String16.h>

namespace com {
namespace example {
namespace binder {

class IStringService : public ::android::IInterface {
public:
    DECLARE_META_INTERFACE(StringService);

    virtual ::android::String16 upper(const ::android::String16& input) = 0;
    virtual ::android::String16 lower(const ::android::String16& input) = 0;
};

}  // namespace binder
}  // namespace example
}  // namespace com
