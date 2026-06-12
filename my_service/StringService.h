// ═══════════════════════════════════════════════════════════════
// StringService.h — 你写的，只写业务逻辑声明
// ═══════════════════════════════════════════════════════════════
#pragma once

#include "gen/BnStringService.h"

// 继承生成好的 BnStringService，只覆写纯虚业务方法
// 不需要关心 onTransact、Parcel、ioctl——AIDL 已全部生成
class StringService : public com::example::binder::BnStringService {
public:
    // 这两个方法就是 .aidl 里定义的业务方法
    ::android::String16 upper(const ::android::String16& input) override;
    ::android::String16 lower(const ::android::String16& input) override;
};
