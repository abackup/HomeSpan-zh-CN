<!-- 原文时间：2024.7.1，翻译时间：2024.7.4，校对时间：2024.7.12 -->

# 脉宽调制（PWM）

ESP32 具有多达 16 个 PWM 通道，可用于驱动各种器件。HomeSpan 包括一个集成的 PWM 库，其中包含专为控制**可调光 LED** 以及**伺服电机**设计的专用类。

## *LedPin(uint8_t pin [,float level [,uint16_t frequency [,boolean invert]]])*<a name="ledpin"></a>

创建此**类**实例可将指定 *pin* 的配置为输出适合控制可调光 LED 的 PWM 信号。参数及其默认值（如果未指定）如下所示：

  * *pin* - 输出 PWM 控制信号的引脚
  * *level* - 设置 PWM 的初始占空比百分比，从 0（LED 完全关闭）到 100（LED 完全打开）。默认值为 0（LED 最初关闭）
  * *frequency* - 在 1-65535（仅限 ESP32）或 5-65535（ESP32-S2 和 ESP32-C3）之间设置 PWM 频率，单位为 Hz.如果未指定或设置为 0，则默认为 5000 Hz
  * *boolean* - 如果为 true，PWM 信号的输出将被反转。默认值为 False
 
支持以下方法：

*  `void set(float level)`

   * 将 PWM 占空比设置为 *level*，*level* 范围从 0（LED 完全关闭）到 100（LED 完全打开）

*  `int fade(float level, uint32_t fadeTime, int fadeType=LedPin::ABSOLUTE)`

   * 使用 ESP32 的 PWM 硬件在最大 *fadeTime* 毫秒内将 LED 平滑淡化至 *level*（从 0-100）
   * 如果 *FadeType* 将设置为 **LedPin::ABSOLUTE**（默认值），则淡入淡出将占用指定 *fadeTime* 的全部时间
   * 如果 *FadeType* 将设置为 **LedPin::PROPORTIONAL**，则将根据当前级别与指定级别之间的差异按比例缩小淡入淡出时间。例如，如果当前级别设置为 30，则
     * 在 1 秒内 `fade(20, 1000, LedPin::ABSOLUTE)` 将级别设置为 20，而
     * 在 100 毫秒的过程中将级别 `fade(20, 1000, LedPin::PROPORTIONAL)` 设置为 20（因为级别只需更改 100 个单位中的 10 个单位）
   * 这是一个 **NON-BLOCKING** 方法，将立即返回。衰落发生在由 ESP32 硬件控制的背景中
   * 注意：一旦衰落开始，就不能停止或更改，直到完成（这是 ESP32 硬件的限制）。
   * 如果已成功开始淡入淡出，则此方法返回 0；如果淡入淡出已在进行中且尚无法更改，则此方法返回 1（在特定 LEDpin 的淡入淡出已在进行中时，新的淡入淡出请求将被忽略）。
   * 使用下面的 *fadeStatus* 方法来确定任何给定 LedPin 的当前衰落状态

*  `int fadeStatus()`

   * 返回 LEDPIN 的淡化状态。返回值如下：
  
     * **LedPin::NOT_FADING** - LEDPIN 当前未褪色
     * **LedPin::FADING** - LEDPIN 上的淡入淡出当前正在进行，无法更改/停止
     * **LedPin::COMPLETED** - 褪色刚刚完成
       * 返回此值后，对的 `fadeStatus()` 后续调用将返回 **LedPin::NOT_FADING**（除非你再次调用 `fade()`）
       * 通过在 `loop()` 方法中检查 `fadeStatus()==LedPin::COMPLETED`，你可以在淡入淡出完成后触发新的操作（如果需要）
  
*  `int getPin()`

   * 返回 pin 编号（如果 LedPin 未成功初始化，则返回 -1）
  
LedPin 还包括一个静态类函数，可将色调/饱和度/亮度值（通常由 HomeKit 使用）转换为红色/绿色/蓝色值（通常用于控制多色 LED）。

*  `static void HSVtoRGB(float h, float s, float v, float *r, float *g, float *b)`

   * *h* - 输入色调值，范围 0-360
   * *s* - 输入饱和值，范围 0-1
   * *v* - 输入亮度值，范围 0-1
   * *r* - 输出红色值，范围 0-1
   * *g* - 输出绿色值，范围 0-1
   * *b* - 输出蓝色值，范围 0-1

有关使用 LedPin 控制全彩 LED 的示例，请参见教程草图 [#10（RGB_LED）](../examples/10-RGB_LED/10-RGB_LED.ino)。有关使用 ESP32 的内置淡入淡出控件的示例，另请参见 [*文件→示例→HOMESPAN→其他示例→FadingLED*](../examples/Other%20Examples/FadingLED/FadingLED.ino)。

## *ServoPin(uint8_t pin [,double initDegrees [,uint16_t minMicros, uint16_t maxMicros, double minDegrees, double maxDegrees]])*<a name="servopin"></a>

创建此**类**实例可将指定 *pin* 的配置为输出 50Hz PWM 信号，该信号适用于控制大多数伺服电机。构造函数有三种形式：一种只有一个参数；一个有两个参数；另一个包含所有六个参数。参数及其默认值（如果未指定）如下所示：

  * *pin* - 输出 PWM 控制信号的引脚。伺服电机的控制线应连接在此引脚上
  * *initDegrees* - 伺服电机应设置的初始位置（以度为单位）（默认值为 0°）
  * *minMicros*  - 将伺服电机移动到其“最小”位置的 *minDegrees* 脉冲宽度（以微秒为单位）（默认值为 1000𝛍s）
  * *maxMicros* - 将伺服电机移动到其“最大”位置的 *maxDegrees* 脉冲宽度（以微秒为单位）（默认值为 2000𝛍s）
  * *minDegrees* - 接收脉冲宽度 *minMicros*（默认值为 -90°）时，伺服电机移动到的位置（以度为单位）
  * *maxDegrees* - 接收脉冲宽度 *maxMicros*（默认值为 90°）时，伺服电机移动到的位置（以度为单位）

*minMicros* 参数必须小于*maxMicros*参数，但允许设置 *minDegrees* 为大于 *maxDegrees* 得值，并可用于反转伺服电动机得最小与最大位置。支持以下方法：

*  `void set(double position)`

   * 将伺服电机的位置设置为 *position*（以度为单位）。为了保护伺服电机，小于 *minDegrees* 的 *position* 值将自动重置为 *minDegrees*，大于 *maxDegrees* 的值将自动重置为 *maxDegrees*
   * 如果 *position* 指定为 *NAN*（即 C++ “非数字”常量），则占空比设置为零，这实际上会停止脉冲生成——对于大多数模拟伺服电机，这意味着电机可以自由旋转。再次调用 `set()`，使 *position* 等于实际度数，重新启动脉冲序列并相应地设置伺服位置
  
*  `int getPin()`

   * 返回 pin 编号（如果 ServoPin 未成功初始化，则返回 -1）

在 [*文件→示例→主页→其他示例→伺服控制* ](../examples/Other%20Examples/ServoControl/ServoControl.ino) 的 Arduino IDE 中有一个示例，显示了如何使用 ServoPin 来控制电动窗帘的水平倾斜。

### PWM 资源分配和限制

以下 PWM 资源可用：

* ESP32：  16 个通道/8 个定时器（安排在两组不同的 8 个通道和 4 个定时器中）
* ESP32-S2：8 个通道/4 个定时器
* ESP32-C3：6 个通道/4 个定时器
* ESP32-S3：8 个通道/4 个定时器

HomeSpan *自动地*在 LedPin 和 ServoPin 对象实例化时将通道和计时器分配给它们。分配的每个引脚占用一个通道；在所有通道中指定的每个*唯一的*频率（对于 ESP32，在同一组中）消耗单个定时器。HomeSpan 将通过对以相同频率工作的所有信道重复使用相同的定时器来节省资源。*HomeSpan 还自动配置每个定时器，以支持指定频率的最大占空比分辨率。*

#### 诊断消息

**LedPin** 和 **ServoPin** 类根据使用 Arduino IDE 编译草图时所选的*内核调试级别*将输出 *Information \[I\]* 和 *Warning \[W\]* 信息发送到串口监视器。当没有足够的通道或计时器资源阻止创建新的 LedPin 或 ServoPin 对象时，会产生非致命的警告消息。对未能正确创建的对象的 `set()` 方法的调用将被忽略。

---

[↩️](../README.md#resources) 返回欢迎页面
