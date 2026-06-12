// ═══════════════════════════════════════════════════════════════
// StringService.cpp — 你写的，纯业务逻辑，无 Binder 痕迹
// ═══════════════════════════════════════════════════════════════

#include "StringService.h"

// 只实现 .aidl 里定义的方法，不需要知道 Parcel/ioctl/BBinder 的存在
::android::String16 StringService::upper(const ::android::String16& input) {
    // 纯业务逻辑: 转大写
    // 在真实 AOSP 中, String16 本质是 char16_t 数组
    size_t len = input.size();
    ::android::String16 out(input);
    char16_t* buf = out.lockBuffer(len);
    for (size_t i = 0; i < len; i++) {
        if (buf[i] >= 'a' && buf[i] <= 'z')
            buf[i] -= 32;
    }
    out.unlockBuffer(len);
    return out;
}

::android::String16 StringService::lower(const ::android::String16& input) {
    size_t len = input.size();
    ::android::String16 out(input);
    char16_t* buf = out.lockBuffer(len);
    for (size_t i = 0; i < len; i++) {
        if (buf[i] >= 'A' && buf[i] <= 'Z')
            buf[i] += 32;
    }
    out.unlockBuffer(len);
    return out;
}
