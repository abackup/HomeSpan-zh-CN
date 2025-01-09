<!--  原文时间：2024.2.19，翻译时间：2024.5.6，校对时间：2024.7.12  -->

# 常见问题

*常见问题的答案列表，以及对各种感兴趣话题的讨论。*

#### 如何设置我的 WiFi 网络名称和密码（即 WiFi 凭据）？

* 虽然商业 HomeKit 设备可以从 iPhone 自动检索 WiFi 凭据，但苹果并未在 HomeSpan 使用的非商业版本的 HomeKit 中提供此机制。可选地，你需要使用以下四种方法之一将你的 WiFi 凭据添加到 HomeSpan 设备：

   * 在 Arduino IDE 中，只需使用 HomeSpan命令行界面并在串口监视器中键入 "W"。HomeSpan 将提示你输入你的 WiFi 网络名称和密码。你只需执行一次，因为 HomeSpan 将你提供的 WiFi 凭据存储在设备的非易失性存储 (NVS) 中，以便在设备每次启动时使用。有关完整的详细信息，请参阅 [命令行界面](CLI.md)。
  
  * 作为替代方案，特别是如果你的 HomeSpan 设备未连接到电脑，你可以启动 HomeSpan 的临时 WiFi 设置网络并将你的 WiFi 凭据直接输入到 HomeSpan 提供的临时 Web 表单中。与上面类似，这只需执行一次，因为 HomeSpan 同样存储你使用此方法输入的 WiFi 凭据。有关详细信息，请参阅 [用户指南](UserGuide.md#设置-homespan-的-wifi-凭据和设置代码)。

  * 如果你真的想，你可以使用 HomeSpan 的 `setWifiCredentials()` 方法以编程方式直接在你的草图中设置你的 WiFi 凭据。但是，**不**建议这样做，因为将敏感密码硬编码到草图中会带来安全风险，并且通常不被认为是一种好的做法。有关详细信息，请参阅 [API 参考](Reference.md)。

  * 最后，对于高级用户，HomeSpan 提供了一个 API 挂钩 `setApFunction()`，允许你创建自己的自定义方法来输入你的 WiFi 凭据。有关详细信息，请参阅 [API 参考](Reference.md)。

#### 用于将 HomeSpan 设备与 HomeKit 配对的设置代码是什么？

* HomeSpan 默认设置代码为 466-37-726。你可以（也应该）使用 HomeSpan命令行界面将此默认设置更改为每个设备唯一的密码。只需在串口监视器中键入 "S \<code\>"，或在使用 HomeSpan 的临时 WiFi 设置网络配置 WiFi 凭据时一开始就指定所需的设置代码。或者，你可以使用 HomeSpan 的 `setPairingCode()` 方法以编程方式在草图中设置设置代码。但是，在草图中硬编码你的设置代码通常被认为存在安全风险，并且与苹果公司的 HomeKit 指南推荐的不一致。

#### 你可以在同一个 HomeKit 网络上使用多个 HomeSpan 设备吗？

* 是的，多个 ESP32，每个运行一个单独的 HomeSpan 副本，可以在同一个 HomeKit 网络上使用，前提是每个设备都有一个唯一的*设备 ID* 、唯一的 *Host Name* 和唯一的 *Display Name*。通常，*设备 ID* 是 HomeSpan 在启动时随机生成的，因此在多个设备中将自动唯一。此外，除非你覆盖默认 *Host Name* 的后缀，否则它在多个设备中也是唯一的，因为 HomeSpan 使用 *设备 ID* 作为 *Host Name* 的后缀。你唯一需要确保的是为每个设备分配不同的*显示名称*。有关如何执行此操作的详细信息，请参阅 [API 参考](https://github.com/HomeSpan/HomeSpan/blob/master/docs/Reference.md)。

#### Does HomeSpan require the use of a HomeKit Hub, such as a HomePod or Apple TV?

* Yes. If you want HomeSpan to be able to send notifications, such as status updates if a contact sensor is opened, or updates from a temperature sensor, you must use a HomeKit Hub.  If your device can only be operated from the Home App (i.e. it has no local control buttons) and does not generate any status messages, than a HomeKit Hub *may* not be necessary.  Note you will also need a HomeKit Hub if you wish to control your device via the Internet when away from your local WiFi network.  These requirements and conditions are not specific to HomeSpan, but apply to all commercial HomeKit devices as a result of Apple updating its HomeKit Architecture (as of iOS 16.4).

#### HomeSpan 是否支持视频和音频流？

* 不，HomeSpan 不支持流媒体服务，例如音频流管理、数据流传输管理或摄像头 RTP 管理。有关 HomeSpan 支持的所有服务的完整列表，请参阅 [服务和特征](ServiceList.md)。

#### HomeSpan 是否与 PlatformIO 一起使用？

* HomeSpan 不明确支持 PlatformIO，但多个用户报告成功使用带有 PlatformIO 的 HomeSpan 库（译者注：可以与 PlatformIO 一起使用）。

#### HomeSpan 可以在 ESP8266 设备上工作吗？

* 不，HomeSpan 是专门为 ESP32 编码的，不能在 ESP8266 设备上运行。


#### 如何阅读 [OTA](OTA.md) 文档中提到的 HomeSpan 的 MDNS 广播？

* HomeSpan 使用 MDNS（多播 DNS）广播希望与 HomeSpan 配对的控制权使用的各种 HAP 信息。苹果使用名称 *Bonjour* 来指代 MDNS，最初在 Safari 中包含一个 Bonjour“浏览器”，该浏览器现已停产。但是，有许多可在 Mac 和 iPhone 上免费运行的替代 MDNS 浏览器，例如 [Discovery - DNS-SD 浏览器](https://apps.apple.com/us/app/discovery -dns-sd-browser/id1381004916?mt=12)。你会在名为 *_hap._tcp 的 MDNS 服务下找到所有 HomeSpan 设备以及你可能拥有的任何其他 HomeKit 设备。* HomeSpan 广播的字段是 HAP 所需的所有数据元素的组合（HAP-R2 , 表 6-7) 加上三个额外的 HomeSpan 字段：

  * *hspn* - 用于编译草图的 HomeSpan 库的版本号
  * *sketch* - 草图的版本号，由 `homeSpan.setSketchVersion(const char *)` 指定，如果没有指定版本，则 *n/a*
  * *ota* - *yes* 或 *no* 取决于是否使用方法 `homeSpan.enableOTA()` 为草图启用了 OTA

#### HomeSpan 是否支持电视服务？

* 是的。虽然苹果没有记录，也不是 HAP-R2 的正式组成部分，但 HomeSpan 支持 HomeKit 电视控制。有关详细信息，请参阅 [电视服务](../docs/TVServices.md)。

#### 你可以通过蓝牙使用 HomeSpan 吗？

* 不支持。HomeSpan 不支持通过蓝牙连接 HomeKit 的苹果 HAP-R2 协议。但是，如果需要，你仍然可以将设备上的蓝牙无线电用于与 HomeKit 无关的其他连接。

#### 你可以将 HomeSpan 与以太网连接而不是 WiFi 连接一起使用吗？

* 现在不行。尽管使用兼容的以太网板，ESP32 可以配置为作为以太网服务器运行，但由于以太网 UDP 堆栈存在一些明显的问题，使用以太网上的 MDNS 无法在 ESP32 上运行。不幸的是，HomeSpan 和 HAP-R2 需要 MDNS 才能运行。如果有人设法让 MDNS 的以太网版本在 ESP32 上运行，请告诉我 - 将以太网支持添加到 HomeSpan 会很棒。

#### HomeSpan 是否适用于 SPI 和 I2C？

* 是的，标准 Arduino 库 `SPI.h` 和 `Wire.h` 在 HomeSpan 环境中都可以正常工作。通常，从 SPI 或 I2C 设备读取的代码是在 HomeSpan 服务的 `loop()` 方法中实现的，任何初始化都在该服务的构造函数中完成。有关说明性示例，请参阅 [TemperatureSensorI2C](https://github.com/HomeSpan/TempSensorI2C)。

#### 可以在 HomeSpan 中添加网页服务器吗？

* 可以，前提是你使用标准 ESP32-Arduino 库（例如 "WebServer.h"）实现网页服务器。请参阅 [ProgrammableHub](https://github.com/HomeSpan/ProgrammableHub)，了解如何轻松地将网页服务器集成到 HomeSpan 中的说明性示例。该项目还涵盖了其他各种高级主题，包括 TCP 插槽管理、附件的动态创建以及在 ESP32 的 NVS 中保存任意数据。

#### 你可以将自定义服务和特征添加到 HomeSpan 吗？

* 是的，HomeSpan 包含两个易于使用的宏来定义你自己的自定义服务和自定义特征，超出 HAP-R2 中指定的那些。有关详细信息和演示如何执行此操作的示例，请参阅 [API 参考](https://github.com/HomeSpan/HomeSpan/blob/master/docs/Reference.md)。请注意，你创建的任何新特征都将被 家庭 应用*完全忽略*。同样，你创建的任何新服务都将显示在“家庭”应用中标有“不支持”的磁贴上。Apple ***不***提供任何机制来扩展“家庭”应用本身的功能。但是，可以使用自定义服务和特征的地方是为这些额外功能设计的第三方应用。例如，*Eve 应用*可以正确处理 HAP-R2 中定义的所有服务和特征，*加上* 明确设计用于 Eve 产品的各种附加服务和特征。如果你知道这些额外服务和特征的 UUID 代码，你可以将它们添加到 HomeKit 并在 Eve 应用中使用它们，就像它们是 HAP-R2 服务和特征一样。

#### HomeSpan 可以用于商业设备吗？

* 不可以，创建商业设备需要 MFi 许可证。HomeSpan 是使用苹果公司的 HAP-R2 规范开发的，苹果公司为 [不会分发或销售的非商业设备] (https://developers.apple.com/homekit/faq/) 提供了该规范。尽管我相信商业规范在功能上是相同的，但 HAP-R2 和 MFi 之间的配对协议存在细微但关键的差异。请注意，当你配对 HomeSpan 设备（或任何基于 HAP-R2 的设备，例如苹果公司的 HAP-R2 ADK、Espressif 的非商业 ADK、HomeBridge 等）时，iPhone 上的 家庭 应用会将设备标记为未经认证，并要求你授予其继续配对的权限。这条关于设备未经认证的警告信息不会出现在商业设备上，大概是因为苹果公司为被许可方提供了 iPhone 可以识别的自定义 MFi 授权码。

#### 为什么“家庭”应用显示门铃服务不受支持？

* 虽然 HAP-R2 中没有记录，但门铃服务似乎被设计为与另一个服务（如 Lock Mechanism）结合使用。如果你添加第二个服务，“家庭”应用将显示相应的磁贴（如锁），门铃是第二个服务。然而，你仍然可以在独立的基础上使用门铃服务-即使“家庭”应用说它不受支持，但按下设备上的按钮将按预期在你的Home Pods上正确触发铃声。

#### 如何从草图中读取当前日期和时间？

* 当你使用 `homeSpan.enableWebLog()` 启用 HomeSpan 网络日志时，HomeSpan 可以自动获取你当地时区的当前日期和时间。请注意，即使不启用网络日志本身，此命令也可用于设置日期和时间（有关如何执行此操作的详细信息，请参阅 [API 参考](Reference.md)）。获取日期和时间后，你可以在需要时使用内置的 Arduino-ESP 32 `getLocalTime(struct tm *info)` 命令来填充当前日期和时间的信息。注释信息采用标准 [standard Unix](https://man7.org/linux/man-pages/man0/time.h.0p.html) `struct tm` 格式，易于解析如下：

```C++
struct tm myTime;        // 创建一个tm结构
getLocalTime(&myTime);   // 用当前日期和时间填充 tm 结构

// 打印 tm 结构的各个元素（有关详细信息，请参阅标准 Unix tm 结构）

Serial.printf("Current Date = %02d/%02d/%04d\n", myTime.tm_mon+1, myTime.tm_mday, myTime.tm_year+1900);
Serial.printf("Current Time = %02d:%02d:%02d\n", myTime.tm_hour, myTime.tm_min, myTime.tm_sec);
```

#### I am getting a "Sketch Too Big" error when I compile

* As a result of increases in the size of the Arduino-ESP32 Board Manager, HomeSpan sketches will no longer fit into the Default partition scheme which only allocates 1.3MB to an App partition.  HomeSpan sketches must instead be compiled under a larger partition scheme, such as Minimal SPIFFS, which provides for 1.9MB partitions.  You can select this partition scheme (or any other that has an App or OTA partition that is large enough to fit your sketch) from with Tools menu in the Arduino IDE.

* Note that it is NOT possible to change the partition scheme on remote devices via OTA.  The device MUST be connected via USB to your computer.  If you change the partition table on the Arduino IDE and upload your sketch via OTA to a remote device, the remote device will IGNORE the request to change the partition table and if the size of the sketch is too large for the previous partition table on the remote device, the new sketch will not be saved.  To solve this, you need to connect the device to your computer via USB, change the partition table, and then upload. Once the partition table is changed, you can then disconnect and use OTA again.
  

---

[↩️](../README.md#resources) 返回欢迎页面
