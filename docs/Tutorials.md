<!--原文时间：2024.2.18，翻译时间：2024.7.10，校对时间：2024.7.12   -->

# HomeSpan 教程

HomeSpan 库包括许多逐渐复杂的教程草图，带你了解 HomeSpan 的所有功能和特征。这些草图带有大量注释，你甚至可以通过阅读所有示例来了解很多关于 HomeKit 本身的知识。如果你已经将 HomeSpan 加载到 Arduino IDE 中，则可以在 *文件→示例→HomeSpan* 下找到教程。每个草图都可以编译并上传到你的 ESP32，这样你就可以看到它们的运行情况。或者你可以通过点击下面的任何标题来探索 GitHub 中的代码。注意：在浏览教程之前，你可能需要先通读 [API 概述](Overview.md)。如果你这样做，它们可能会更有意义！

>[!TIP] 
>每个示例都设计为在将 ESP32 与 HomeKit 配对后进行操作，以便你可以从 iPhone、iPad 或 Mac 上的“家庭”应用控制 HomeSpan。理论上，一旦你配置设备并将其与 HomeKit 配对，你的“家庭”应用应该会在你上传不同的教程时自动更新配置中的所有更改。**然而实际上情况并非总是如此**，因为 HomeKit 有时会缓存有关设备的信息，这意味着你在“家庭”应用中看到的内容可能与你的草图不完全同步。如果发生这种情况，取消配对然后重新配对 ESP32 通常可以解决问题。如果没有，你可能需要重置 ESP32 上的 ID，以便 HomeKit 认为它是新设备并且不会使用任何缓存数据。这很容易做到——请参阅 [命令行界面](CLI.md) 了解详细信息。

### [示例 1 - 简单灯泡](../examples/01-SimpleLightBulb/01-SimpleLightBulb.ino)
第一个示例介绍了 HomeSpan 库，并演示了如何使用 HomeSpan 附件、服务和特征对象的组合来实现简单的开/关灯控制。将此草图上传到你的 HomeSpan 设备并且与你的“家庭”应用配对后，你的 iPhone、iPad 或 Mac 的“家庭”应用中将出现一个新的“灯泡”图块。虽然图块完全可操作（即你可以将灯泡的状态从“开”或“关”更改），但我们尚未将实际的灯或 LED 连接到 HomeSpan 设备，因此不会亮起任何真实的东西。相反，在这个和接下来的几个示例中，我们将专注于了解配置 HomeKit 控件的不同方式。从示例 5 开始，我们将把 LED 连接到设备，并介绍在“家庭”应用中实际打开和关闭 LED 的方法。此示例中涵盖的 HomeSpan API 主题包括：

* 全局对象 `homeSpan`，还有它的 `begin()` 和 `poll()` 方法
* 引用在 `Categories::` 命名空间中定义的 HomeSpan 类别
* 实例化一个新的 `SpanAccessory`
* 实例化在 `Service::` 和 `Characteristic::` 命名空间中定义的 HomeSpan 服务和特征

### [示例 2 - 两个简单灯泡](../examples/02-TwoSimpleLightBulbs/02-TwoSimpleLightBulbs.ino)
示例 2 通过实现两个灯泡来扩展示例 1，每个灯泡都作为自己的附件。

### [示例 3 - 带灯的吊扇](../examples/03-CeilingFanWithLight/03-CeilingFanWithLight.ino)
示例 3 展示了如何将多个服务添加到单个附件中，从而使我们能够创建多功能附件，例如带吸顶灯的吊扇。

### [示例 4 - 高级吊扇](../examples/04-AdvancedCeilingFan/04-AdvancedCeilingFan.ino)
示例 4 通过添加特征来设置风扇速度、风扇旋转方向和灯光亮度，从而扩展了示例 3。此示例中涵盖的新 HomeSpan API 主题包括：

* 使用 `setRange()` 设置特征的允许范围和增量值

### [示例 5 - 工作指示灯](../examples/05-WorkingLED/05-WorkingLED.ino)
示例 5 通过添加实际控制从 HomeKit 连接到 ESP32 的 LED 所需的代码来扩展示例 2。在示例 2 中，我们构建了所有功能，以在 HomeKit 中创建一个显示开/关灯的附件图块，但这些控件实际上并未对 ESP32 进行任何操作。要操作真实设备，需要对 HomeSpan 进行编程，以通过执行某种形式的操作来响应来自 HomeKit 的“更新”请求。此示例中涵盖的新 HomeSpan API 主题包括：

* 从基础 HomeSpan 服务类创建派生的特定于设备的服务结构（类）
* 将派生的服务类放在它们自己的 `\*.h` 文件中以提高可读性和可移植性
* 为你的派生服务实现虚拟 `update()` 方法
* 使用 `SpanCharacteristic *` 指针保存对特征对象的引用
* 使用 `getVal()` 和 `getNewVal()` 方法检索当前和新的特征值

### [示例 6 - 可调光指示灯](../examples/06-DimmableLED/06-DimmableLED.ino)
示例 6 更改了示例 5，使得 LED #2 现在可以调光，而不仅仅是开/关。此示例中涵盖的新 HomeSpan API 主题包括：

* 实现脉宽调制 (PWM) 以通过实例化 `LedPin()` 对象来控制连接到 ESP32 引脚的 LED
* 使用 LedPin `set()` 方法设置 LED 的亮度
* 将类似的派生服务类存储在同一个 \*.h 文件中以便于使用

### [示例 7 - 附件名称](../examples/07-AccessoryNames/07-AccessoryNames.ino)
示例 7 演示了如何将示例 6 中创建的两个 LED 配件的名称从“家庭”应用生成的默认值更改为更实用和自定义的名称。

### [示例 8 - 桥接](../examples/08-Bridges/08-Bridges.ino)
示例 8 在功能上与示例 7 相同，只是我们没有定义两个附件（一个用于开/关 LED，一个用于可调光 LED ），而是定义了三个附件，其中第一个充当 HomeKit 桥接器。

### [示例 9 - 消息日志](../examples/09-MessageLogging/09-MessageLogging.ino)
示例 9 说明了如何将日志消息添加到 HomeSpan 草图。除了包含新的日志消息外，该代码与示例 8 相同。此示例中涵盖的新 HomeSpan API 主题包括：

* 使用 `LOG0()`、`LOG1()` 和 `LOG2()` 宏来为不同的日志级别创建日志消息
* 使用 `homeSpan.setLogLevel()` 方法设置草图的初始日志级别

### [示例 10 - 全彩 LED](../examples/10-RGB_LED/10-RGB_LED.ino)
示例 10 说明了如何控制全彩 LED 来设置任何颜色和亮度。此示例中涵盖的新 HomeSpan API 主题包括：

* 使用 `PwmPin::HSVtoRGB()` 将 HomeKit 的色相/饱和度/亮度转换为红/绿/蓝
* 使用 `getVal()` 的可选模板功能，例如 `getVal<float>()`

### [示例 11 - 服务名称](../examples/11-ServiceNames/11-ServiceNames.ino)
示例 11 演示了如何将多服务附件中不同服务的名称从“家庭”应用生成的默认值更改为更实用和自定义的名称。这些示例还探讨了“家庭”应用显示这些名称的方式和时间，以及当设备配置为桥接时“家庭”应用如何为附件图块选择适当的图标。

### [示例 12 - 服务循环](../examples/12-ServiceLoops/12-ServiceLoops.ino)
示例 12 引入了 HomeKit **事件通知**来实现两个新附件——温度传感器和空气质量传感器。当然，出于本示例的目的，我们实际上不会将这些物理设备连接到 ESP32，但我们将模拟定期“读取”它们的属性，并通知 HomeKit 更新的值。此示例中涵盖的新 HomeSpan API 主题包括：

* 在派生服务中实现虚拟 `loop()` 方法
* 使用 `timeVal()` 方法跟踪自上次更新特征以来经过的时间
* 使用 `setVal()` 方法设置特征值并触发事件通知

### [示例 13 - 目标状态](../examples/13-TargetStates/13-TargetStates.ino)
示例 13 通过实现两个新服务（车库门开启器和电动窗帘）演示了同时使用和方法。这两个示例都展示了 HomeKit 的目标状态/当前状态框架。本示例涵盖的新 HomeSpan API 主题包括：`update()``loop()`

* 使用枚举常量来设置表示离散状态的特征值（例如“提升”、“关闭”）

### [示例 14 - 模拟按钮](../examples/14-EmulatedPushButtons/14-EmulatedPushButtons.ino)
示例 14 演示了如何使用服务的 `loop()` 方法中的 `setVal()` 和 `timeVal()` 方法在“家庭”应用中创建一个模拟按钮开关的图块。在此示例中，按下“家庭”应用中的图块将使其打开，LED 闪烁 3 次，然后关闭（就像真正的按钮可能会做的那样）。

### [示例 15 - 真实按钮](../examples/15-RealPushButtons/15-RealPushButtons.ino)
此示例介绍了 HomeSpan 功能，可让你轻松地将真正的按钮连接到 ESP32 上的任何引脚。然后，这些按钮可用于手动控制连接到设备的任何设备，例如灯或风扇。在此示例中，我们实现了 3 个按钮来控制 LED 的开关、亮度和“收藏”设置，使用单击、双击和长按按钮的组合。每次按下按钮后，使用 `setVal()` 方法将事件通知发送回 HomeKit，以便“家庭”应用图块立即反映你对 LED 功率和亮度的手动更改。此示例中涵盖的新 HomeSpan API 主题包括：

* 使用 `SpanButton()` 在任何 ESP32 引脚上创建按钮对象
* 在派生服务中实现虚拟 `button()` 方法
* 解析单击、双击和长按按钮按下

### [示例 16 - 可编程开关](../examples/16-ProgrammableSwitches/16-ProgrammableSwitches.ino)
示例 16 没有引入任何新的 HomeSpan 功能，而是展示了 HomeKit 的一个独特功能，你可以使用 HomeSpan 轻松访问该功能。在之前的所有示例中，我们使用 ESP32 来控制本地设备——直接连接到 ESP32 的设备。然后我们了解了如何通过 iOS 或 MacOS 的 HomeKit “家庭”应用或通过添加直接连接到 ESP32 的本地按钮来控制设备。在这个例子中，我们做相反的事情，并使用连接到 ESP32 的按钮来控制任何类型的其他 HomeKit 设备。为此，我们使用 HomeKit 的可编程开关。

### [示例 17 - 链接服务](../examples/17-LinkedServices/17-LinkedServices.ino)
示例 17 介绍了链接服务的 HAP 概念，并演示了如何通过实现多头淋浴来使用它们。此示例还说明了一些不同的编码风格，展示了 HomeSpan 的 C++ *基于结构*设计范例的强大功能和灵活性。此示例中涵盖的新 HomeSpan API 主题包括：

* 使用 `addLink()` 方法创建链接服务

### [示例 18 - 保存状态](../examples/18-SavingStatus/18-SavingStatus.ino)
示例 18 演示了通过两个可调光 LED 的实现，如何将特征值自动保存在非易失性存储器 (NVS) 中，以便在设备断电时在启动时恢复。此示例中涵盖的新 HomeSpan API 主题包括：

* 通过将构造函数的第二个参数设置为 `true` 来指示 HomeSpan 在 NVS 中存储特征的值

### [示例 19 - 网页日志](../examples/19-WebLog/19-WebLog.ino)
示例 19 通过实现两个可开关 LED 说明如何将网页日志添加到 HomeSpan 草图中，如何使用 NTP 时间服务器同步设备的内部时钟，以及如何创建自己的网页日志消息。此示例中涵盖的新 HomeSpan API 主题包括：
* 启用 HomeSpan 网页日志并使用 `homeSpan.enableWebLog()` 方法指定可选的 NTP 时间服务器
* 使用`WEBLOG()`宏创建网页日志消息

### [示例 20 - 高级技术](../examples/20-AdvancedTechniques/20-AdvancedTechniques.ino)
示例 20 通过实现**动态**桥接说明了许多高级技术，该桥接允许随时**交互地**添加和删除灯光配件，而无需重新启动设备。此示例中涵盖的新 HomeSpan API 主题包括:

* 使用 `SpanUserCommand()` 创建自定义 CLI 命令
* 使用 `homeSpan.deleteAccessory()` 动态删除附件
* 使用 `homeSpan.updateDatabase()` 刷新附件数据库（自动更新“家庭”应用）
* 使用 `homeSpan.autoPoll()` 在后台（以及在第二个内核上，如果可用）实现 HomeSpan 轮询

### [示例 21 - 配件标识符](../examples/21-AccessoryIdentifier/21-AccessoryIdentifier.ino)
示例 21 展示了如何使用每个配件所需的 AccessoryInformation 服务中始终存在的标识符特征来创建自定义“识别例程”，该例程可在配对设备时从“家庭”应用中触发。此示例未使用任何新的 HomeSpan 方法。

### [示例 22 - TLV8 特征](../examples/22-TLV8_Characteristics/22-TLV8_Characteristics.ino)
示例 22 演示了如何通过实现 DisplayOrder 特征来创建和利用基于 TLV8 的特征，该特征用于设置电视服务输入源在“家庭”应用中的显示顺序。此示例中涵盖的新 HomeSpan API 主题包括：

* 使用 HomeSpan 的 TLV8 类创建 TLV8 对象
* 使用 `setTLV()` 更新 TLV8 特征

## 其他示例

以下示例展示了 HomeSpan 文档的不同部分中引用的各种 HomeSpan 和 HomeKit 功能。这些草图可以在 Arduino IDE 中的 *文件→示例→HomeSpan→其他示例* 下找到

### [台灯](../examples/Other%20Examples/TableLamp/TableLamp.ino)
台灯配件的基本实现。用作 [API 概述](Overview.md) 中的教程

### [控制远程](../examples/Other%20Examples/RemoteControl/RemoteControl.ino)
一个独立的示例，展示了如何使用 HomeSpan 的 *RFControl* 类来生成自定义脉冲序列。出于说明目的，脉冲宽度非常长，适合输出到 LED，因此你可以“看到”脉冲序列。请参阅 [射频控制](RMT.md) 了解完整详情

### [控制舵机](../examples/Other%20Examples/ServoControl/ServoControl.ino)
窗帘的实现，它使用 HomeSpan 的 *ServoPin* 类来控制窗条的水平倾斜。有关完整详细信息，请参阅 [ServoPin](PWM.md#servopin)

### [电视](../examples/Other%20Examples/Television/Television.ino)
HomeKit *未认证*电视服务的示例，展示了如何使用不同的特征来控制电视的电源、输入源和其他一些功能。请参阅 [电视服务](TVServices.md) 了解完整详情

### [灯带](../examples/Other%20Examples/Pixel/Pixel.ino)
演示如何使用 HomeSpan 的 *Pixel* 和 *Dot* 类来控制单线和两线可寻址全彩和 RGB/W/C LED。有关完整详细信息，请参阅 [灯带](Pixels.md) 

### [像素测试仪](../examples/Other%20Examples/PixelTester/PixelTester.ino)
帮助确定任何 RGB(W) LED 灯带的 *pixelType* 的草图。请参阅 [灯带](Pixels.md) 页面了解完整详情

### [Pixel-RGBWC](../examples/Other%20Examples/Pixel-RGBWC)
附加 Pixel 示例，演示如何实现 RGBWC Pixel 灯带，并为 RGB 和 WC LED 设置单独的家庭应用控件

### [自定义服务](../examples/Other%20Examples/CustomService/CustomService.ino)
演示如何在 HomeSpan 中创建自定义服务和自定义特征，以实现 *Eve 应用*识别的大气压力传感器。有关完整详细信息，请参阅 [自定义特征和自定义服务宏](Reference.md#custom)

### [可编程集线器](../examples/Other%20Examples/ProgrammableHub/ProgrammableHub.ino)
演示如何实现一个完全可编程的灯光配件集线器，允许用户直接通过设备托管的**网页界面**或通过 HomeSpan 的**命令行界面动态**添加或者删除多达 12 个灯光配件。每个灯都可以配置为可调光/非可调光，没有颜色控制、全彩颜色控制或色温控制。建立在许多使用的技术之上 [示例 20](../examples/20-AdvancedTechniques/20-AdvancedTechniques.ino)

### [远程传感器](../examples/Other%20Examples/RemoteSensors)
演示如何使用 SpanPoint 将消息从运行测量本地温度的轻量级草图的电池供电远程从设备传输到运行实现温度传感器附件的完整 HomeSpan 草图的电源供电主设备上。请参阅 [Span 热点](NOW.md) 有关 *SpanPoint* 类及其所有方法的完整详细信息

### [呼吸指示灯](../examples/Other%20Examples/FadingLED/FadingLED.ino)
演示 *LedPin* 类如何使用 ESP32 的内置淡入淡出控件，在指定的时间段内自动将 LED 从一个亮度级别淡入淡出另一个亮度级别。有关完整详细信息，请参阅 [LedPin](PWM.md#ledpin)

### [电动窗帘](../examples/Other%20Examples/MotorizedWindowShade/MotorizedWindowShade.ino)
演示如何使用 *StepperControl* 类操作步进电机。实现基于上述 [示例 13](../examples/13-TargetStates/13-TargetStates.ino) 的电动窗帘。有关完整详细信息，请参阅 [控制步进电机](Stepper.md)

### [自定义分区](../examples/Other%20Examples/CustomNVSPartition/CustomNVSPartition.ino)<a name="customnvspartition"></a>
演示如何通过将 *Partition.csv* 文件添加到草图文件夹来为草图创建自定义分区方案。可用于扩展非易失性存储（NVS）分区的大小，在创建具有许多附件的 HomeSpan 设备时可能需要该分区，当你想将这些附件的特征保存在 NVS 中时

### [外部引用](../examples/Other%20Examples/ExternalReference/ExternalReference.ino)
演示如何从这些服务外部访问服务的特征（例如在 Arduino 的 `loop()` 中访问）。在此草图中，我们重新创建了示例 5 中的两个 LED，并在 Arduino `loop()` 部分添加一个新功能，用于检查两个 LED 是否同时打开，如果是，则自动关闭

### [多线程](../examples/Other%20Examples/MultiThreading/MultiThreading.ino)
演示如何在多线程环境中使用 HomeSpan。在此草图中，我们重新创建了示例 1 中的简单灯泡的多线程版本，但不是从主 Arduino `loop()` 调用 `poll()`，而是实现了每隔 10 秒休眠一次的逻辑（不会干扰 HomeSpan！），然后根据当前设置自动将灯泡的电源从 ON 切换为 OFF，或从 OFF 切换为 ON

---

[↩️](../README.md#resources) 返回欢迎页面
