<!--  原文时间：2022.11.6,翻译时间：2024.5.6，校对时间：2024.7.12   -->

# HomeSpan 状态

可选的 ***homeSpan*** 方法`void setStatusCallback(void (*func)(HS_STATUS status))` 可用于创建一个回调函数 func，每当 HomeSpan 的状态发生变化时，它就会调用该函数。HomeSpan 向 func 传递一个参数 status,类型为 HS_STATUS，定义如下：

```C++
enum HS_STATUS {
  HS_WIFI_NEEDED,                         // 尚未设置/存储 WiFi 凭证been set/stored
  HS_WIFI_CONNECTING,                     // HomeSpan 正在尝试连接到存储的 WiFi 凭证中指定的网络
  HS_PAIRING_NEEDED,                      // HomeSpan 已连接到家庭 WiFi，但设备尚未与 HomeKit 配对
  HS_PAIRED,                              // HomeSpan 已连接到家庭 WiFi,并且该设备已与 HomeKit 配对
  HS_ENTERING_CONFIG_MODE,                // 用户已请求设备进入命令模式
  HS_CONFIG_MODE_EXIT,                    // HomeSpan 处于命令模式，并指定“退出命令模式”作为选择
  HS_CONFIG_MODE_REBOOT,                  // HomeSpan 处于命令模式，并指定“重新启动”作为选择
  HS_CONFIG_MODE_LAUNCH_AP,               // HomeSpan 处于命令模式，并指定“启动接入点”作为选择
  HS_CONFIG_MODE_UNPAIR,                  // HomeSpan 处于命令模式，并指定“取消配对设备”作为选项
  HS_CONFIG_MODE_ERASE_WIFI,              // HomeSpan 处于命令模式，并指定“清除 WiFi 凭证”作为选项
  HS_CONFIG_MODE_EXIT_SELECTED,           // 用户已选择“退出命令模式”
  HS_CONFIG_MODE_REBOOT_SELECTED,         // 用户已从命令模式中选择“重新启动”
  HS_CONFIG_MODE_LAUNCH_AP_SELECTED,      // 用户已从命令模式中选择“启动 AP 访问”
  HS_CONFIG_MODE_UNPAIR_SELECTED,         // 用户已从命令模式中选择“取消配对设备”
  HS_CONFIG_MODE_ERASE_WIFI_SELECTED,     // 用户已从命令模式中选择“清除 WiFi 凭证”
  HS_REBOOTING,                           // HomeSpan 正在重启设备
  HS_FACTORY_RESET,                       // HomeSpan 正在对设备进行恢复出厂设置
  HS_AP_STARTED,                          // HomeSpan 已启动接入点，但尚未有人连接
  HS_AP_CONNECTED,                        // 接入点已启动，且用户设备已连接
  HS_AP_TERMINATED,                       // HomeSpan 已终止接入点
  HS_OTA_STARTED                          // HomeSpan 正在接收无线软件更新

};
```

 ***homeSpan*** 方法 `char* statusString(HS_STATUS s)` 是一个方便的函数，用于将上面的任何枚举转换为简短的预定义字符串消息，如下所示：

```C++
const char* Span::statusString(HS_STATUS s){
  switch(s){
    case HS_WIFI_NEEDED: return("WiFi Credentials Needed");
    case HS_WIFI_CONNECTING: return("WiFi Connecting");
    case HS_PAIRING_NEEDED: return("Device not yet Paired");
    case HS_PAIRED: return("Device Paired");
    case HS_ENTERING_CONFIG_MODE: return("Entering Command Mode");
    case HS_CONFIG_MODE_EXIT: return("1. Exit Command Mode"); 
    case HS_CONFIG_MODE_REBOOT: return("2. Reboot Device");
    case HS_CONFIG_MODE_LAUNCH_AP: return("3. Launch Access Point");
    case HS_CONFIG_MODE_UNPAIR: return("4. Unpair Device");
    case HS_CONFIG_MODE_ERASE_WIFI: return("5. Erase WiFi Credentials");
    case HS_CONFIG_MODE_EXIT_SELECTED: return("Exiting Command Mode...");
    case HS_CONFIG_MODE_REBOOT_SELECTED: return("Rebooting Device...");
    case HS_CONFIG_MODE_LAUNCH_AP_SELECTED: return("Launching Access Point...");
    case HS_CONFIG_MODE_UNPAIR_SELECTED: return("Unpairing Device...");
    case HS_CONFIG_MODE_ERASE_WIFI_SELECTED: return("Erasing WiFi Credentials...");
    case HS_REBOOTING: return("REBOOTING!");
    case HS_FACTORY_RESET: return("Performing Factory Reset...");
    case HS_AP_STARTED: return("Access Point Started");
    case HS_AP_CONNECTED: return("Access Point Connected");
    case HS_AP_TERMINATED: return("Access Point Terminated");
    case HS_OTA_STARTED: return("OTA Update Started");
    default: return("Unknown");
  }
}
```

### 例如:

```C++
#include "HomeSpan.h"

void setup(){
  homeSpan.setStatusCallback(statusUpdate);   // 设置回调函数
  ...
  homeSpan.begin();
  ...
}

// 创建一个回调函数，每当 HomeSpan 状态发生变化时，该函数就会在串口监视器上打印预定义的短消息

void statusUpdate(HS_STATUS status){
  Serial.printf("\n*** HOMESPAN STATUS CHANGE: %s\n",homeSpan.statusString(status));
}
```

当然，你可以在 *func* 中创建任何替代消息，或者采取任何需要的操作，而不需要使用上面预定义的字符串。

---

[↩️](Reference.md) 返回 API 页面
