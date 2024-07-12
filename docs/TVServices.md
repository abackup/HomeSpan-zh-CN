<!--  原文时间：2024.7.4，翻译时间：2024.7.7，校对时间：2024.7.12  -->

# 电视服务和特点

HomeSpan 包括许多不属于 [HAP-R2](../master/docs/HAP-R2.pdf) 的未记录的电视服务和特征。尽管更广泛的 HomeKit 社区已经确定了每个电视服务和特性的 UUID 和规范，但只有通过反复试验才能确定正确的使用方法。本页记录了使用 iOS 15.1 中提供的 家庭 应用在 HomeSpan 中试验不同电视服务和特性的结果。本文档应该被认为是可靠的，但苹果当然可以随时更改“家庭”应用中此类服务和特征的行为，而无通知。

### `Category::Television`

使用 `Category::Television` 作为 `homeSpan.begin()` 中的类别来指定电视配件。这会导致“家庭”应用在附件列表中显示电视图标。然而，这似乎只适用于第一个实施的附件。如果你创建具有多个电视配件的设备，或将电视配件放置在桥接配件后面，则电视的图标将转换为通用 HomeKit 符号。

### `Service::Television()`

使用 `Service::Television()` 作为服务来创建电视配件图块。它支持两个主要特征：

* `Characteristic::Active()` - 这个 HAP-R2 标准特征用于打开或关闭电视。这是必需的特征

* `Characteristic::ConfiguredName()` - 这是 `Characteristic::Name()` 的可选电视特定版本，似乎是设置电视默认名称的唯一方法。与所有其他 HomeKit 服务不同，“家庭”应用在与 `Service::Television()` 一起使用时会忽略使用 `Characeteristic::Name()` 指定的任何名称

基于上述内容，以下代码片段定义了一个带有基本开/关开关的简单电视配件：

```C++
new Service::Television();
  new Characteristic::Active(0);                    // set power to OFF at start-up
  new Characteristic::ConfiguredName("Sony TV");    // optional Characteristic to set name of TV
```
可以通过其他两个可选特性启用对电视的更高级控制：

* `Characteristic::RemoteKey()` - 这个只写的数字特性使 HomeSpan 能够从位于控制中心下的 iPhone 上的远程控制小部件中读取按钮按下。这个小部件通常用于控制 Apple TV，但似乎上面创建的任何电视配件也可以通过远程控制小部件进行操作。小部件的布局（不能修改）包括 4 个箭头、一个中央选择按钮、一个播放/暂停按钮、一个大的“后退”按钮和一个“信息”按钮。当按下“键”时，“家庭”应用会向 `Characteristic::RemoteKey()` 发送更新，HomeSpan 可以使用通常的 `update()` 方法读取该更新。值如下： 
  * 4 = 向上箭头
  * 5 = 向下箭头
  * 6 = 左箭头
  * 7 = 右箭头
  * 8 = 中心选择按钮
  * 9 = 后退按钮
  * 11 = 播放/暂停按钮
  * 15 = 信息按钮
  
* `Characteristic::PowerModeSelection()` - 这个只写的特性导致文本“查看电视设置”出现在电视配件设置页面下的“家庭”应用中。当按下此文本时，“家庭”应用会向 `Characteristic::PowerModeSelection()` 发送一个值为 0 的更新，HomeSpan 可以使用通常的 `update()` 方法读取该更新
* `Characteristic::ActiveIdentifier()` - 此数字 Characteristic 用于控制电视的输入源（例如 HDMI-1、HDMI-2、Netflix 等）。它仅在使用 `Service::InputSource()` 定义和链接输入源时使用（见下文），在这种情况下它是*必需*特性。
* `Characteristic::DisplayOrder()` - 此 TLV8 特性用于控制链接的输入源在家庭应用程序中的显示顺序
  * 如果没有指定此特性的顺序，“家庭”应用将在选择部分（电源按钮下方）以随机顺序显示输入源，并根据每个输入源的数字标识符在附件的设置页面上以数字顺序显示输入源
  * 此特征使用的 TLV8 对象的格式是一系列 TLV8“标识符”记录，其中 TAG=1 且 VALUE 设置为特定输入源的标识符;每个“标识符”记录都应由一个空的 TLV8 记录分隔，TAG=0
  * 例如，以下代码片段将标识符为 10、20 和 30 的三个输入源的显示顺序设置为 20、30 和 10：

```C++
TLV8 orderTLV;             // 创建一个名为“orderTLV”的空 TLV8 对象
orderTLV.add(1,20);        // TAG=1, VALUE=20（要显示的第一个输入源的标识符）
orderTLV.add(0);           // TAG=0（使用空记录作为分隔符）
orderTLV.add(1,30);        // TAG=1, VALUE=30（要显示的第二个输入源的标识符）
orderTLV.add(0);           // TAG=0（使用空记录作为分隔符）
orderTLV.add(1,10);        // TAG=1, VALUE=10（要显示的第三个输入源的标识符）

new Characteristic::DisplayOrder(orderTLV);    // 实例化 DisplayOrder 特征并将其值设置为 orderTLV 对象
```   

### `Service::InputSource()`

使用 `Service::InputSource()` 为电视创建新的输入源选择，例如 HDMI-1、HDMI-2、Netflix 等。`Service::InputSource()` 的使用是可选的 - 它是在没有选择不同输入源的能力的情况下创建电视服务完全可以。但是，如果使用，添加的每个输入源服务都应该在*与它所应用的电视服务相同的*附件中定义，并且***必须***使用 `addLink()` 链接到该电视服务。如果“家庭”应用发现任何未链接到电视服务的输入源服务，则会出现异常行为。

输入源可以出现在“家庭”应用中的两个位置。第一个位于输入源“选择器”中，当你打开电视的控件（即长按附件拼贴）时，该输入源“选择器”显示在开/关电源按钮下方。这是你更改电视输入源的方法。输入源出现的第二个位置是电视附件的设置页面。你可以在此处更改输入源的名称，以及配置是在输入源选择器中包含还是排除特定输入源。

总体思路是，你的草图应该实现一个包含所有潜在输入的完整列表的电视附件，使用与电视标签匹配的名称，例如 "HDMI 1"、"Component 1"、"HDMI 2" 等。如果你的电视遥控器有 Netflix、HBO Max、Amazon Prime 等专用按钮，你也可以将它们添加到列表中。创建此通用列表后，你可以直接从 家庭 应用中重命名和启用每个输入源。例如，你可以将 "HDMI 1" 重命名为 "Comcast Cable"，将 "HDMI 2" 重命名为 "Sony Blue-Ray"。如果你没有连接到 "Component 1"，你可以将其从输入源选择器中排除。这使得配置和重新配置你的电视输入源变得容易，而无需更改或更新你的 HomeSpan 草图。

所有这些都是通过使用以下一些或全部特征的组合来实现的：

* `Characteristic::ConfiguredName()` - 类似于应用于 `Service::Television()` 时的使用方式，此特征允许你设置输入源的默认名称。请注意，如果你在 家庭 应用中更改输入源的名称，则会向 HomeSpan 发送带有新名称的更新，供你在需要时在草图中使用。这与用于许多其他服务的通常的 `Characteristic::Name()` 非常不同，并且在“家庭”应用中执行的名称更改永远不会传回给附件。

*  `Characteristic::Identifier()` -该数字特征为每个输入源设置一个 ID.任何无符号的 32 位数字都可以用作 ID，前提是它*唯一的*未被同一电视服务中的任何其他输入源使用。当你使用 家庭 应用中的输入源选择器选择特定输入源时，电视服务（见上文）中的 `Characteristic::ActiveIdentifier()` 将更新为与所选输入源对应的 ID 相匹配的值。在 HomeSpan 中，你只需使用该 `update()` 方法来确定何时 `Characteristic::ActiveIdentifer()` 更新，并根据其值来确定选择了哪个输入源。HomeKit 似乎不需要 `Characteristic::Identifier()` 为输入源定义。但是，如果未设置，家庭 应用将不允许将其显示为输入源选择器中的选项，这就违背了创建输入源的目的。

*  `Characteristic::IsConfigured()` -此特性决定是否允许输入源在 家庭 应用的输入源选择器中显示为选项。如果定义了 isConfigured（）并将其设置为 0，则输入源将显示在设置页面中，但它将作为输入源选择器中的一个选项被排除。如果定义了 isConfigured（）并将其设置为 1，则输入源将显示在设置页面中，并且还将作为输入源选择器中的一个选项。如果 `Characteristic::IsConfigured()` 未为输入源定义，则该源仍将作为输入源选择器中的一个选项出现，但它将*不*出现在“设置”页面上的输入源列表中。这意味着你将无法从 家庭 应用重命名输入源，也无法将其作为输入选择器中的允许选项进行切换（见下文）。

* `Characteristic::CurrentVisibilityState()` 和 `Characteristic::TargetVisibilityState()` - 这两个特征协同工作，就像任何当前状态/目标状态对一样。当为输入源定义这些时，如果还定义了 `Characteristic::IsConfigured()`，则设置页面上输入源名称旁边会出现一个复选框切换。单击该复选框会导致“家庭”应用在 0 到 1 之间切换 TargetVisibilityState，其中 0 具有讽刺意味的是，该复选框是*选中*，而 1 表示它是*未选中*（与你的预期相反！）。如果你在 HomeSpan 中阅读此更新，则可以使用 `setVal()` 更改 CurrentVisibiltyState() 以匹配 TargetVisibilityState()。将 CurrentVisibilityState() 设置为 0 意味着输入源在输入源选择器中显示为一个选项。将 CurrentVisibilityState() 设置为 1 意味着它不会显示为选择。请注意，这些功能仅在使用 `Characteristic::Identifier()` 为输入源设置 ID 并且 IsConfigured() 已定义并设置为 1 时才起作用。

###  `Service::TelevisionSpeaker()`

此服务允许你在通过 iPhone 的远程控制小工具（可在 iPhone 控制中心找到）操作电视时，使用 iPhone 的物理音量控制按钮更改电视的音量。类似于上面的输入源业务，电视扬声器业务链***必须***接到使用 `addLink()` 的一个电视业务。电视扬声器服务需要以下两个特性：

*  `Characteristic::VolumeControlType()` -此只读特性似乎是必需的，但其用途尚不确定。在示例 HomeSpan Sketches 中，我根据 [HomeBridge](https://developers.homebridge.io/#/service/TelevisionSpeaker) 其他人在脚本中所做的工作，将此特性的值初始化为 3。

*  `Characteristic::VolumeSelector()` -每当用户通过 iPhone 的远程控制小工具操作电视并按下物理音量控制按钮时，此只写字符就会更新。当按下音量增大按钮时，家庭 应用发送的值为 0，当按下音量减小按钮时，发送的值为 1. 通过为从电视扬声器服务派生的类创建 `update()` 方法，可以以通常的方式读取这些值。

### 示例

* 请参阅 [*文件→示例→HomeSpan→其他示例→电视*](../examples/Other%20Examples/Television/Television.ino) 以获得完整的工作示例，以展示使用上述特征的不同组合的效果。此外，不要忘记查看 [HomeSpan 项目](https://github.com/topics/homespan)，了解一些真实的电视草图和遥控示例。
* 有关如何将 TLV8 记录与 DisplayOrder 特性结合使用的详细信息，请参阅教程示例 [22 - TLV8 特性](../examples/22-TLV8_Characteristics%20/22-TLV8_Characteristics.ino)
* 有关更高级的用例，请参阅 [参考草图](https://github.com/HomeSpan/HomeSpanReferenceSketches) 页面上的电视示例
* 另外，不要忘记查看 [HomeSpan 项目](https://github.com/topics/homespan) 页面，了解一些电视草图和控制器的真实示例。


### 感谢

非常感谢 @unreality 让 PR 包含了电视代码和相关功能！


---

[↩️](../README.md#resources) 返回欢迎页面
