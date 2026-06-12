// IStringService.aidl  —  你写的接口定义
// 一行不涉及 Binder 实现细节，只描述"有什么方法、什么参数、什么返回"
package com.example.binder;

interface IStringService {
    String upper(String input);       // 事务码自动 = IBinder::FIRST_CALL_TRANSACTION + 0  (1)
    String lower(String input);       // 事务码自动 = IBinder::FIRST_CALL_TRANSACTION + 1  (2)
}
