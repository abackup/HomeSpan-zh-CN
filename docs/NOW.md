<!--  原文时间：2024.3.8，翻译时间：2024.5.6，校对时间：2024.7.12  -->

# SpanPoint：ESP32 设备之间的点对点通信

SpanPoint 是 HomeSpan 对乐鑫 ESP-NOW 协议的易于使用的实现。SpanPoint 根据 MAC 地址直接在 ESP32 设备之间提供小型、固定大小消息的双向点对点通信，无需家庭 WiFi。当配置为远程设备时，SpanPoint 也可以在 ESP-8266 上使用。

要在任意两个设备之间建立连接，只需在每个设备上实例化一个 SpanPoint 对象，该对象引用另一个设备的 MAC 地址，并指定每个设备预期发送或接收到另一个设备的消息的大小（可能不同）。

SpanPoint 创建管理消息流所需的所有内部数据队列，配置 ESP-NOW 以加密所有消息流量，并自动设置 ESP-NOW 用于传输的 WiFi 通道，以匹配也连接到的任何设备所需的任何内容 HomeKit 通过你的家庭 WiFi。

SpanPoint 是 HomeSpan 库的一部分，可以通过在草图顶部附近添加 `#include "HomeSpan.h"` 来访问。下面提供了 SpanPoint 类及其所有方法的详细描述。

## *SpanPoint(const char \*macAddress, int sendSize, int receiveSize [, int queueDepth=1 [, boolean useAPaddress=false]])*

创建此**类**的实例使设备能够向另一个 ESP32 设备上的 *SpanPoint* 的“补充”实例发送消息和/或从其接收消息。参数及其默认值（如果未指定）如下：

   * *macAddress* - 你想要向其发送数据和/或从中接收数据的*其他*设备的 MAC 地址，采用标准 6 字节格式 "XX:XX:XX:XX:XX:XX"，其中每个 XX 代表从 00 到 FF 的单个 2 位十六进制字节
   * *sendSize* - 将从该设备发送到*其他*设备的任何消息的大小（以字节为单位）。允许的范围是 0 到 200，其中值 0 用于向 SpanPoint 表示你**不会**使用 `send()` 将任何消息从该设备传输到*其他*设备
   * *receiveSize* - 该设备将从*其他*设备接收的任何消息的大小（以字节为单位）。允许的范围是 0 到 200，其中值 0 用于向 SpanPoint 表示你**不会**使用 `get()` 来检索由*其他*设备传输到此设备的任何消息
   * *queueDepth* - 保留的队列深度，用于保存该设备从*其他*设备接收到的 *receiveSize* 字节的消息，但尚未使用 `get()` 检索。如果未指定，则默认=1，这对于大多数应用来说应该足够了。有关更多详细信息，请参阅下面的 `get()`
   * *useAPaddress* - SpanPoint 通常使用 STA MAC 地址通过 ESP32 的 WiFi 站 (STA) 接口进行通信。将 *useAPaddress* 设置为 *true* 会导致 SpanPoint 使用 AP MAC 地址通过 ESP32 的 WiFi 接入点 (AP) 接口进行通信。当使用 ESP-8266 作为远程设备时需要这样做（见下文）。如果未指定，则默认为 *false*

通过在 CLI 中键入"i"，将在草图中实例化的所有 SpanPoint 对象的列表、上面指定的参数以及每个远程设备用于连接回主 HomeSpan 设备的特定 MAC 地址显示在串口监视器中

> 如果每个 SpanPoint 对象中指定的 MAC 地址引用彼此的设备，并且一台设备上的 SpanPoint 对象的 *sendSize* 和 *receiveSize* 分别与另一台设备上的 *sendSize* 和 *receiveSize* 匹配，则在两个单独设备上创建的 SpanPoint 对象被视为“互补”。但是，无论另一个设备上的值如何设置，始终可以将 *sendSize* 和 *receiveSize* 设置为零。

如果发生以下情况，SpanPoint 将在实例化期间抛出致命错误并停止草图：
   * 指定的 *macAddress* 格式不正确，或者
   * *sendSize* 或 *receiveSize* 设置为大于 200，或者
   * *sendSize* 和 *receiveSize* 均设置为 0，因为 SpanPoint 既不发送也不接收数据
   
**以下 SpanPoint 方法用于从一个设备上的 SpanPoint 对象向*其他*设备上的互补 SpanPoint 对象传输和接收消息：**

* `boolean send(const void *data)`

   * 使用 *data* 指向的数据（可以是标准数据类型，例如 *uint16_t* 或用户定义的 *struct*）向 *其他* 设备传输消息
   * 要传输的 *data* 元素的大小与创建 SpanPoint 对象时指定的 *sendSize* 参数非常匹配
   * 如果传输成功则返回**true**，如果传输失败则返回**false**。请注意，只要设备可以根据其 MAC 地址找到并连接到*其他*设备，则传输被视为成功，无论其他设备是否具有相应的 SpanPoint 对象
  
* `boolean get(void *dataBuf)`

   * 检查消息是否已从*其他*设备接收到 SpanPoint 的内部消息队列
   * 如果没有可用的消息，该方法返回 **false** 并且 *dataBuf* 未修改
   * 如果消息可用，则将其从内部消息队列 **移动** 到 *dataBuf* 并且该方法返回 **true**
   * 消息的大小始终等于创建 SpanPoint 对象时指定的 *receiveSize* 参数，因此请确保 *dataBuf* 的大小足以存储此类消息

请注意，无论你是否调用 `get()` 方法，SpanPoint 都会配置为存储（在内部队列中）从*其他*设备接收到的任何 SpanPoint 消息，前提是 (a) 内部队列中有空间，以及 (b) 接收到的消息的大小与实例化相关 SpanPoint 对象时指定的 *receiveSize* 参数相匹配。如果收到消息时内部队列已满，则该消息*不会*移动到队列中，而是在通过`get()`方法检索之前被丢弃。为了避免这种情况，请确保你调用 `get() `的频率高于预期接收消息的频率，或者在实例化 SpanPoint 对象时将 *queueDepth* 参数设置为大于 1 的值。

另请注意，无论队列是否已满，如果接收到的消息的大小与为此 SpanPoint 对象实例指定的 *receiveSize* 参数不匹配，则该消息将被丢弃。如果 *receiveSize* 大于零，则还将在串口监视器上输出有关大小不匹配的非致命运行时警告。

**SpanPoint支持的其他方法如下：**

* `uint32_t time()`

   * 返回自 SpanPoint 对象上次收到有效消息以来经过的时间（以毫秒为单位）
   * 有效消息是那些可以正确解密且大小与 *receiveSize* 参数匹配的消息，无论队列中是否有空间存储该消息
   * 使用 get() 读取队列中的消息对经过的时间计算没有影响
   * 此方法通常用于检查来自传输设备的消息是否过期（表明该设备存在潜在问题）

* `static void setPassword(const char *pwd)`

   * 此*可选***类级**方法将用于为所有 SpanPoint 对象生成 ESP-NOW 加密密钥的默认密码短语从默认密码短语 ( "HomeSpan" ) 更改为 *pwd*，它可以是以下字符串 任意长度
   * 如果使用，必须在实例化任何 SpanPoint 对象之前调用此方法。示例： `SpanPoint::setPassword("MyPassword");`
   * 通过 SpanPoint 进行通信的所有设备必须使用相同的密码，否则接收设备将无法解密其收到的消息


* `static void setEncryption(boolean encrypt)`

  * 此*可选*的*类级*方法提供了根据 encrypt 设置为 true 还是 false 启用或禁用加密的功能
  * 默认情况下，通常启用加密（使用上面的密码）
  * 如果使用，则必须在实例化任何 SpanPoint 对象*之前*调用此方法。示例：`SpanPoint::setEncryption(false);` 禁用所有 SpanPoint 连接的加密
  * 请注意，这是一个全局设置——如果在主设备上禁用了 SpanPoint 加密，则还必须在每个远程设备上禁用它，否则设备之间的通信将失败
  * 启用/禁用加密会影响 ESP32 的 ESP-NOW 功能可以支持的 SpanPoint 连接总数：
    * 启用加密后，ESP32 最多可以支持 7 个 ESP-NOW 链路（即 7 个 SpanPoint 实例）
    * 禁用加密后，ESP32 最多可以支持 20 个 ESP-NOW 链路（即 20 个 SpanPoint 实例）


* `static void setChannelMask(uint16_t mask)`

   * 这个*可选***类级**方法将默认通道位掩码从 0x3FFE（即 0011 1111 1111 1110）更改为*掩码*
   * 通道位掩码用于限制每当 SpanPoint 在传输失败后需要重置 ESP-NOW 通道时应尝试 ESP32 WiFi 无线电支持的标准通道 (1-13)
   * 将位掩码中的位号 *N* 设置为 1，其中 N=[1,13]，启用 WiFi 通道号 *N*
   * 将位掩码中的位号 *N* 设置为 0，其中 N=[1,13]，禁用 WiFi 通道号 *N* 的使用
   * 示例： `SpanPoint::setChannelMask(1<<1 | 1<<6 | 1<<11);` 导致 SpanPoint 在传输消息时仅尝试 WiFi 通道 1、6 和 11
   * 如果使用不启用至少一个通道的 *mask* 调用此方法，则会引发致命错误并停止草图
   * 如果在通过家庭 WiFi 网络连接到 HomeKit 的完整 HomeSpan 草图中使用此方法，则对 SpanPoint 没有影响，因为在这些条件下，WiFi 通道必须保持设置为家庭 WiFi 网络所需的任何值

## 典型用例

使用 SpanPoint 的主要原因之一是支持电池供电设备的部署。由于 HomeKit 需要始终在线的 WiFi 连接，因此壁式电源是必须的。但 ESP-NOW 不需要始终连接到家庭 WiFi 网络，这使得只需电池即可为远程传感器设备等设备供电。这种电池供电的“远程设备”可以定期进行本地测量，并通过 SpanPoint 消息将它们传输到墙上供电的“主设备”，该“主设备”正在运行通过家庭 WiFi 网络连接到 HomeKit 的完整 HomeSpan 草图。

显示此类配置的示例可以在 Arduino IDE 中的 [*文件→例子→HomeSpan→其他示例→RemoteSensors*](../examples/Other%20Examples/RemoteSensors) 下找到。该文件夹包含以下草图：

* *MainDevice.ino* - 一个完整的 HomeSpan 草图，实现了两个温度传感器附件，但它不是进行自己的温度测量，而是使用 SpanPoint 读取包含来自其他远程设备的温度更新的消息
* *RemoteDevice.ino* - 一个轻量级草图，模拟定期进行温度测量，然后通过 SpanPoint 传输到主设备
* *RemoteTempSensor.ino* - 一个类似于 *RemoteDevice.ino* 的轻量级草图，不同之处在于它不是模拟温度传感器，而是实现了实际的 Adafruit ADT7410 基于 I2C 的温度传感器。该草图还使用了一些电源管理技术来延长电池寿命，例如降低 CPU 频率并在每次测量后进入深度睡眠
* *RemoteDevice8266.ino* - 功能与 *RemoteDevice.ino* 类似，但使用本机 ESP-NOW 命令在 ESP8266 设备上运行（因为 HomeSpan 和 SpanPoint 都不支持 ESP8266）。请注意，必须将 ESP32 上从 ESP8266 接收数据的“互补”SpanPoint 对象配置为使用 ESP32 的 *AP MAC 地址*（而不是 *STA MAC 地址*），方法是在 SpanPoint 构造函数将 *useAPaddress* 设置为 *true* 

另请参阅 [SpanPointLightSwitch 库](https://github.com/abackup/SpanPointLightSwitch-Chinese)，了解如何使用 SpanPoint 在实现两个 HomeKit 灯泡配件的 ESP32 “家庭中心”设备、控制 LED 的远程 ESP32 设备和控制另一个 LED 的单独 ESP8266 设备之间进行双向通信。

---

[↩️](../README.md#resources) 返回欢迎页面
