 <!--原文时间：2024.7.4，翻译时间：2024.7.12，校对时间：2024.7.12；文件版本号：1.9.1 -->

个人翻译的 [HomeSpan](https://github.com/HomeSpan/HomeSpan)（家庭空间），喜欢的请去 [原地址](https://github.com/HomeSpan/HomeSpan) 查看。

# 欢迎！


欢迎使用 HomeSpan - 一个强大且极易使用的 Arduino 库，用于完全在 [Arduino IDE](http://www.arduino.cc) 内创建你自己的基于 [ESP32](https://www.espressif.com/en/products/modules/esp32) 的 HomeKit 设备。

HomeSpan 提供以微控制器为中心的苹果 HomeKit 配件协议规范版本 R2 ([HAP-R2](../master/docs/HAP-R2.pdf)) 的实现，专门为在 Arduino IDE 中运行的乐鑫 ESP32 微控制器而设计。HomeSpan 通过家庭 WiFi 直接与 HomeKit 配对，无需任何外部网桥或组件。通过 HomeSpan，你可以使用 ESP32 I/O 的全部功能来创建自定义控制软件和/或硬件，以通过 iPhone、iPad 或 Mac 上的“家庭”应用或 Siri 自动操作外部设备。

HomeSpan 需要 [Arduino-ESP32 开发板管理](https://github.com/espressif/arduino-esp32)  的 2 版本。HomeSpan 可以在原版的乐鑫 ESP32 以及 ESP32-S2、ESP32-C3 和 ESP32-S3 芯片上运行。

HomeSpan 目前与 Arduino-ESP32 开发板管理的 3.X 版本不兼容，因为版本 3 包含许多重大更改，并且不向后兼容 Arduino-ESP32开发板管理的版本 2.X。目前 HomeSpan 只能在开发板管理的 2.X 版本下编译。

> [!NOTE] 
苹果公司的新 HomeKit 架构 [需要使用家庭中心](https://support.apple.com/en-us/HT207057)（HomePod 或 Apple TV）才可用于任何 HomeKit 设备的完整地和正确地操作，包括基于 HomeSpan 的设备。如果没有家庭中心，HomeSpan 将无法向“家庭”应用发送通知——按钮和温度传感器等将无法向“家庭”应用发送更新。不建议在没有家庭中心的情况下使用 HomeSpan。

### HomeSpan 亮点

* 提供自然、直观且**非常**易于使用的框架
* 利用独特*以服务为中心*的方法创建 HomeKit 设备
* 充分利用广受欢迎的 Arduino IDE
* 100% 符合 HAP-R2 标准
* 数十种集成 HomeKit 服务
* 在附件模式或桥接模式下运行
* 支持与设置码或二维码配对

### 对于 HomeSpan 开发人员

* Arduino 串口监视器的广泛使用
  * 实时、易于理解的诊断
  * 对每个底层 HomeKit 操作、数据请求和数据响应完全透明
  * 具有各种信息、调试和配置命令的命令行界面
* 内置数据库验证，确保你的配置满足所有 HAP 要求
* 利用 ESP32 的 16 通道 PWM 外设的专用类，可轻松控制：
  * LED 亮度（包括自动淡化）
  * 伺服电机
* 集成按钮和拨动开关功能，支持单击、双击和长按：
  * 将 ESP32 引脚连接到地或 VCC 的物理按钮
  * 连接到 ESP32 引脚的触摸板/传感器（用于支持触摸板的 ESP32 设备）
* 集成访问 ESP32 的片上远程控制外设，可轻松生成红外和射频信号
* 控制单线和双线可寻址 RGB 和 RGBW LED 和 LED 灯带的专用类
* 专门用于控制步进电机的类，可以在后台平稳运行，而不会干扰 HomeSpan 设备
* 使用 ESP-NOW 在 ESP32 设备之间实现无缝点对点通信的专用类
* 用于用户定义的日志消息的集成网络日志
* 大量注释的教程草图将带你从 HomeSpan 的基础知识到高级 HomeKit 主题
* 展示 HomeSpan 实际实现的其他示例和项目
* 一套完整的文档，全面解释了 HomeSpan 的各种 API 

### 对于 HomeSpan 最终用户

* 嵌入式 WiFi 接入点和网络界面，允许最终用户（非开发人员）：
  * 使用自己的家庭 WiFi 凭据设置 HomeSpan
  * 创建自己的 HomeKit 配对设置代码
* 状态指示灯和控制按钮，允许最终用户：
  * 强制解除设备与 HomeKit 的配对
  * 执行恢复出厂设置
  * 启动 WiFi 接入点
* 独立、详细的最终用户指南

[HomeSpan 更新列表](docs/updatelist.md)

# HomeSpan 资源<a name="resources"></a>

HomeSpan 包括以下文档：

* [HomeSpan 入门](docs/GettingStarted.md) - 设置开发 HomeSpan 设备所需的软件和硬件
* [HomeSpan API 概述](docs/Overview.md) - HomeSpan API 概述，包括开发第一个 HomeSpan 草图的分步指南
* [HomeSpan 教程](docs/Tutorials.md) - HomeSpan 的草图指南
* [HomeSpan 服务和特征](docs/ServiceList.md) - HomeSpan 支持的所有 HAP 服务和特征的列表
* [HomeSpan 配件类别](docs/Categories.md) - HomeSpan 定义的所有 HAP 配件类别的列表
* [HomeSpan 命令行界面](docs/CLI.md) - 配置 HomeSpan 设备的 WiFi 凭据，修改其 HomeKit 设置代码，监控和更新其状态，并从 Arduino IDE 串口监视器访问详细的实时设备诊断
* [HomeSpan 用户指南](docs/UserGuide.md) - 有关如何配置已编程的 HomeSpan 设备的 WiFi 凭据、修改其 HomeKit 设置代码以及将设备与 HomeKit 配对的交钥匙式说明。不需要电脑！
* [HomeSpan API 参考](docs/Reference.md) - HomeSpan 库 API 的完整指南
* [HomeSpan 二维码](docs/QRCodes.md) - 创建并使用二维码来配对 HomeSpan 设备
* [HomeSpan OTA](docs/OTA.md) - 直接从 Arduino IDE 无线更新草图，无需串口连接
* [HomeSpan PWM](docs/PWM.md) - 使用 ESP32 的片载 PWM 外设集成控制标准 LED 和伺服电机
* [HomeSpan 射频控制](docs/RMT.md) - 使用 ESP32 的片载 RMT 外设，轻松生成射频和红外遥控信号
* [HomeSpan 灯带](docs/Pixels.md) - 可寻址的单线和双线 RGB 和 RGBW LED 和 LED 灯带的集成控制
* [HomeSpan 控制步进电机](docs/Stepper.md) - 步进电机集成控制，包括 PWM 微步进
* [HomeSpan Span 热点](docs/NOW.md) - 使用 ESP-NOW 促进 ESP32 设备之间的点对点双向通信
* [HomeSpan 电视服务](docs/TVServices.md) - 如何使用 HomeKit 未记录的电视服务和特征
* [HomeSpan 消息记录](docs/Logging.md) - 如何生成在 Arduino 串口监视器上显示的日志消息，以及可选地发布到集成的网络日志页面
* [HomeSpan TLV8 特征](docs/TLV8.md) - 用于创建 TLV8 对象以与基于 TLV8 的特征一起使用的类和方法
* [HomeSpan 设备克隆](docs/Cloning.md) - 将损坏的设备无缝更换为新设备，而无需重新配对和丢失 HomeKit 自动化
* [HomeSpan 项目](https://github.com/topics/homespan) - HomePan 库的实际应用
* [HomeSpan 常见问题解答](docs/FAQ.md) - 常见问题解答
* [常见问题的解决方案](docs/Solutions.md) - 解决使用/编译 HomeSpan 时的一些常见问题
* [HomeSpan 推荐草图](https://github.com/abackup/HomeSpanReferenceSketches-Chinese) - 一系列独立的参考草图，展示了一些更复杂的 HomeKit 服务，如恒温器和灌溉系统

请注意，所有文档都是受版本控制的，并与每个分支相关联。*master* 分支通常指向最新版本。当可用时，该 *dev* 分支将包含正在开发的代码。

# 外部资源

除了 HomeSpan 资源之外，刚接触 HomeKit 编程的开发人员可能会发现苹果公司的 HomeKit 配件协议规范非商业版本 R2 ([HAP-R2](../master/docs/HAP-R2.pdf)) 的第 8 章和第 9 章非常有用。不幸的是，苹果公司不再提供这份文件（也许是因为它最后一次更新是在 2019 年 7 月，现在已经有些过时了）。但是，你可以在网络上的其他地方找到此文档的副本。请注意，苹果并未将 HAP-R2 文档替换为用于非商业用途的任何其他版本，并且苹果公司的开源 [HomeKit ADK](https://github.com/apple/HomeKitADK) 仅反映了原始的 HAP-R2 规格（而不是用于商业设备的 HomeKit 中提供的所有最新服务和特征）。

 --- 

### 反馈或问题？

请将 HomeSpan 的错误报告或其他问题发布到 [Issues](https://github.com/HomeSpan/HomeSpan/issues) 页面。请将有关 HomeSpan 的所有其他问题（使用、功能、规格、示例等）或你对新功能的任何想法、建议或有关 HomeSpan 或 HomeKit 的任何反馈发布到 [Discussion](https://github.com/HomeSpan/HomeSpan/discussions) 页面。对于与特定 HomeSpan 问题或与功能无关的更一般的问题或反馈，你可以直接 [homespan@icloud.com](mailto:homespan@icloud.com) 给我发电子邮件。

### 关于作者

HomeSpan 由 Gregg Berman 开发并继续维护和支持。它最初是为了解决无法用 Siri 操作射频控制的厨房通风罩的麻烦问题。我希望你会发现它很有用，而且使用起来很有趣。

这是我的第二个大型开源项目——我的第一个项目是设计一个开源系统，用于操作模型铁路，仅使用 Arduino Uno 和 Arduino Motor Shield 来生成数字命令和控制 (DCC) 信号。虽然我已经多年没有参与模型铁路爱好的建设，但展示我的原始系统（称为 DCC++）的视频，以及它如何工作的详细教程，仍然可以在 [DCC++ YouTube 频道](https://www.youtube.com/@dcc2840/videos) 上找到。
