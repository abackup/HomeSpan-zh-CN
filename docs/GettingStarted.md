<!--  原文时间：2023.7.11, 翻译时间：2024.5.10，校对时间：2024.7.12   -->

# HomeSpan 入门

本页提供了获取和设置开发 HomeSpan 设备所需的所有软件和硬件的分步说明。那些有 Arduino 或 ESP32 经验的人可能会发现你们可以跳过一些步骤。

#### 第 1 步：安装 Arduino IDE

由于 HomeSpan 是为 Arduino 集成开发环境设计的库，因此第一步是从 [Arduino 官网](https://www.arduino.cc/en/software) 下载并安装此 IDE 的最新版本。如果你有 Arduino 草图编程经验，你会发现 Homespan 对标准 Arduino `setup( )` 和 `loop( )` 函数的使用，以及非常熟悉一直存在的 *\*.ino* 文件。如果你从未编写过 Arduino，你可能希望首先查看 [Arduino's IDE 指南](https://www.arduino.cc/en/Guide/Environment)（并尝试一些 Arduino 示例）再继续使用 HomeSpan 进行编程。

#### 第 2 步：安装 Arduino-ESP32 内核

Arduino IDE 带有内置编译器，并支持各种 Arduino 板，例如 Arduino Uno 和 Arduino Nano。但是 IDE 本身并不支持对 ESP32 进行编程。幸运的是，IDE 允许你为其他微控制器（包括 ESP32 ）下载和安装**内核**，以使它们可以在 Arduino 环境中进行编程。有关如何将 Arduino-ESP32 内核安装到使用 [Arduino IDE 开发板管理](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md) 的 Arduino IDE 中，请参阅 [ESP32 开发板管理](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)。

#### 第 3 步：安装 HomeSpan

HomeSpan 打包为标准 Arduino 库，可以从 Arduino IDE 自动安装，也可以通过 GitHub 手动安装。

* 自动安装，请从 Arduino IDE 中打开 Arduino 库管理，方法是从 Arduino 单栏中选择 *草图→包含库→管理库...*。然后，在搜索框中输入 *HomeSpan*，从下拉框中选择最新版本，然后单击`安装`按钮。Arduino 库管理将自动下载并安装你选择的版本。HomeSpan 和所有教程示例草图也都可以使用了。


* 手动安装，请先将 *Source Code(zip)* 的 [最新版本](https://github.com/HomeSpan/HomeSpan/releases/latest) 下载到你的桌面。如果你的操作系统将包保存为 zip 格式或者在保存到你的桌面时自动解压缩，无关紧要。接下来，从 Arduino IDE 的顶部菜单栏中，选择 *草图→包含库→添加 .ZIP 库...* 并导航到桌面文件夹，你应该在该文件夹中看到刚刚下载的 HomeSpan 包（ zip 文件，或文件夹）。选择包（不要打开它），然后单击`选择`。这表示 Arduino IDE 要已将 HomeSpan 包复制到其库子文件夹中。HomeSpan 现在可以用作标准 Arduino 库（你现在可以从桌面删除 HomeSpan 包，因为它不再需要）。

#### 第 4 步：获取 ESP32 开发板

ESP32 开发板在大多数电子爱好网站上提都供不同配置的 ESP32 开发板，例如 [Adafruit](https://www.adafruit.com) 和 [Sparkfun](https://www.sparkfun.com)。HomeSpan 可与 [Arduino-ESP32 内核](https://docs.espressif.com/projects/arduino-esp32/en/latest/getting_started.html#supported-soc-s) 支持的任何 ESP32 配合使用，包括原版的 ESP32、ESP32-S2、ESP32-S3 以及 ESP32-C3。


#### 第 5 步：安装所需的 USB 驱动程序


对 ESP32 板进行编程通常通过 USB 串口完成。大多数 ESP32 开发板都包含一个专用芯片，该 ESP32 使用的串口 UART 信号转换为可以在电脑之间传输的 USB 信号，通常通过标准 USB 数据线传输。根据 ESP32 上使用的他不同的 USB-UART 芯片，你可能需要安装单独的驱动程序。你的主板制造商可能会提供有关如何下载和安装操作系统所需的驱动程序的说明。

#### 第 6 步：编程你的第一个 HomeSpan 设备

在继续之前，你可能需要先通读 [概述](Overview.md)。或者你可以直接进入并使用 HomeSpan 的内置教程草图之一对你的设备进行编程：

* 将 ESP32 连接到电脑上的空闲 USB 端口

* 启动 Arduino IDE

* 加载 HomeSpan 教程草图之一（例如，*文件→示例→HomeSpan→01-SimpleLightBulb*）

* 设置开发板参数以匹配你的 ESP32（例如，*工具→开发板→ESP32 Arduino→Adafruit ESP32 Feather* ）

* 打开 Arduino 串口监视器（*工具→串口监视器*）

* 编译并上传 HomeSpan 草图（*草图→上传*）

编译和上传完成后，HomeSpan 将向 Arduino 串口监视器报告各种初始诊断消息，之后报告已准备就绪。那么恭喜！你刚刚成功对你的第一个 HomeSpan 设备进行了编程。

#### 第 7 步：配置和配对 HomeSpan 设备

尽管该设备现在已编程并完全可操作，但需要先配置 WiFi 凭据和 HomeKit 设置代码，然后才能与你的 HomeKit 账户配对。有关如何通过 Arduino 串口监视器配置 HomeSpan 的详细信息，请参阅 [命令行界面](CLI.md)。一旦设备连接到你的家庭 WiFi，HomeSpan 现在就可以通过 iPhone 上的“家庭”应用进行配对了！


## 添加控制按钮和状态指示灯（*可选*）

除了能够通过 Arduino 串口监视器使用 [命令行界面](CLI.md) 配置 HomeSpan 设备外，HomeSpan 还为最终用户提供了一种替代方法来配置未连接到电脑的独立 HomeSpan 设备。此方法需要安装两个外部组件：

1. 一个常开按钮，用作 HomeSpan 的控制按钮，以及
1. 一个 LED（带有限流电阻）用作表示 HomeSpan 状态的 LED。

控制按钮应安装在接地和 ESP32 上任何可用作输入的引脚之间。要告知 HomeSpan 你选择了哪个引脚，你必须在你的草图顶部附近调用方法 `homeSpan.setControlPin(pin)`(有关详细信息，请参阅 [API 参考](Reference.md))，否则 HomeSpan 将默认**未**安装控制按钮。

同样，状态指示灯可以连接到 ESP32 上任何可用作输出的引脚（并通过适当大小的限流电阻接地）。要告知 HomeSpan 你选择了哪个引脚，你必须在草图顶部附近调用方法 `homeSpan.setStatusPin(pin)`，否则 HomeSpan 将默认**未**安装状态指示灯。请注意，一些 ESP32 板有一个内置 LED - 如果它**不是**需要特殊驱动器的可寻址全彩 LED，而是一个简单的开关 LED，则可以将其用于状态指示灯。


使用控制按钮和状态指示灯来配置独立的 HomeSpan 设备，包括启动 HomeSpan 的临时 WiFi 网络以配置设备的 WiFi 凭据和 HomeKit 设置代码，在 [用户指南](UserGuide.md) 中有详细说明。


## 接下来是什么？

如果你还没有通读 [API 概述](Overview.md)，你现在应该阅读，以便你对开发 HomeSpan 草图的框架有一个很好的理解。

接下来，探索教程草图，上传几个试试，看看它们是如何工作的。这些示例从简单开始逐渐复杂化，带你了解 HomeSpan 的所有功能和特征。在此过程中，你还将学习很多 HomeKit 提示和技巧。有关所有包含示例的摘要，请参阅 [教程](Tutorials.md)。在草图中找到你不理解的东西？ 有关所有 HomeSpan 对象、函数和方法的详细信息，请访问 [API 参考](Reference.md)。有更一般的问题吗？ 查看是否已出现在 [常见问题解答](FAQ.md) 页面或任何 [Disussion](https://github.com/HomeSpan/HomeSpan/discussions) 或 [Issues](https://github.com/HomeSpan/HomeSpan/issues) 页面。如果没有，请随时通过添加新问题来加入讨论。

准备好开始创建自己的 HomeSpan 草图了吗？ 查看 [服务和特征](ServiceList.md) 以获取 HomeSpan 支持的所有 HomeKit 服务和特征的完整列表，以及 [配件类别](Categories.md) 以获取所有支持的 HomeKit 类别列表。并且不要忘记使用 [苹果 (HAP-R2)](../master/docs/HAP-R2.pdf) 作为你对每项服务和详细信息的参考特征。

在开发你的草图时，请记住使用 Arduino 串口监视器。HomeSpan 生成广泛的调试信息，可帮助你调试草图并监控 HomeSpan 设备的各个方面。你还可以使用 [命令行界面](CLI.md) 从串口监视器控制 HomeSpan 的各个方面，包括配置设备的 WiFi 凭据和 HomeKit 设置代码。

要访问更高级的功能，请查看 [PWM](PWM.md)、 [射频控制](RMT.md) 和 [灯带](Pixels.md) 以获取关于如何使用 HomeSpan 通过 PWM 信号控制 LED、灯和步进电机；生成射频和红外遥控代码来操作电器和电视；甚至控制全彩 LED 灯带的教程和详细信息。

最后，断开 HomeSpan 设备与电脑的连接，并直接从电源适配器为其供电。毕竟 HomeSpan 设备旨在独立运行连接到现实世界的设备，如灯、风扇、门锁和窗帘。有关如何**无需将设备连接到电脑**就可操作和配置独立 HomeSpan 设备的 WiFi 凭据和 HomeKit 设置代码，请参阅 [用户指南](UserGuide.md)。

---

[↩️](../README.md#resources) 返回欢迎页面
