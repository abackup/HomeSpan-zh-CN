<!-- 原文时间：2024.7.1，翻译时间：2024.7.6，校对时间：2024.7.12  -->

# HomeSpan API 参考

HomeSpan 库通过在 Arduino 草图中包含 *HomeSpan.h* 来调用，如下所示：

```C++
#include "HomeSpan.h"
```

## *homeSpan*

在运行时，HomeSpan 将创建一个名为 `homeSpan` 的全局**对象**（类型为 *Span*），该对象支持以下方法：

* `void begin(Category catID, const char *displayName, const char *hostNameBase, const char *modelName)`
  * 初始化 HomeSpan
  * **必须**在每个草图的开头调用，然后再调用任何其他 HomeSpan 函数，并且通常放置在 Arduino `setup()` 方法的顶部附近，但在 `Serial.begin()` **之后**，以便初始化诊断可以输出到串口监视器
  * 所有参数都是**可选**
     * *catID* - HAP 类别 HomeSpan 广播用于与 HomeKit 配对。默认为 Category::Lighting。有关完整列表，请参阅 [HomeSpan 配件件类别](Categories.md)
     * *displayName* - HomeSpan 广播的 MDNS 显示名称。默认为 "HomeSpan Server"
     * *hostNameBase* - 完整的 MDNS 主机名由 HomeSpan 作为 *hostNameBase-DeviceID*.local 广播，其中 DeviceID 是 HomeSpan 自动生成的唯一的 6 字节代码。默认为 "HomeSpan"
     * *modelName* - 与 HomeKit 配对的 HomeSpan 广播 HAP 模型名称。默认为 "HomeSpan-ESP32"
  * 例：`homeSpan.begin(Category::Fans, "Living Room Ceiling Fan");`
* `void poll()`
  * 检查 HAP 请求、本地命令和设备活动
  * **必须**在每个草图中重复调用，并且通常放在 Arduino `loop()` 方法的顶部（*除非*使用下面进一步描述的 `autoPoll()`）

---

以下**可选** `homeSpan` 方法会覆盖 `begin()` 中使用的各种 HomeSpan 初始化参数，因此**应该**在 `begin()` 之前调用才能生效。返回类型为 `Span&`的方法返回对 `homeSpan` 本身的引用，因此可以链接在一起（例如 `homeSpan.setControlPin(21).setStatusPin(13);`）。如果未调用某个方法，HomeSpan 将使用如下所示的默认参数：

* `Span& setControlPin(uint8_t pin, triggerType=PushButton::TRIGGER_ON_LOW)`
  * 设置用于 HomeSpan 控制按钮的 ESP32 *pin*
  * 如果未调用此方法，HomeSpan 将假定没有控制按钮
  * 可选的第二个参数 *triggerType* 将按如下方式配置控制按钮：
    * `PushButton::TRIGGER_ON_LOW` - 当 *pin* 驱动为低电平时触发
      * 适用于将 *pin* 连接到地面的按钮（当未指定 *triggerType* 时，这是默认值）
    * `PushButton::TRIGGER_ON_HIGH` - 当 *pin* 驱动为高电平时触发
      * 适用于将 *pin* 连接到 VCC（通常为 3.3V）的按钮
    * `PushButton::TRIGGER_ON_TOUCH` - 使用设备的触摸传感器外设在触摸 *pin* 时触发
      * ESP32-C3 上不可用
  * 或者，你可以将 *triggerType* 设置为任何用户定义的 `boolean(int arg)` 形式的函数，以将任何设备用作控制按钮。有关详细信息，请参阅下面的 **[SpanButton](#spanbutton)**

* `int getControlPin()`
  * 返回由 `setControlPin(pin)` 设置的 HomeSpan 控制按钮的引脚号，如果未设置引脚，则返回 -1

* `Span& setStatusPin(uint8_t pin)`
  * 设置用于 HomeSpan 状态指示灯的 ESP32 *pin*
  * 假设标准 LED 将连接到 *pin*
  * 如果未调用此方法或任何等效方法，HomeSpan 将假定没有状态指示灯

* `Span& setStatusPixel(uint8_t pin, float h=0, float s=100, float v=100)`
  * 设置用于 HomeSpan 状态指示灯的 ESP32 *pin*
  * 此方法是使用上述 `setStatusPin()` 的*替代方法*
  * 假设 RGB NeoPixel（或等效物）将连接到 *pin*
  * 适用于 ESP32 板有内置 NeoPixel LED，但添加外部 NeoPixel 也可以
  * 用户可以通过提供以下 HSV 值来选择性地指定 HomeSpan 将与 NeoPixel 一起使用的颜色：
    * h = 色调，范围从 0-360
    * s = 饱和度百分比，范围从 0-100
    * v = 亮度百分比，范围从 0-100
  * 如果未指定，颜色默认为*红色*
  * 示例：`homeSpan.setStatusPixel(8,120,100,20)` 使用连接到引脚 8 的 NeoPixel 将状态指示灯设置为浅绿色
  * 如果未调用此方法或任何等效方法，HomeSpan 将假定没有状态指示灯

* `Span& setStatusDevice(Blinkable *sDev)`
  * 将状态指示灯设置为用户指定的可闪烁设备 *sDev*
  * 此方法是使用上述 `setStatusPin()` 或 `setStatusPixel()` 的*替代方法*
  * 参见 [Blinkable](Blinkable.md) 详细了解如何创建通用的 Blinkable 设备
  * 在使用连接到引脚扩展器或其他专用驱动器的 LED 作为状态指示灯时很有用
  * 如果未调用此方法或任何等效方法，HomeSpan 将假定没有状态指示灯

* `Span& setStatusAutoOff(uint16_t duration)`
  * 将状态指示灯设置为在 *duration* 秒后自动关闭
  * 每当 HomeSpan 激活新的闪烁模式时，状态指示灯将自动打开，并且持续时间计时器将重置
  * 如果 *duration* 设置为零，则自动关闭将被禁用（状态指示灯将无限期保持打开状态）

* `int getStatusPin()`
  * 返回由 `setStatusPin(pin)` 设置的状态指示灯的引脚号，如果未设置引脚，则返回 -1

* `Span& setApSSID(const char *ssid)`
  * 设置 HomeSpan 设置接入点的 SSID（网络名称）（默认为 "HomeSpan-Setup"）

* `Span& setApPassword(const char *pwd)`
  * 设置 HomeSpan 设置接入点的密码（默认为 "homespan"）

* `Span& setApTimeout(uint16_t nSec)`
  * 设置 HomeSpan 设置接入点激活后在超时前保持活动的持续时间（以秒为单位）（默认值为 300 秒）

* `Span& setCommandTimeout(uint16_t nSec)`
  * 设置 HomeSpan 最终用户命令模式激活后在超时前保持活动的持续时间（以秒为单位）（默认值为 120 秒）

* `Span& setLogLevel(int level)`
  * 设置诊断消息的日志记录级别，其中：
    * 0 = 顶级 HomeSpan 状态消息，以及用户在草图中指定的任何 `LOG0()` 消息（默认值）
    * 1 = 所有 HomeSpan 状态消息，以及用户在草图中指定的任何 `LOG1()` 消息
    * 2 = 所有 HomeSpan 状态消息以及往返于 HomeSpan 设备的所有 HAP 通信数据包，以及用户在草图中指定的所有 `LOG1()` 和 `LOG2()` 消息
    * -1 = 抑制所有消息HomeSpan 状态消息，包括用户在草图中指定的所有 `LOG0()`、`LOG1()` 和 `LOG2()` 消息，释放串口以用于其他用途
  * 日志级别设置对草图中可能使用的任何 `Serial.print()` 或 `Serial.printf()` 语句没有影响。如果你想通过设置 HomeSpan 日志级别来控制输出，请使用其中一个 `LOG()` 宏，而不是 `Serial.print()` 或 `Serial.printf()`
  * 日志级别设置对 ESP32 操作系统本身输出的任何 ESP32 诊断消息没有影响。要隐藏这些消息，请确保在编译草图时将 Arduino IDE 的工具菜单中的*核心调试级别*设置为“无”
  * 注意，也可以在运行时通过 [命令行界面](CLI.md) 使用 "L" 命令更改日志级别
  * 有关完整详细信息，请参阅 [消息日志](Logging.md)

* `int getLogLevel()`
  * 返回由 `setLogLevel(level)` 设置的当前日志级别

* `Span& setPortNum(uint16_t port)`
  * 设置用于 HomeKit 和 HomeSpan 之间通信的 TCP 端口号（默认值为 80）

* `Span& setHostNameSuffix(const char *suffix)`
  * 设置 HomeSpan 附加到 *hostNameBase* 的后缀以创建完整的 hostName
  * 如果未指定，则默认为 HomeSpan 在设备的 6 字节附件 ID 后附加一个破折号 "-"
  * 将 *suffix* 设置为允许使用空字符串 ""
  * 示例：`homeSpan.begin(Category::Fans, "Living Room Ceiling Fan", "LivingRoomFan");` 将产生一个默认的 *hostName*，形式为 *LivingRoomFan-A1B2C3D4E5F6.local*。在 `homeSpan.begin()` 之前调用 `homeSpan.setHostNameSuffix("v2")` 将产生 *LivingRoomFanv2.local* 的 *hostName*

* `Span& setQRID(const char *id)`
  * 将用于将设备与 [二维码](QRCodes.md) 配对的设置 ID 从 HomeSpan 默认值更改为 *id*
  * 除非通过 [命令行界面](CLI.md) 使用 "Q" 命令永久更改设备，否则 HomeSpan 默认值为 "HSPN"
  * *id* 必须正好是 4 个字母数字字符（0-9、A-Z 和 a-z）。如果不是，更改设置 ID 的请求将被默默忽略，并使用默认值

---

以下**可选** `homeSpan` 方法启用其他功能并提供对 HomeSpan 环境的进一步自定义。除非另有说明，否则**应**在 `begin()` 之前进行调用才能生效：

* `int enableOTA(boolean auth=true, boolean safeLoad=true)`
   * 启用 HomeSpan 设备的 [无线 (OTA) 更新](OTA.md)，否则该功能将被禁用
   * HomeSpan OTA 需要授权密码，除非指定 *auth* 并将其设置为 *false*
   * 新 HomeSpan 设备的默认 OTA 密码是 "homespan-ota" 
   * 这可以通过 [命令行界面](CLI.md) 使用 "O" 命令进行更改
   * 注意启用 OTA 会使 HAP 套接字连接数减少 1
   * 除非指定第二个参数并将其设置为 *false*，否则默认情况下将启用 OTA 安全加载。HomeSpan OTA 安全加载检查以确保上传到现有 HomeSpan 设备的草图本身就是 HomeSpan 草图，并且它们也启用了 OTA。详情请参见 [HomeSpan OTA 安全加载](OTA.md#ota-安全加载)
   * 如果启用 OTA 成功，则返回 0，否则返回 -1；并向串口监视器报告错误

* `int enableOTA(const char *pwd, boolean safeLoad=true)`
   * `enableOTA()` 的替代形式，允许你以编程方式将 OTA 密码更改为指定的 *pwd*
   * *pwd* 必须包含 1 至 32 个字符
   * 此命令使 HomeSpan 忽略但不更改使用 "O" 命令存储的任何密码
   * 如果启用 OTA 成功，则返回 0，否则返回 -1；并向串口监视器报告错误

* `Span& enableAutoStartAP()`
   * 如果在启动时**未**找到 WiFi 凭证，则启用 WiFi 接入点的自动启动
   * 改变 HomeSpan 接入点行为的方法，例如 `setApTimeout()`，必须在 `enableAutoStartAP()` 之前调用才能生效

* `Span& setApFunction(void (*func)())`
   * 调用用户定义的函数 *func* 取代 HomeSpan 的内置 WiFi 接入点
   * *func* 必须是 *void* 类型并且没有参数
   * *func* 将在每次启动接入点时调用，而不是 HomeSpan 的内置 WiFi 接入点：
     * 通过 CLI 输入 "A"，或者
     * 通过使用命令模式选项 3 的控制按钮，或
     * 如果设置了 `enableAutoStartAP()` 并且没有存储 WiFi 凭证，则启动时自动启动
   * 识别所需网络的 SSID 和密码后，*func* 必须调用 `setWifiCredentials()` 来保存并使用这些值
   * 建议 *func* 通过使用 `ESP.restart()` 重新启动设备来终止。重新启动后，HomeSpan 将使用刚刚保存的 SSID 和密码

* `Span& setWifiCredentials(const char *ssid, const char *pwd)`
  * 设置 HomeSpan 将连接的 WiFi 网络的 SSID (*ssid*) 和密码 (*pwd*)
  * *ssid* 和 *pwd* 会自动保存在 HomeSpan 的非易失性存储器 (NVS) 中，以便在设备重启时检索
  * 请注意，如果保存的值超过允许的最大字符数 (ssid=32; pwd=64)，则会被截断
  * :warning: 安全警告：此函数的目的是允许高级用户使用由 `setApFunction(func)` 指定的自定义接入点函数*动态*设置设备的 WiFi 凭据。不建议使用此函数将你的 WiFi SSID 和密码直接硬编码到你的草图中。相反，使用 HomeSpan 提供的更安全的方法之一，例如从 CLI 输入 "W"，或启动 HomeSpan 的接入点，来设置你的 WiFi 凭据，而无需将它们硬编码到你的草图中

* `Span& setConnectionTimes(uint32_t minTime, uint32_t maxTime, uint8_t nSteps)`
  * overrides HomeSpan's default repeating pattern of increasing wait times when trying to connect to a WiFi network, where
    * *minTime* - the minimum time (in seconds) that HomeSpan initially waits when first trying to connect to a WiFi network
    * *maxTime* - the maximum time (in seconds) that HomeSpan will wait on subequent attempts to connect if first attempt fails
    * *nSteps* - the number of steps HomeSpan uses to set the increasing intermediate times between *minTime* and *maxTime* as each re-attempt to connect is made
  * example: `homeSpan.setConnectionTimes(5,60,3);` causes HomeSpan to initially wait 5 seconds when first attempting to connect to a WiFi network, and then wait 11, 26, and finally 60 seconds on the next 3 subsequent attempts. If HomeSpan has still not connected, the pattern repeats indefinitely
  * if either *minTime* or *nSteps* is set to zero, or if *maxTime* is not strictly greater than *minTime*, HomeSpan ignores the request and reports a warning message to the Serial Monitor
  * note this is an optional method. If not called HomeSpan uses default parameters of {5,60,5} which yields a wait pattern of 5, 8, 14, 22, 36, and 60 seconds between connection attempts

* `Span& setWifiBegin(void (*func)(const char *ssid, const char *pwd))`
  * sets an **alternative** user-defined function, *func*, to be called by HomeSpan when it tries to connect to a WiFi network with specified credentials SSID=*ssid* and password=*pwd*, **instead** of HomeSpan's default behavior of simply calling `WiFi.begin(ssid, pwd)`
  * this ability to define an alternative *func* is provided for users that either need to use a different type of call to establish WiFi connectivity (e.g. connectivity to an enterprise network), or that require additional functionality to be called when connectivity is established (e.g. changing the WiFi power)
  * note *func* is called every time HomeSpan tries to connect to WiFi network, including during repeated wait periods as well as reconnects after a disconnect
  * the function *func* must be of type *void* and accept two argument into which HomeSpan will pass whatever SSID and Password you previously saved as HomeSpan's WiFi Credentials, or that you explicitly set in the sketch using `setWifiCredentials()` above
    * *func* does not necessarily need to use this information, but it must must be able to accept the data
  * example: `homeSpan.setWifiBegin(myWifi)` where *myWifi* is defined below would address the issue on some ESP32 boards that will not connect to a WiFi network unless the WiFi radio power is changed to a lower value immediately after calling `WiFi.begin()`:

```C++
 void myWifi(const char *ssid, const char *pwd){
   WiFi.begin(ssid,pwd);                 // don't forget to call WiFi.begin(), if still needed, in your alternative function
   WiFi.setTxPower(WIFI_POWER_8_5dBm);   // set power immediately after as required for some ESP32 boards
}
```

* `Span& enableWiFiRescan(uint32_t iTime=1, uint32_t pTime=0, int thresh=3)`
  * when you configure HomeSpan to connect to a WiFi SSID that broadcasts from more than one access point (e.g. a mesh network), HomeSpan connects to the access point with the strongest RSSI signal for that SSID
  * once connected, HomeSpan remains "attached" to that specific access point unless it looses overall connectivity to the network, or is otherwise purposely disconnected, at at which point it will attempt to reconnect once again the strongest access point broadcasting the original SSID
  * calling `enableWiFiRescan()` enables HomeSpan to further optimize WiFi connectivity in the background during normal operation by periodically rescanning all access points with the original SSID, and automatically performing a disconnect/reconnect if it finds an access point with a stronger RSSI signal, where
    * *iTime* - the time HomeSpan waits (in minutes) between first making a connection to an access point and rescanning to check for other stronger access points
    * *pTime* -  the time HomeSpan waits after its first scan before performing any subsequent rescans
    * *thresh* -  the threshhold difference (in RSSI units) by which the signal strength of a newly-scanned access point must be stronger than that of the access point to which HomeSpan is current connected for HomeSpan to trigger a connect/disconnect.  This prevents the HomeSpan from switching back and forth between two access points with very similar RSSI strengths
  * example: `homeSpan.enableWiFiRescan(2, 5, 3)` causes HomeSpan to rescan all access points 2 minutes after initially connecting, and then every 5 minutes thereafter; if after any rescan HomeSpan finds a different access point with an RSSI signal that is 3 or more units stronger than the currently-connect access point, it will disconnect from the existing access point and connect to the stronger one
  * the default is for HomeSpan to NOT perform any background rescans unless you specifically enable this by calling `homeSpan.enableWiFiRescan()` from your sketch
    * if *iTime* is set to zero, rescanning is disabled
    * if *iTime* is greater than zero but *pTime* is set to zero, rescanning occurs only once at *iTime* minutes after connecting, but not thereafter

* `Span& addBssidName(String bssid, string name)`
  * creates a friendly display name for any given WiFi access point that HomeSpan can display (in both the Serial Monitor and the Web Log) alongside the BSSID whenever needed, where
    * *bssid* - the 6-byte BSSID MAC address of an access point, in the form "XX:XX:XX:XX:XX:XX"
    * *name* - a friendly display name for the access point
  * most useful when HomeSpan is connected to mesh WiFi network containing multiple access points sharing the same SSID, and you want to keep track of which access point is being used withouth having to memorize the BSSIDs of each access point
  * example: `homeSpan.addBssidName("3A:98:B5:EF:BF:69","Kitchen").addBssidName("3A:98:B5:DB:54:86","Basement");` creates display names "Kitchen" and "Basement" for access points with BSSIDs "3A:98:B5:EF:BF:69" and "3A:98:B5:DB:54:86", respectively
 
* `Span& setVerboseWifiReconnect(bool verbose)`
  * when trying connecting to WiFi, HomeSpan normally logs "Trying to connect to..." messages to the Serial Monitor and the Web Log
  * calling this method with *verbose* set to *false* supresses these messages
  * calling this method a second time with *verbose* set to *true* re-activates these messages (default behavior)

* `Span& setConnectionCallback(void (*func)(int count))`
  * sets an optional user-defined callback function, *func*, to be called by HomeSpan every time WiFi or Ethernet connectivity has been established or re-established after a disconnect.  The function *func* must be of type *void* and accept a single *int* argument, *count*, into which HomeSpan passes the number of times WiFi or Ethernet connectivity has been established or re-established (i.e. *count*=1 on initial WiFi or Ethernet connection; *count*=2 if re-established after the first disconnect, etc.)


* `Span& setVerboseWifiReconnect(bool verbose)`
  * 尝试连接到 WiFi 时，HomeSpan 通常会将“尝试连接到...”消息记录到串口监视器和网络日志
  * 调用此方法并将 *verbose* 设置为 *false* 会抑制这些消息
  * 第二次调用此方法并将 *verbose* 设置为 *true* 会重新激活这些消息（默认行为）


* `Span& setPairCallback(void (*func)(boolean status))`
  * 设置可选的用户定义回调函数 *func*，在完成与控制器的配对（*status=true*）或与控制器的配对（*status=false*）后由 HomeSpan 调用
  * 此一次性调用 *func* 是为希望在设备首次配对或设备稍后取消配对时触发其他操作的用户提供的
  * 请注意，此 *func* **不会**在启动时调用，不应仅用于检查设备是否配对或取消配对。仅在配对状态改变时调用它
  * 函数 *func* 必须是 *void* 类型并接受一个 *boolean* 参数

* `Span& setControllerCallback(void (*func)())`

  * 设置可选的用户定义回调函数 func，每次添加、删除或更新新控制器时，即使配对状态没有更改，HomeSpan 也会调用该函数
  * 注意此方法与 `setPairCallback()` 不同，仅当设备的配对状态发生变化时，例如在初始配对时添加第一个控制器，或在取消配对时删除最后一个控制器时，才会调用该方法 
  * 函数 func 的类型必须为 void 并且没有参数
  * 有关如何读取每个配对控制器的配对数据的详细信息，请参阅 （仅支持某些高级用例 `controllerListBegin()` `controllerListEnd())` 时需要） 

* `Span& setStatusCallback(void (*func)(HS_STATUS status))`
  * 设置一个可选的用户定义回调函数 *func*，每当 HomeSpan 的运行状态（例如 WiFi 连接、需要配对...）发生变化时，该函数将被调用，从而改变（可选）状态指示灯的闪烁模式
  * 如果设置了 *func*，则无论是否实际定义了状态指示灯，都会调用它
  * 这允许用户使用替代方法反映 HomeSpan 当前状态的变化，例如将消息输出到嵌入式 LCD 或 E-Ink 显示器
  * 函数 *func* 必须是 *void* 类型并接受一个枚举类型 [HS_STATUS](HS_STATUS.md) 的参数

* `char* statusString(HS_STATUS s)`
  * 返回一个预定义的字符串消息，表示*s*，必须是枚举类型 [HS_STATUS](HS_STATUS.md)
  * 通常与上面的 `setStatusCallback()` 一起使用

* `Span& setPairingCode(const char *s)`
  * 将设置配对代码设置为 *s*，**必须**正好是 8 位数字（无破折号）
  * 示例：`homeSpan.setPairingCode("46637726");`
  * 配对代码的哈希版本将保存到设备的非易失性存储器中，覆盖任何当前存储的配对代码
  * 如果 *s* 包含无效代码，则会报告错误，并且不会保存代码。相反，将使用当前存储的配对代码（如果未存储任何代码，则使用 HomeSpan 默认配对代码）
  * :warning: 安全警告：将设备的配对代码硬编码到你的草图中被视为安全风险，不建议这样做。相反，请使用 HomeSpan 提供的更安全的方法之一来设置你的配对代码，例如从 CLI 输入 "S \<code\>"，或启动 HomeSpan 的接入点，而无需将其硬编码到你的草图中

* `Span& setSketchVersion(const char *sVer)`
  * 将 HomeSpan 草图的版本设置为 *sVer*，可以是任意字符串
  * 如果未指定，HomeSpan 将使用 "n/a" 作为默认版本文本
  * HomeSpan 在启动时在 Arduino IDE 串口监视器中显示草图的版本
  * HomeSpan 还包括草图的版本以及用于编译草图的 HomeSpan 库的版本，作为其 HAP MDNS 广播的一部分。HAP *不*使用此数据。相反，它仅用于提供信息，并允许你识别通过 [OTA](OTA.md) 更新的设备的草图版本，而不是连接到计算机

* `const char *getSketchVersion()`
  * 返回 HomeSpan 草图的版本，使用 `void setSketchVersion(const char *sVer)` 设置，如果未设置则返回 "n/a"
  * 可以从草图中的任何位置调用

* `Span& enableWebLog(uint16_t maxEntries, const char *timeServerURL, const char *timeZone, const char *logURL)`
  * 启用滚动网络日志，显示用户使用 `WEBLOG()` 宏创建的最近 *maxEntries* 条目。参数及其默认值（如果未指定）如下：
    * *maxEntries* - 要保存的最大（最近）条目数。如果未指定，则默认为 0，在这种情况下，Web 日志将仅显示状态而没有任何日志条目
    * *timeServerURL* - 时间服务器的 URL，HomeSpan 在建立 WiFi 连接后将使用该时间服务器在启动时设置其时钟。指定时间服务器后，HomeSpan 将保留一个额外的套接字连接。如果未指定，则默认为空，在这种情况下，HomeSpan 将跳过设置设备时钟
    * *timeZone* - 指定用于设置时钟的时区。仅使用 POSIX.1 格式，不支持 *Time Zone Database* 或 *tzdata*。根据 [TZ 的 GNU libc 文档](https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html)，*偏移量指定你必须**添加到本地时间**才能获得协调世界时值的时间值*。"UTC+5:00" 表示本地时间 + 5 小时为 UTC 时间。请参阅 GNU libc 文档以获取一些示例，包括如何指定北美东部标准时间 (EST) 和东部夏令时间 (EDT)，开始日期和结束日期为 EDT。如果 *serverURL=NULL*，则忽略此字段；如果 *serverURL!=NULL*，则必须填写此字段
    * *logURL* - 此设备的网络日志页面的 URL。如果未指定，则默认为 "status" 。如果 *logURL* 设置为空，HomeSpan 将使用 *timeServerURL* 和 *timeZone* 参数来设置时钟，但它不会响应任何 HTTP 请求来提供任何网络日志页面。但是，Web 日志数据仍在内部累积，并且可以通过调用 `homeSpan.getWebLog()` 方法随时访问生成的 HTML（见下文）
  * 示例：`homeSpan.enableWebLog(50,"pool.ntp.org","UTC-1:00","myLog");` 在 URL *http<nolink>://HomeSpan-\[DEVICE-ID\].local:\[TCP-PORT\]/myLog* 处创建一个网络日志，该日志将显示使用 WEBLOG() 宏生成的 50 条最新日志消息。启动时（建立 WiFi 连接后），HomeSpan 将尝试通过调用服务器 "pool.ntp.org" 并将时间调整为比 UTC 早 1 小时来设置设备时钟。
  * 尝试连接到 *timeServerURL* 时，HomeSpan 会等待 120 秒以获得响应。这是在后台完成的，在 HomeSpan 尝试设置时间时不会阻止其照常运行。如果 120 秒超时期限后仍未收到任何响应，HomeSpan 将假定服务器无法访问并跳过时钟设置程序。使用 `setTimeServerTimeout()` 将 120 秒超时时间重新配置为其他值
  * 有关完整详细信息，请参阅 [消息日志](Logging.md)

* `Span& setTimeServerTimeout(uint32_t tSec)`
  * 将 HomeSpan 在 `enableWebLog()` 尝试将设备时钟从互联网时间服务器设置为 *tSec* 秒时使用的默认 120 秒超时时间更改为

* `Span& setWebLogCSS(const char *css)`
  * 将 HomeSpan网络日志的格式设置为 *css* 指定的自定义样式表
  * 有关如何构造 *css* 的详细信息，请参阅 [消息日志](Logging.md)

* `Span& setWebLogCallback(void (*func)(String &htmlText))`
  * 设置可选的用户定义回调函数 *func*，每当生成网络日志时 HomeSpan 都会调用该函数
  * 允许用户向通过**扩展**字符串 *htmlText* 来构造网络日志的初始表，该字符串作为引用传递给 *func*
  * 函数 *func* 必须是 *void* 类型，并接受一个 *String* 类型的参数
  * 有关如何构造 *htmlText* 的详细信息，请参阅 [消息日志](Logging.md)

* `void getWebLog(void (*f)(const char *htmlBuf, void *args), void *userData)`
  * 调用时，HomeSpan *流式传输*当前网络日志 HTML 文本以及任何可选指定的 *userData*，直接发送到用户定义的函数 *f()*，该函数应返回一个 *void* 并接受以下两个参数：
    * *htmlBuf* - 指向网络日志页面 HTML 文本部分的指针
    * *args* - *userData* 参数的传递
  * 如果不需要用户定义的数据，请将 *userData* 设置为空
  * 为避免创建单个大型文本缓冲区，HomeSpan 将网络日志的 HTML 拆分为 1024 字节的块并重复调用 *f()* 直到所有 HTML 都已流式传输；然后 HomeSpan 最后调用 *f()*，并将 *htmlBuf* 设置为空，向用户指示已到达 HTML 文本的末尾
  * 此命令主要用于将网络日志页面重定向到用户定义的进程，以进行其他处理、显示或传输
  * 有关更多详细信息，请参阅 [消息日志](Logging.md)
* `void processSerialCommand(const char *CLIcommand)`
  * 处理 *CLIcommand*，就像输入到串口监视器中一样
  * 允许以编程方式访问所有 CLI 命令，包括用户定义的任何自定义命令
  * 无​​论设备是否连接到计算机，都可以工作
  * 示例：`homeSpan.processSerialCommand("A");` 启动 HomeSpan 设置接入点
  * 示例：`homeSpan.processSerialCommand("Q HUB3");` 将 二维码的 HomeKit 设置 ID 更改为 "HUB3"

* `Span& setRebootCallback(void (*func)(uint8_t count), uint32_t upTime)`
  * 设置可选的用户定义回调函数 *func*，在重启后经过 *upTime* 毫秒时调用（仅一次）（如果未指定，则默认 *upTime*=5000 毫秒）
  * 函数 *func* 必须是输入 *void* 并接受一个 *uint8_t* 类型的参数
  * HomeSpan 传递给 *func* 的参数 *count* 表示在当前重启之前发生的“短”重启次数，其中“短”重启是指在 *upTime* 毫秒过去之前发生的任何重启
  * 这允许用户通过快速打开/关闭设备电源指定次数来向草图提供通用形式的输入，通常用于提供重置远程设备某些方面的方法
  * 使用 lamba 函数的示例：
    * `homeSpan.setRebootCallback( [](uint8_t c) {if(c==3) homeSpan.processSerialCommand("X");} );`
    * 如果设备“短”重启恰好 3 次，每次重启时间少于 5 秒，则导致 HomeSpan 运行 "X" 串行命令，该命令将擦除 WiFi 数据
    * 请注意，创建 3 次短重启意味着你实际上总共循环电源（或按下重置按钮）4 次，自上次允许草图运行而无需重新启动以来

* `Span& setSerialInputDisable(boolean val)`
  * 如果 *val* 为 true，则禁用 HomeSpan 从串口读取输入
  * 如果 *val* 为 false，则重新启用 HomeSpan 从串口读取输入
  * 当需要主 USB 串口从外部串行外围设备读取数据而不是用于从 Arduino 串口监视器读取输入时很有用

* `boolean getSerialInputDisable()`
  * 如果 HomeSpan 从串口读取当前被禁用，则返回 *true*
  * 如果 HomeSpan 正常运行并将读取输入到 Arduino 串口监视器的任何 CLI 命令，则返回 *false*

---

以下**可选** `homeSpan` 方法为更高级的用例提供了额外的运行时功能：

* `void deleteStoredValues()`
  * 删除所有存储特性的值设置来自 NVS
  * 执行的功能与在 CLI 中输入 "V" 相同

* `boolean deleteAccessory(uint32_t aid)`
  * 如果找到，则删除附件 ID 为 *aid* 的附件
  * 如果成功（找到匹配项），则返回 true；如果指定的 *aid* 与任何当前附件不匹配，则返回 false
  * 允许在运行时动态更改附件数据库（即在 Arduino `setup()` 完成后更改配置）
  * 删除附件会自动删除其包含的所有服务、特性和任何其他资源
  * 输出列出所有已删除组件的 1 级日志消息
  * 注意：虽然删除会立即生效，但 HomeKit 控制器（如“家庭”应用）在数据库配置编号更新并重新广播之前不会知道这些更改 - 请参阅下面的 `updateDatabase()`

* `boolean updateDatabase()`
  * 重新计算数据库配置编号，如果发生更改，则通过 MDNS 重新广播新编号，以便所有连接的 HomeKit 控制器（如“家庭”应用）可以请求完全刷新以准确反映新配置
  * 如果配置编号已更改，则返回 true，否则返回 false
  * *仅*在你想要对设备的附件数据库进行运行时（即 Arduino `setup()` 函数完成后）更改时才需要
  * 在动态添加一个或多个附件（使用 `new SpanAccessory(aid)`）或删除一个或多个附件（使用`homeSpan.deleteAccessory(aid)`)
  * **重要**：删除配件后，你不能在添加新配件时（在同一设备上）重复使用相同的 *aid*，除非新配件配置了与已删除配件完全相同的服务和特性
  * 注意：如果你有一个在草图的 Arduino `setup()` 函数中完全定义的静态配件数据库，则**不需要**此方法

* `Span& resetIID(uint32_t newIID)`
  * 将当前附件的 IID 计数重置为 *newIID*，该计数必须大于 0
  * 如果在创建至少一个附件之前调用，则会引发错误并暂停程序
  * 例如：`homeSpan.resetIID(100)` 导致 HomeSpan 将当前附件中定义的下一个服务或特性的 IID 设置为 100，然后向前增加 IID 计数，以便随后定义的任何服务或特性（在同一个附件内）具有 IID=101、102 等。
  * 注意：调用此函数仅影响当前附件的 IID 生成（实例化新附件时，计数将重置为 IID=1）

* `const_iterator controllerListBegin()` and `const_iterator controllerListEnd()`
  * 返回一个*常量迭代器*，指向存储所有控制器数据的不透明链表的*开头*或*结尾*
  * 迭代器应使用“auto”关键字定义，如下所示： `auto myIt=homeSpan.controllerListBegin();`
  * 可以使用以下方法从取消引用的迭代器读取控制器数据：  
    * `const uint8_t *getID()` 返回指向控制器 36 字节 ID 的指针
    * `const uint8_t *getLTPK()` 返回指向控制器的 32 字节长期公钥的指针
    * `boolean isAdmin()` 如果控制器具有管理员权限则返回 true，否则返回 false

  * <details><summary>单击此处查看示例代码</summary><br>

    ```C++
    // 提取并打印有关每个控制器的相同数据，HomeSpan 使用 "s" CLI 命令时会将其打印到串行监视器
    
    Serial.printf("\nController Data\n");
    
    for(auto it=homeSpan.controllerListBegin(); it!=homeSpan.controllerListEnd(); ++it){  // 循环遍历每个控制器
    
      Serial.printf("Admin=%d",it->isAdmin());    // 指示控制器是否具有管理员权限

      Serial.printf("  ID=");                     // 打印控制器的 36 字节设备 ID
      for(int i=0;i<36;i++)
        Serial.printf("%02X",it->getID()[i]);
    
      Serial.printf("  LTPK=");                   // 打印控制器的32字节长期公钥
      for(int i=0;i<32;i++)
        Serial.printf("%02X",it->getLTPK()[i]);
    
      Serial.printf("\n");
    }
    ```
    </details>

---

以下 `homeSpan` 方法被视为实验性的，因为并非所有用例都经过探索或调试。请谨慎使用：

* `void autoPoll(uint32_t stackSize, uint32_t priority, uint32_t cpu)`

  * 一种*可选*方法，用于创建一个单独的任务，该任务在后台重复调用 `poll()`。这释放了 Ardino `loop()` 方法，以便任何用户定义的代码可以并行运行，否则这些代码会阻塞或被 cal 阻塞在 `loop()` 方法中调用 `poll()`。参数及其默认值（如果未指定）如下：

    * *stackSize* - 轮询任务使用的堆栈大小（以字节为单位）。如果未指定，则默认值为 8192
    * *priority* - 任务运行的优先级。最小值为 1。最大值通常为 24，但这取决于 ESP32 操作系统的配置方式。如果将其设置为任意高值（例如 999），它将被设置为允许的最大优先级。如果未指定，则默认值为 1
    * *cpu* - 指定轮询任务将在其上运行的 CPU。有效值为 0 和 1。在单 CPU 板上，此参数将被忽略。如果未指定，则默认为 0
  * 如果使用，**必须**将其放在草图中作为 Arduino `setup()` 方法的最后一行
  * 如果在同一个草图中同时使用 `poll()` 和 `autoPoll()`，HomeSpan 将抛出错误并停止 - 将 `poll()` 放在 Arduino `loop()` 方法中**或**将 `autoPoll()` 放在 Arduino `setup()` 方法的末尾
  * 如果使用此方法，并且你不需要将自己的代码添加到主 Arduino `loop()`，则可以安全地跳过在草图中定义空白的 `void loop(){}` 函数
  * 警告：如果你添加到 Arduino `loop()` 方法的任何代码试图更改任何 HomeSpan 设置或在后台 `poll()` 任务中运行的函数，竞争条件可能会产生未定义的结果

* `TaskHandle_t getAutoPollTask​​()`
  * 返回自动轮询任务的任务句柄，如果未使用自动轮询，则返回空

## *SpanAccessory(uint32_t aid)*

创建此**类**的实例会将新的 HAP 附件添加到 HomeSpan HAP 数据库。

* 每个 HomeSpan 草图至少需要一个附件
* 每个草图最多可包含 150 个附件（如果超过，将引发运行时错误，并且草图将停止）
* 没有关联方法
* 参数 *aid* 是可选的。

  * 如果指定且*不*为零，附件 ID 将设置为 *aid*。
  * 如果未指定或等于零，附件 ID 将设置为比先前实例化的附件的 ID 多 1，如果这是第一个附件，则设置为 1。
  * 实例化的第一个附件必须始终具有 ID=1（如果未指定 *aid*，则为默认值）。
  * 将第一个附件的 *aid* 设置为 1 以外的任何值都会在初始化期间引发错误。

* 你必须在实例化任何附件之前调用 `homeSpan.begin()`
* 示例：`new SpanAccessory();`

## *SpanService()*

这是一个**基类**，所有 HomeSpan 服务都从中派生，不应直接实例化。相反，要创建新服务，请实例化 [服务和特征](ServiceList.md) 命名空间中定义的 HomeSpan 服务之一。不需要任何参数。

* 实例化的服务将添加到 HomeSpan HAP 数据库并与最后实例化的附件相关联
* 在没有先实例化附件的情况下实例化服务会在初始化期间引发错误
* 示例：`new Service::MotionSensor();`

支持以下方法：

* `SpanService *setPrimary()`
  * 指定这是附件的主要服务。返回指向服务本身的指针，以便可以在实例化期间链接该方法
  * 示例：`(new Service::Fan)->setPrimary();`
  * 注意，尽管此功能由 Apple 在 HAP-R2 中定义，但它似乎已被弃用，不再有任何用途或对 “家庭”应用有任何影响

* `SpanService *setHidden()`
  * 指定这是配件的隐藏服务。返回指向服务本身的指针，以便可以在实例化期间链接该方法。
  * 示例：`(new Service::Fan)->setHidden();`
  * 注意，尽管此功能由 Apple 在 HAP-R2 中定义，但它似乎已被弃用，不再有任何用途或对 “家庭”应用有任何影响

* `SpanService *addLink(SpanService *svc)`
  * 将 *svc* 添加为链接服务。返回指向调用服务本身的指针，以便可以在实例化期间链接该方法
  * 请注意，链接服务仅适用于部分 HAP 服务。有关完整详细信息，请参阅 Apple 的 HAP-R2 文档
  * 示例：`(new Service::Faucet)->addLink(new Service::Valve)->addLink(new Service::Valve);`（将两个阀门链接到一个水龙头）

* `vector<T> getLinks<T=SpanService *>(const char *serviceName=NULL)`
  * 模板函数返回使用 `addLink()` 添加的服务指针向量
    * 如果模板参数 T 留空，则返回向量的元素将为 SpanService * 类型
    * 如果指定了模板参数 *T*，则返回向量的元素将被转换为类型 *T*
  * 如果指定了 *serviceName*，则只有与 *serviceName* 匹配的服务才会包含在返回向量中
    * *serviceName* 必须是 HomeSpan 的内置服务之一（例如 "Valve"）
    * 如果 *serviceName* 留空或设置为 NULL，则所有服务都将包含在返回向量中
  * 此功能对于创建遍历所有链接服务的循环非常有用
  * 例如：在包含 *MyValveService* 中定义的链接阀门的水龙头服务中，使用 `for(auto valve : getLinks<MyValveService *>()) { if(valve->active->getVal()) ... }` 来检查哪些阀门处于活动状态

* `virtual boolean update()`
  * HomeSpan 在收到来自 HomeKit 控制器的请求以更新与服务相关的一个或多个特性时调用此方法。用户应该用实现请求的更新的代码覆盖此方法使用下面一个或多个 SpanCharacteristic 方法更新。如果更新成功，方法**必须**返回 *true*，否则返回 *false*。

* `virtual void loop()`
  * 每次执行 `homeSpan.poll()` 时，HomeSpan 都会调用此方法。用户应使用下面一个或多个 SpanCharacteristic 方法，用代码覆盖此方法，以监控需要通知 HomeKit 控制器的特性的状态变化。

* `virtual void button(int pin, int pressType)`
  * 每当触发与服务关联的 SpanButton() 对象时，HomeSpan 都会调用此方法。用户应使用下面一个或多个 SpanCharacteristic 方法，用代码覆盖此方法，以实施响应 SpanButton() 触发器而采取的任何操作。
    * *pin* - 与 SpanButton() 对象关联的 ESP32 引脚
    * *pressType* -
      * 0=单击 (SpanButton::SINGLE)
      * 1=双击 (SpanButton::DOUBLE)
      * 2=长按 (SpanButton::LONG)

* `uint32_t getIID()`
  * 返回服务的 IID

## *SpanCharacteristic(value [,boolean nvsStore])*

这是一个**基类**，所有 HomeSpan 特性都从中派生而来，不应直接实例化。相反，要创建新的特性，请实例化 [服务和特征](ServiceList.md) 命名空间中定义的 HomeSpan 特性之一。

* 实例化的特征被添加到 HomeSpan HAP 数据库并与最后实例化的服务相关联
* 在没有首先实例化服务的情况下实例化特征会在初始化期间引发错误
* 第一个参数可选地允许您在启动时使用以下格式设置特征的初始*值*：
  * 对于数字特征，*值*可以是任何整数或十进制数字类型，例如 `boolean`、`int`、`uint64_t`、`double` 等。HomeSpan 将自动将*值* 转换为具有指定特征的正确数值精度的变量
  * 对于字符串特征，*值*必须是 `char *` 类型或文字引号括起来的 UTF-8 字符串
  * 对于 TLV8 特征，*值*必须是 `TLV8` 类型
  * 对于数据特征，*值*必须是括号括起来的*对*，形式为 `{uint8_t *data, size_t len}`，其中 *len* 指定字节数组 *data* 的大小的长度
* 如果未指定 *value*，HomeSpan 将为特征提供合理的默认值
* 对于数值特征，如果 *value* 超出特征的最小/最大范围，则抛出运行时警告，其中最小/最大是 HAP 默认值，或通过调用 `setRange()` 设置的任何新值
* 如果将第二个可选参数设置为 `true`，则指示 HomeSpan 将此特征值的更新保存在设备的非易失性存储器 (NVS) 中，以便在设备断电时在启动时恢复。如果未指定，*nvsStore* 将默认为 `false`（无存储）
* 示例：
  * `new Characteristic::Brightness();`           亮度初始化为默认值
  * `new Characteristic::Brightness(50);`         亮度初始化为 50
  * `new Characteristic::Brightness(50,true);`    亮度初始化为 50；保存在 NVS 中的更新

#### 以下方法支持基于数值的特性（例如 *int*、*float*...）：

* `type T getVal<T>()`
  * 模板方法，在转换为指定的类型 *T*（例如 *int*、*double* 等）后返回基于数值的特性的**当前**值。如果排除模板参数，值将转换为 *int*。
  * 指定模板的示例：`double temp = Characteristic::CurrentTemperature->getVal<double>();`
  * 排除模板的示例：`int tilt = Characteristic::CurrentTiltAngle->getVal();`

* `type T getNewVal<T>()`
  * 模板方法，返回 HomeKit 控制器请求更新特性所需的**新**值。与 `getVal<>()` 相同的转换规则
  * example with template excluded : `int tilt = Characteristic::CurrentTiltAngle->getVal();`

* `void setVal(value [,boolean notify])`
  * 将基于数字的特性的值设置为 *value*，如果 *notify* 设置为 true，则通知所有 HomeKit 控制器有关更改。*notify* 标志是可选的，如果未指定，则将设置为 true。将 *notify* 标志设置为 false 允许您在不通知任何 HomeKit 控制器的情况下更新特性，这对于 HomeKit 自动调整的特性（例如倒数计时器）很有用，但如果 Home App 关闭然后重新打开，则会从配件中​​请求
  * 适用于任何整数、布尔值或浮点型数值*值*，但 HomeSpan 会将*值*转换为每个特征的适当类型（例如，在基于整数的特征上调用`setValue(5.5)`会导致*值*=5）
  * 如果 *value* 超出了特征的最小/最大范围，则会引发运行时警告，其中最小/最大是 HAP 默认值，或者通过之前调用 `setRange()` 设置的任何新的最小/最大范围
  * 请注意，*值* **不**限于步长增量；例如，在基于浮动的特性上调用 `setRange(0,100,5)` 后调用 `setVal(43.5)` 是完全有效的，即使 43.5 与指定的步长不一致。Home App 将正确保留该值为 43.5，但在滑块图形中使用时（例如设置恒温器的温度），它将四舍五入到最接近的步长增量（在本例中为 45）
  * 如果从 **SpanService** 的 `update()` 例程中调用，并且 `isUpdated()` 对于特征为 *true*（即，它正在通过 Home App 同时更新），则会引发运行时警告，*除非*您正在响应来自 HomeKit 的 *write-response* 请求来更改特征的值（通常仅用于某些基于 TLV 的特征）
  * 请注意，即使特征未获得事件通知 (EV) 许可，此方法也可用于更新特征的值，在这种情况下，HomeSpan 存储的值将被更新，但 Home App 将*不会*收到有关更改的通知

* `SpanCharacteristic *setRange(min, max, step)`
  * 覆盖指定了 *min*、*max* 和 *step* 参数的特性的默认 HAP 范围
  * *step* 是可选的；如果未指定（或设置为非正数），则默认 HAP 步长保持不变
  * 适用于任何整数或浮点型参数，但 HomeSpan 会将参数重铸为每个特征的适当类型（例如，在基于整数的特征上调用 `setRange(50.5,70.3,0.5)` 会导致 *min*=50、*max*=70 和 *step*=0）
  * 如果出现以下情况，则会抛出错误：
    * 在不支持范围更改的特征上调用，或者
    * 在同一个特征上调用多次
  * 返回指向特征本身的指针，以便在实例化期间可以链接该方法
  * 示例：`(new Characteristic::Brightness(50))->setRange(10,100,5);`

* `SpanCharacteristic *setValidValues(int n, [int v1, int v2 ...])`
  * 使用可变长度的 *n* 值列表覆盖具有特定枚举有效值的特征的默认 HAP 有效值 *v1*、*v2* 等。
  * 仅适用于 UINT8、UINT16、UINT32 和 INT 格式的特性
    * 如果在任何其他格式的特性上调用此方法，则会抛出警告消息并忽略请求
  * 返回指向特性本身的指针，以便可以在实例化期间链接该方法
  * 示例：`(new Characteristic::SecuritySystemTargetState())->setValidValues(3,0,1,3);` 创建一个长度为 3 的新有效值列表，其中包含值 0、1 和 3。这会通知 HomeKit，SecuritySystemTargetState 的值为 2（夜间布防）无效，不应在 “家庭”应用中显示为选项

#### 以下方法支持基于字符串的特性（即以空字符结尾的 C 风格字符数组)：

* `char *getString()`
  * 相当于 `getVal()`，但专用于字符串特征（即以空字符结尾的字符数组）

* `char *getNewString()`
  * 相当于 `getNewVal()`，但专用于字符串特征（即以空字符结尾的字符数组）

* `void setString(const char *value)`
  * 相当于 `setVal(value)`，但专用于字符串特征（即以空字符结尾的字符数组）

#### DATA（即字节数组）特征支持以下方法：

* `size_t getData(uint8_t *data, size_t len)`
  * 类似于 `getVal()`，但专用于字节数组特征
  * 用指定大小 *len* 的字节数组 *data* 填充所有字节“编码”为特征的当前值
  * 返回特征中编码的总字节数
  * 如果 *len* 小于编码的总字节数，则不会提取任何数据（即 *data* 未修改），并会抛出一条警告消息，表明 *data* 数组的大小不足以提取特征中编码的所有字节
  * 将 *data* 设置为空 会返回编码的总字节数，但不提取任何数据。这可用于在提取数据之前帮助创建足够大小的 *data* 数组
  * 注意：字节数组特征被编码并传输为 base-64 字符串。HomeSpan 会自动执行此格式与指定字节数组之间的所有编码和解码。但是，当使用 "i" CLI 命令输出到串行监视器时，字节数组特征的值将以其 base-64 字符串格式显示（仅显示前 32 个字符），因为 base-64 是实际传入和传出 HomeKit 的表示形式
  * 如果存储在 Characteristic 中的值不是 base-64 格式，则会引发警告消息

* `size_t getNewData(uint8_t *data, size_t len)`
  * 类似于 `getData()`，但会根据 HomeKit 控制器要求更新特征所需的**新**值，用字节填充指定大小 *len* 的字节数组 *data*

* `void setData(uint8_t *data, size_t len)`
  * 类似于 `setVal()`，但专门用于字节数组特征
  * 通过用字节数组 *data* 中的 *len* 个字节“填充”来更新特征
  
#### TLV8（结构化数据）特征支持以下方法：
* `size_t getTLV(TLV8 &tlv)`

  * 与 `getVal()` 类似，但专门用于 TLV8 特性
  * 用 TLV8 记录填充 TLV8 结构 *TLV*，这些记录来自特征的当前值
  * 返回在特征中编码的总字节数
  * 如果 *tlv8* 不为空，则特征中的 TLV8 记录将附加到任何现有记录中
  * 与 DATA 特征类似，TLV8 特征以 base-64 字符串的形式存储和传输
  * 如果存储在特征中的值不是 base-64 格式，或者似乎不包含 TLV8 记录，则会引发警告消息
* `size_t getNewTLV(TLV8 &tlv)`

  * 类似，但根据 HomeKit 控制器请求将特征更新到的所需**新**值，用 TLV8 记录填充 TLV8 结构 *tlv* `getTLV()`
* `void setTLV(TLV8 &tlv [,boolean notify])`

  * 与 `setVal()` 类似，但专门用于 TLV8 特性
  * 通过将 TLV8 结构 tlv 打包到字节数组中，然后在 base-64 中将其编码为字符串来存储来更新特征值
* `NULL_TLV`

  * 这不是一个方法，而是一个定义为空 TLV8 对象的静态 HomeSpan 常量。它可以在需要空 TLV8 对象的地方使用占位符，例如当您想要使用 nvsStore=true 实例化 TLV8 特征，但您还不想设置 TLV8 值时
  * 例：`new Characteristic::DisplayOrder(NULL_TLV,true);`
* 有关如何使用 HomeSpan 的 TLV8 库读取/处理/创建 TLV8 对象的完整详细信息，请参阅 [TLV8 特征](docs/TLV8.md) 页面。

#### 所有特征都支持以下方法：

* `boolean updated()`
  * 如果 HomeKit 控制器已请求更新特征值，则返回 *true*，否则返回 *false*。请求的值本身可以通过 `getNewVal<>()` 或 `getNewString()` 检索

* `int timeVal()`
  * 返回自上次更新特性值以来经过的时间（以毫秒为单位）（无论是通过 `setVal()`、`setString()` 还是通过 HomeKit 控制器成功更新请求）

* `SpanCharacteristic *setPerms(uint8_t perms)`
  * 将特性的默认权限更改为 *perms*，其中 *perms* 是附加权限列表，如 HAP-R2 表 6-4 中所述。有效值为 PR、PW、EV、AA、TW、HD 和 WR
  * 返回指向特征本身的指针，以便可以在实例化期间链接该方法
  * 示例：`(new Characteristic::IsConfigured(1))->setPerms(PW+PR+EV);`

* `SpanCharacteristic *addPerms(uint8_t perms)`
  * 将新权限 *perms* 添加到特征的默认权限中，其中 *perms* 是附加权限权限列表，如 HAP-R2 表 6-4 中所述。有效值为 PR、PW、EV、AA、TW、HD 和 WR
  * 返回指向特征本身的指针，以便可以在实例化期间链接该方法
  * 示例：`(new Characteristic::IsConfigured(1))->addPerms(PW);`

* `SpanCharacteristic *removePerms(uint8_t perms)`
  * 从 Characteristic 的默认权限中删除权限 *perms*，其中 *perms* 是权限的附加列表，如 HAP-R2 表 6-4 中所述。有效值为 PR、PW、EV、AA、TW、HD 和 WR
  * 返回指向特征本身的指针，以便可以在实例化期间链接该方法
  * 示例：`(new Characteristic::ConfiguredName("HDMI 1"))->removePerms(PW);`

* `SpanCharacteristic *setDescription(const char *desc)`
  * 向特征添加可选描述 *desc*，如 HAP-R2 表 6-3 中所述
  * 此字段通常用于提供有关自定义特征的信息，但 “家庭”应用似乎并未以任何方式使用
  * 返回指向特征本身的指针，以便可以在实例化期间链接该方法
  * 示例：`(new Characteristic::MyCustomChar())->setDescription("Tuner Frequency");`

* `SpanCharacteristic *setUnit(const char *unit)`
  * 添加或覆盖特征的 *unit*，如 HAP-R2 表 6-3 中所述HAP-R2 表 6-6
  * 返回指向特征本身的指针，以便在实例化期间可以链接该方法
  * 示例：`(new Characteristic::RotationSpeed())->setUnit("percentage");`

### *SpanButton(int pin, uint16_t longTime, uint16_t singleTime, uint16_t doubleTime, boolean (\*triggerType)(int))*<a name="spanbutton"></a>

创建此**类**的实例会将按钮处理程序附加到指定的 ESP32 *pin*。

* 实例化的按钮与最后实例化的服务相关联
* 在没有先实例化服务的情况下实例化按钮会在初始化期间引发错误

第一个参数是必需的；其余的是可选的：

* *pin* - 按钮连接的 ESP32 引脚
* *longTime* - 按下并按住按钮以触发长按所需的最短时间（以毫秒为单位）（默认值 = 2000 毫秒）
* *singleTime* - 按下按钮以触发单击所需的最短时间（以毫秒为单位）（默认值 = 5 毫秒）
* *doubleTime* - 两次单击之间允许的最大时间（以毫秒为单位）以符合双击的条件（默认值 = 200 毫秒）
* *triggerType* - 指向布尔函数的指针，该函数接受单个 *int* 参数并返回 `true` 或 `false`，具体取决于传递给 *int* 参数的 *pin* 编号是否触发了“按下”。为方便使用，你可以从以下内置函数中进行选择：
  * `SpanButton::TRIGGER_ON_LOW` - 当 *pin* 驱动为低电平时触发。适用于将 *pin* 连接到 GROUND 的按钮（当未指定 *triggerType* 时，这是默认值）

  * `SpanButton::TRIGGER_ON_HIGH` - 当 *pin* 驱动为高电平时触发。适用于将 *pin* 连接到 VCC（通常为 3.3V）的按钮
  * `SpanButton::TRIGGER_ON_TOUCH` - 使用设备的触摸传感器外设在连接到 *pin* 的传感器被触摸时触发（ESP32-C3 设备上不可用）

当选择任何这些内置函数时（或 *triggerType* 未指定并使用默认值），SpanButton 将在实例化时根据需要自动配置 *pin*。

或者，你可以将 *triggerType* 设置为任何用户定义的函数，形式为 `boolean(int arg)`，并提供你自己的逻辑来确定在指定的 *pin* 上是否发生了触发，该逻辑将作为 *arg* 传递给你的函数。在这种情况下，*arg* 可以表示实际的设备引脚，也可以只是你的函数使用的任意 *int*，例如多路复用器上的虚拟引脚号。注意：如果你为 *triggerType* 指定自己的函数，你还必须在草图中包含初始化逻辑或配置 *triggerType* 正在使用的任何资源（例如引脚多路复用器）所需的任何代码。

为方便起见，还提供了 *SpanButton()* 构造函数的第二种形式：
* `SpanButton(int pin, boolean (*triggerType)(int), uint16_t longTime=2000, uint16_t singleTime=5, uint16_t doubleTime=200)`
  * 这允许你仅设置 *pin* 和 *triggerType*，同时将其余参数保留为默认值

#### 触发规则 ###
* 如果按下按钮并持续按住，则每隔 longTime 毫秒将触发一次长按，直到按钮被释放
* 如果按下按钮的时间超过 singleTime 毫秒但少于 longTime 毫秒，然后释放，将触发一次单击，除非在 doubleTime 毫秒内第二次按下按钮并再次按住至少 singleTime 毫秒，在这种情况下将触发一次双击；直到按钮被释放，才会发生其他事件
* 如果 singleTime>longTime，则只能发生长按触发
* 如果 doubleTime=0，则不能发生双击效果

#### 用法 ####
HomeSpan 会在与该服务关联的任何 SpanButton 中触发事件时自动调用服务的 `button(int pin, int pressType)` 方法，其中 *pin* 是按钮所连接的 ESP32 引脚，而 *pressType* 是一个整数，也可以用以下枚举常量表示：
* 0=单击（`SpanButton::SINGLE`）
* 1=双击（`SpanButton::DOUBLE`）
* 2=长按（`SpanButton::LONG`）

如果用户没有覆盖包含一个或多个按钮的服务的虚拟 button() 方法，则 HomeSpan 将在初始化期间报告警告，但不会报告错误；这些按钮的触发器将被忽略。

使用一个或多个触摸传感器时，HomeSpan 会在实例化第一个类型为 "SpanButton::TRIGGER_ON_TOUCH" 的 SpanButton 时轮询基线传感器读数，从而自动校准触发触摸传感器的阈值。对于 ESP32 设备，阈值设置为基线值的 50%，因为当传感器值低于阈值水平时会发生触发。对于 ESP32-S2 和 ESP32-S3 设备，阈值设置为基线值的 200%，因为当传感器值高于阈值水平时会发生触发。通常，HomeSpan 的自动校准功能可以准确检测触摸传感器的单击、双击和长按。但是，如果需要，你可以使用以下类级方法覆盖校准并设置自己的阈值：

* `void SpanButton::setTouchThreshold(uintXX_t thresh)`
  * 将阈值设置为高于（对于 ESP32 设备）或低于（对于 ESP32-S2 和 ESP32-S3 设备）触发触摸传感器的 *thresh*
  * *XX* 为 16（对于 ESP32 设备）或 32（对于 ESP32-S2 和 ESP32-S3 设备）
  * 指定的阈值被视为全局的，用于 *所有* 类型为 `SpanButton::TRIGGER_ON_TOUCH` 的 SpanButton 实例
  * 此方法可以在创建 SpanButton 之前或之后调用

此外，你还可以使用以下类级方法覆盖 ESP32 的触摸传感器时序参数：

* `void SpanButton::setTouchCycles(uint16_t measureTime, uint16_t sleepTime)`
  * 将测量时间和睡眠时间时钟周期分别更改为 *measureTime* 和 *sleepTime*。这只是对 Arduino-ESP32 库 `touchSetCycles()` 函数的传递调用
  * 除非已使用 `setTouchThreshold()` 设置特定阈值，否则必须在实例化第一个类型为 `SpanButton::TRIGGER_ON_TOUCH` 的 SpanButton() *之前*调用 `setTouchCycles()`，以便 HomeSpan 根据指定的新计时参数校准触摸阈值

### *SpanToggle(int pin, boolean (\*triggerType)(int)=PushButton::TRIGGER_ON_LOW, uint16_32 toggleTime=5)*

创建此**类**的实例会将拨动开关处理程序附加到指定的 ESP32 *pin*。这是 *SpanButton* 的子类，因此派生了所有相同的功能。例如，你可以将 *triggerType* 设置为 PushButton::TRIGGER_ON_HIGH，创建自己的触发器函数等。但是，当按钮被“按下”时，HomeSpan 不会调用 `button(int pin, int pressType)`，而是在开关从一个位置“切换”到另一个位置时，HomeSpan 会调用相同的 `button()` 方法。在这种情况下，传递给 `button()` 的参数 *pressType* 具有一组不同的枚举：
* 3=开关已关闭（`SpanToggle::CLOSED`）
* 4=开关已打开（`SpanToggle::OPEN`）

请注意，构造函数中没有 *singleTime*、*longTime* 或 *doubleTime* 参数，因为你不能单击、双击或长按切换开关。相反，构造函数支持单个参数 *toggleTime*（如果未指定，则默认值为 5ms），该参数设置开关需要移动到关闭位置才能触发对 `button()` 方法的调用的最短时间。这有效地“消除了”拨动开关的抖动。

SpanToggle 还支持以下附加方法：

* `int position()`
  * 返回切换开关的当前位置（即 SpanToggle::CLOSED 或 SpanToggle::OPEN）
  * 相当于传递给 `button()` 方法的 *pressType* 参数，但可以在草图中的任何位置调用
  * 适用于在启动时读取接触开关的初始状态，以便可以相应地设置 Characteristic::ContactSensorState 的初始值
  * 示例 `sensorState=new Characteristic::ContactSensorState(toggleSwitch->position()==SpanToggle::OPEN);`

### *SpanUserCommand(char c, const char \*desc, void (\*f)(const char \*buf [,void \*obj]) [,void \*userObject])*

创建此**类**的实例会将用户定义的命令添加到HomeSpan命令行界面，其中：

* *c* 是用户定义命令的单字母名称
* *desc* 是用户定义命令的描述，当用户在 CLI 中键入 "？" 时显示
* *f* 是指向用户定义函数的指针，当 com调用 mand。*f* 的允许形式为：
  1. `void f(const char *buf)`，或
  1. `void f(const char *buf, void *obj)`
* *userObject* 是指向 HomeSpan 传递给函数 *f* 的任意对象的指针，当使用 *f* 的第二种形式时，该对象作为第二个参数。请注意，当使用 *f* 的第一种形式时，包含 *userObject* 是错误的，当使用 *f* 的第二种形式时，排除 *userObject* 也是错误的

要从 CLI 调用自定义命令，请在单字母名称 *c* 前面加上 "@"。这允许 HomeSpan 区分用户定义的命令和其内置命令。例如，

```C++
new SpanUserCommand('s', "save current configuration", saveConfig);
...
void saveConfig(const char *buf){ ... };
```

将向 CLI 添加一个新命令 '@s'，其描述为“保存当前配置”，该命令将在调用时调用用户定义函数 `void saveConfig(const char *buf)`。参数 *buf* 指向在 '@' 后输入到 CLI 中的所有字符的数组。这允许用户将参数从 CLI 传递给用户定义函数。例如，在调用 saveConfig 时，在 CLI 中输入 '@s123' 会将 *buf* 设置为 "s123"。

在参数的第二种形式中，HomeSpan 将向函数 *f* 传递一个附加对象。例如，

```C++
struct myConfigurations[10];
new SpanUserCommand('s', "<n> save current configuration for specified index, n", saveConfig, myConfigurations);
...
void saveConfig(const char *buf, void *obj){ ... do something with myConfigurations ... };
```

可能用于在仅使用 '@s' 命令调用时保存 *myArray* 中的所有元素，并且可能仅根据添加到命令的索引保存一个元素，例如 '@s34' 用于保存 *myArray* 中的元素 34。用户需要在函数 *f* 中创建所有必要的逻辑，以解析和处理传入 *buf* 的完整命令文本，以及对通过 *obj 传递的任何内容采取行动。

要创建多个用户定义的命令，只需创建 SpanUserCommand 的多个实例，每个实例都有自己的单字母名称。请注意，在 SpanUserCommand 实例中重复使用相同的单字母名称会覆盖任何使用相同字母的先前实例。

## 自定义特性和自定义服务宏<a name="custom"></a>

### *CUSTOM_CHAR(name,uuid,perms,format,defaultValue,minValue,maxValue,staticRange)*
### *CUSTOM_CHAR_STRING(name,uuid,perms,defaultValue)*
### *CUSTOM_CHAR_DATA(name,uuid,perms)*

创建可添加到任何服务的自定义特性。自定义特性通常被 “家庭”应用忽略，但可能被其他第三方应用程序使用（例如 *Eve 应用*）。第一种形式应该用于创建数值特性（例如，UINT8、BOOL...）。第二种形式用于基于字符串的特性。第三种形式用于基于数据（即字节数组）的特性。参数如下（请注意，任何宏参数都不应使用引号，但应用于基于字符串的特性时的 *defaultValue* 除外）：

* *name* - 自定义特性的名称。这将被添加到 Characteristic 命名空间，以便可以像任何 HomeSpan Characteristic 一样对其进行访问。对非 ASCII 字符使用 UTF-8 编码字符串。
* *uuid* - 制造商定义的特性的 UUID。必须是：
  * *正好* 36 个字符，格式为 XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX，其中 *X* 代表有效的十六进制数字，或
  * 单个十六进制数，格式为 XXXXXXXX，*8 位或更少*，没有前导零
* *perms* - 附加权限列表。有效值为 PR、PW、EV、AA、TW、HD 和 WR（例如 PR+PW+EV）
* *format* - 对于数值特性，指定数字格式。有效值为 BOOL、UINT8、UINT16、UNIT32、UINT64、INT 和 FLOAT。不适用于 STRING、DATA 或 TLV8 特性宏
* *defaultValue* - 指定在实例化期间未定义特性的默认值。不适用于 DATA 或 TLV7 特征宏。
* *minValue* - 指定有效值的默认最小范围，可以通过调用 `setRange()` 进行覆盖。不适用于 STRING、DATA 或 TLV8 特征宏
* *minValue* - 指定有效值的默认最小范围，可以通过调用 `setRange()` 进行覆盖。不适用于 STRING、DATA 或 TLV8 特征宏
* *staticRange* - 如果 *minValue* 和 *maxValue* 是静态的，并且不能通过调用 `setRange()` 进行覆盖，则设置为 *true*。如果允许调用 `setRange()`，则设置为 *false*。不适用于 STRING、DATA 或 TLV8 特征宏

例如，下面的第一行创建了一个名为 "Voltage" 的自定义特征，其 UUID 代码可被 *Eve 应用*识别。参数显示该特征是只读的 (PR) 并启用通知 (EV)。允许值的默认范围是 0-240，默认值为 120。随后调用 `setRange()` 可以覆盖该范围。下面的第二行创建了一个自定义的只读字符串型特性：

```C++
CUSTOM_CHAR(Voltage, E863F10A-079E-48FF-8F27-9C2605A29F52, PR+EV, UINT16, 120, 0, 240, false);
CUSTOM_CHAR_STRING(UserTag, AAAAAAAA-BBBB-AAAA-AAAA-AAAAAAAAAAAA, PR, "Tag 123");
...
new Service::LightBulb();
new Characteristic::Name("Low-Voltage Lamp");
new Characteristic::On(0);
new Characteristic::Brightness(50);
new Characteristic::Voltage(12); // 添加电压特征并将初始值设置为 12 伏
new Characteristic::UserTag(); // 添加用户标签特征并保留默认初始值“标签 123”
```

请注意，必须在全局级别（即不在 `setup()` 内）创建自定义特征，并在调用 `homeSpan.begin()` 之前创建

> 高级提示 1：当出现无法识别的自定义特征时，*Eve 应用*会显示一个 *通用控件*，允许你与在 HomeSpan 中创建的任何自定义特征进行交互。但是，由于 Eve 无法识别该特征，因此只有在特征包含 **description** 字段时，它才会呈现通用控件，你可以使用上述 `setDescription()` 方法将其添加到任何特征中。你可能还想使用 `setUnit()` 和 `setRange()`，以便 Eve 应用显示具有适合你的自定义特征范围的控件。

> 高级提示 2：尽管 DATA 格式是 HAP-R2 规范的一部分，但目前任何原生 “家庭”应用特性均未使用该格式。HomeSpan 中包含此格式是因为其他应用程序（例如 *Eve 应用*）确实使用这些类型的特性来创建 “家庭”应用之外的功能，因此可供高级用户进行实验。

> 高级提示 3：使用多文件草图时，如果你在多个文件中定义相同的自定义特性，编译器将抛出“重新定义错误”。为避免此错误并允许在多个文件中使用相同的自定义特性，请在包含先前定义的自定义特性的*重复*定义的每个文件中的 `#include "HomeSpan.h"` *之前*添加行 `#define CUSTOM_CHAR_HEADER`。

### *CUSTOM_SERV(name,uuid)*

创建自定义服务以用于第三方应用程序（例如 *Eve 应用*）。自定义服务将在原生 Apple Home 应用中显示，其 Tile 标记为“不支持”，否则 Home 应用将安全地忽略该服务。参数如下（请注意，任何宏参数中都不应使用引号）：

* *name* - 自定义服务的名称。这将添加到服务命名空间，以便像任何 HomeSpan 服务一样对其进行访问。例如，如果 *name*="Vent"，HomeSpan 会将 `Service::Vent` 识别为新服务类
* *uuid* - 制造商定义的服务的 UUID。必须 *正好* 为 36 个字符，格式为 XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX，其中 *X* 代表有效的十六进制数字。如果需要，则需要前导零，如 HAP-R2 第 6.6.1 节中更详细地描述的那样

自定义服务可能包含自定义特性和标准 HAP 特性的混合，但由于服务本身是自定义的，因此即使服务包含一些标准 HAP 特性，“家庭”应用也会忽略整个服务。请注意，必须在调用 `homeSpan.begin()` 之前创建自定义服务

可以在 Arduino IDE 下的 [*文件→示例→HomeSpan→其他示例→CustomService*](../examples/Other%20Examples/CustomService/CustomService.ino) 下找到一个完整的示例，该示例展示了如何使用 ***CUSTOM_SERV()*** 和 ***CUSTOM_CHAR()*** 宏来创建 *Eve 应用*识别的压力传感器配件。

## 其他宏

### *SPAN_ACCESSORY()* 和 *SPAN_ACCESSORY(NAME)*

一个“便利”宏，它实现了创建配件时使用的以下非常常见的代码片段。仅当 *NAME*（C 样式字符串）已作为宏的参数包含时，才会包含最后一行：

```C++
new SpanAccessory();
new Service::AccessoryInformation();
new Characteristic::Identify();
new Characteristic::Name(NAME); // 仅包含在宏的第二种形式中
```

## 用户可定义的宏

### *#define REQUIRED VERSION(major,minor,patch)*

如果在使用 `#include "HomeSpan.h"` 包含 HomeSpan 库*之前*在主草图中定义了 REQUIRED，则 HomeSpan 将抛出编译时错误，除非包含的库版本等于或晚于使用 VERSION 宏指定的版本。示例：

```C++
#define REQUIRED VERSION(1,3,0) // 抛出编译时错误，除非使用的 HomeSpan 库是版本 1.3.0 或更高版本
#include "HomeSpan.h"
```

---

[↩️](../README.md#resources) 返回欢迎页面
