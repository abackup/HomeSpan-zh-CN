<!-- 原文时间：2024.2.18，翻译时间：2024.5.13，校对时间：2024.7.12 -->

# 可寻址全彩 LED

HomeSpan 包括两个专用类，可轻松控制“可寻址” 全彩 LED。**Pixel()** 类用于仅需要单个“数据”控制线的 RGB 和 RGBW LED，例如此 8 像素 [NeoPixel RGB 灯条](https://www.adafruit.com/product/1426) 或单灯珠 [NeoPixel RGBW LED](https://www.adafruit.com/product/2759)。**Dot()** 类用于需要两条控制线（“数据”和“时钟”）的 全彩 LED，例如这个 144 像素 [DotStar RGB 灯带](https://www.adafruit.com/product/2241) 或这个 60 像素 [全彩 LED 灯带](https://www.sparkfun.com/products/14015)。

这两个类都允许你将多像素 LED 灯条中的每个“像素”单独设置为不同的 24 位 RGB 颜色（如果使用 RGBW LED，则为 32 位颜色）。或者，这些类允许你简单地指定单个 24 位（或 32 位）颜色以在所有像素上复制。

这两个类的方法几乎相同，这使你可以轻松地将为单线设备编写的代码互换为与双线设备一起使用（反之亦然），只需进行少量修改。

## *Pixel(uint8_t pin, [boolean isRGBW])*

创建此**类**的实例可将指定的 *pin* 配置为输出适合控制具有任意数量的单线、可寻址 RGB 或 RGBW LED 设备的波形信号。像素。此类设备通常包含 SK6812 或 WS2812 LED。参数及其默认值（如果未指定）如下：

  * *pin* - 输出 RGB 控制信号的引脚；通常连接到可寻址 LED 设备的“数据”输入
  * *pixelType* - 控制颜色数据的传输顺序，以及设备是否包含 3 色（红/绿/蓝）或 4 色（红/绿/蓝/白）LED。**PixelType** 命名空间中提供了 *pixelType* 的预定义值。从以下十二种格式中选择一种：
    
    * *PixelType::RGB, PixelType::RBG, PixelType::BRG, PixelType::BGR, PixelType::GBR, PixelType::GRB*
    * *PixelType::RGBW, PixelType::RBGW, PixelType::BRGW, PixelType::BGRW, PixelType::GBRW, PixelType::GRBW*
      
* 示例：`Pixel myDevice(26, PixelType::BRGW);` 创建一个连接到引脚 26 的 4 色 RGBW 设备，其中颜色按蓝色、红色、绿色和白色的顺序传输

注意: *pixelType* 是可选的。如果未指定，则默认值为 *PixelType::GRB*.

> [!TIP]
> 由于您的特定设备可能具有哪种类型的 LED 通常并不明显，因此 HomeSpan 包含一个草图，旨在帮助您确定 *pixelType* 的正确值。有关详细说明，请参阅 Arduino IDE 下的 [*文件→示例→HomeSpan→其他示例→PixelTester*](../examples/Other%20Examples/PixelTester/PixelTester.ino)。如果您发现 Pixel 设备的颜色与 Home App 中显示的颜色不匹配，请使用此测试器草图。

设置像素颜色的两种主要方法是：

* `void set(Color color, int nPixels=1)`

  * 将单像素设备中像素的颜色，或者等效地将多像素设备中第一个 *nPixels* 的颜色设置为 *color*，其中 *color* 是下面定义的 **color** 类型的对象。如果未指定，*nPixels* 默认为 1（即单个像素）。如果为 *nPixels* 指定的值与设备中实际 RGB（或 RGBW ）像素总数不匹配，这不是问题；如果 *nPixels* 小于设备像素总数，则只有第一个 *nPixels* 将被设置为 *color*；如果 *nPixels* 大于设备像素总数，设备将忽略额外的输入
  
* `void set(Color color, int nPixels)`

  * 将多像素设备中每个像素的颜色单独设置为 **color** 数组 *\*color*，大小为 *nPixels* 的 **color** 数组 *\*color* 中指定的颜色值，其中设备的第一个像素设置为中的值 *color\[0\]*，第二个像素设置为中的值 *color\[1\]*...最后一个像素设置为中的值 *color\[nPixels-1\]*。与上面类似，如果指定的值没有问题 *nPixels* 与设备中实际 RGB（或 RGBW）像素总数不匹配

在上述两种方法中，颜色都存储在 32 位 **color** 对象中，该对象配置为保存四个 8 位 RGBW 值。**color** 对象可以实例化为单个变量（例如 `Pixel::Color myColor;` ）或数组（例如 `Pixel::Color myColors[8];` ）。请注意，**Pixel** 类使用的 **color** 对象的范围仅限于 **Pixel** 类本身，因此你需要使用完全限定的类名称 "Pixel::Color"。创建 **color** 对象后，可以使用以下两种方法之一设置它存储的颜色：
  
  * `Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t w=0)`

    * 其中 *r* 、 *g* 和 *b* 表示 0-255 范围内的 8 位红色、绿色和蓝色值，*w* 代表白色 LED 的 8 位值 [0-255]。白色值可以未指定，在这种情况下它默认为 0。此外，白色值将被 *set()* 忽略，除非在构造函数中 *isRGBW* 标志被指定为 *true*
    * 示例： `myColor.RGB(255,255,0)` 将 myColor 设置为亮黄色
      
  * `Color HSV(float h, float s, float v, double w=0)`
    
    * 其中 *h* 为色调，范围为 0-360；*s* 为饱和度百分比，从 0-100；*v* 为 0-100 之间的亮度百分比。这些值将转换为等效的 8 位 RGB 值 (0-255)，以便存储在 *color* 对象中。请注意，*w* 值是单独处理的，代表白色 LED 的亮度百分比（从 0-100），该值也被转换为 0-255 的 8 位值，以便存储在 **color** 对象。与上面类似，白色值可以不指定，在这种情况下它默认为 0
    * 示例： `myColor.HSV(120,100,50)` 将 myColor 设置为亮度为 50% 的完全饱和绿色
      
请注意，上述两种方法都会返回完整的 **color** 对象本身，因此可以在需要 **color** 对象的任何地方使用：例如：  `Pixel p(5); Pixel::Color myColor; p.set(myColor.RGB(255,215,0))` 将连接到引脚 5 的单个像素设备的颜色设置为亮金色。

 **Pixel** 类还支持以下类级方法作为创建颜色的便捷替代方法：
  
* `static Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t w=0)`
  * 相当于 `return(Color().RGB(r,g,b,w));`
  * 示例： `Pixel p(8);  p.set(Pixel::RGB(0,0,255),8);` 将 8 像素设备中每个像素的颜色设置为蓝色

* `static Color HSV(float h, float s, float v, double w=0)`
  * 相当于 `return(Color().HSV(h,s,v,w));`
  * 示例： `Pixel::Color c[]={Pixel::HSV(120,100,100),Pixel::HSV(60,100,100),Pixel::HSV(0,100,100)};` 创建红黄绿交通灯图案

最后，**Pixel** 类支持这两个额外但很少需要的方法：

* `int getPin()`

  * 返回 pin 号，如果实例化由于缺乏资源而失败，则返回 -1

* `boolean isRGBW()`

  * 如果 *pixelType* 指定了 RGBW LED，则返回 *true*，如果只是 RGB LED，则返回 *false*

* `void setTiming(float high0, float low0, float high1, float low1, uint32_t lowReset)`

  * **Pixel** 类用于生成设置全彩 LED 设备颜色所需的“数据”信号的默认时序参数应该适用于大多数基于 SK6812 或 WS2812 驱动器芯片的商业产品。如果你需要重写类默认值并将其替换为你自己的计时参数，请使用此方法**仅有的**，其中
    * *high0* 和 *low0* 指定编码零位的脉冲的高相位和低相位的持续时间（以微秒为单位）；
    * *high1* 和 *low1* 指定编码一位的脉冲的高相位和低相位的持续时间（以微秒为单位）；和
    * *lowReset* 指定表示脉冲流结束的延迟（以微秒为单位）
  * 作为参考，**Pixel** 类使用以下默认值：*high0 为 0.32𝛍s, low0 为 0.88𝛍s, high1 为 0.64𝛍s, low1 为 0.56𝛍s, lowReset 为 80.0𝛍s* 

> [!TIP]
> 如果您的 LED 颜色闪烁，这可能是由于时序参数不匹配造成的。要解决此问题，请在网上搜索特定设备使用的芯片的规格文档。这些文档通常提供一个表格，列出传输二进制 1 或二进制 0 所需的高电平和低电平周期的持续时间。然后，您可以使用上面的 setTiming 方法相应地设置时序参数。

### 资源使用和资源冲突

 **Pixel** 类依赖 ESP32 的 RMT 外设来创建控制单线可寻址全彩 LED 所需的精确脉冲序列。由于 **Pixel** 的每个实例化都会消耗一个 RMT 通道，因此你可以实例化的 **Pixel** 对象的数量（每个对象控制连接到特定引脚的单独的多像素全彩 LED 设备）仅限于可用的 RMT 数量，如下所示：ESP32 8 个实例；ESP32-S2 4 个实例；ESP32-C3 2 个实例。

此外，**Pixel** 类经过优化，可以处理包含数百个 RGB 或 RGBW 像素的任意长的 LED 灯条。为了有效地完成此操作，**Pixel** 类实现了自己的 RMT 驱动程序，该驱动程序与 HomeSpan 的 **RFControl** 库使用的默认 RMT 驱动程序冲突。不幸的是，这意味着你不能在同一个 HomeSpan 草图中同时使用 **Pixel** 类库和 **RFControl** 类库。

## *Dot(uint8_t dataPin, uint8_t clockPin)*

创建此**类**的实例将指定的引脚配置为输出适合控制具有任意数量像素的两线、可寻址全彩 LED 设备的波形信号。此类设备通常包含 SK9822 或 APA102 LED，或嵌入式 WS2801 驱动器芯片。参数及其默认值（如果未指定）如下：

  * *dataPin* - 将输出 RGB 数据信号的引脚；通常连接到可寻址 LED 设备的“数据”输入
  * *clockPin* - 将输出 RGB 时钟信号的引脚；通常连接到可寻址 LED 器件的“时钟”输入

设置像素颜色的两种主要方法是：

* `void set(Color color, int nPixels=1)`

  * 将单像素设备中像素的颜色设置为 *color*，或者等效地将多像素设备中第一个 *nPixels* 的颜色设置为 *color*，其中 *color* 是下面定义的 **color** 类型的对象。如果未指定，*nPixels* 默认为 1（即单个像素）。如果为 *nPixels* 指定的值与设备中的实际 RGB 像素总数不匹配，这不是问题；如果 *nPixels* 小于设备像素总数，则仅第一个*nPixels* 将被设置为 *color*；如果 *nPixels* 大于设备像素总数，设备将忽略额外的输入
  
* `void set(Color *color, int nPixels)`

  * 将多像素设备中每个像素的颜色单独设置为 **color** 数组 *\*color*，大小为 *nPixels* 的 **color** 数组 *\*color* 中指定的颜色值，其中设备的第一个像素设置为中的值 *color\[0\]*，第二个像素设置为中的值 *color\[1\]*...最后一个像素设置为中的值 *color\[nPixels-1\]*。与上面类似，如果指定的值没有问题 *nPixels* 与设备中实际 RGB 像素总数不匹配

在上述两种方法中，颜色都存储在 32 位 **color** 对象中，该对象配置为保存三个 8 位 RGB 值以及一个可用于限制 LED 电流的 5 位值。**color** 对象可以实例化为单个变量（例如 `Dot::Color myColor;` ）或数组（例如 `Dot::Color myColors\[8\];` ）。请注意，**Dot** 类使用的 **color** 对象的范围仅限于 **Dot** 类本身，因此你需要使用 full-合格的类名称 "Dot::Color"。创建 **color** 对象后，可以使用以下两种方法之一设置它存储的颜色：
  
  * `Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t driveLevel=31)`

    * 其中 *r*、*g* 和 *b* 表示 0-255 范围内的 8 位红色、绿色和蓝色值，*driveLevel* 表示 5 位值 [0-31]，用于限制 LED 电流从 0（无电流）到 31（最大电流，这是默认值）。通过将 *driveLevel* 设置为小于 31 的值来限制 LED 电流，可以提供一种控制每个像素全彩 LED 亮度的无闪烁方式。
    * 示例： `myColor.RGB(128,128,0)` 使用红色和绿色 LED 的 50% 占空比将 myColor 设置为半亮度黄色（即 128/256）
    * 示例： `myColor.RGB(255,255,0,16)` 通过将像素的 LED 电流限制为其最大值的 50%（即 16/32），将 myColor 设置为半亮度黄色
      
  * `Color HSV(float h, float s, float v, double drivePercent=100)`
    
    * 其中*h* 为色调，范围为 0-360；*s* 为饱和度百分比从 0-100；*v* 为 0-100 之间的亮度百分比。这些值将转换为等效的 8 位 RGB 值 (0-255)，以便存储在 *color* 对象中。*drivePercen* 参数以与上面 *driveLevel* 相同的方式控制电流，不同之处在于它不是指定为 0-31 的绝对值，而是指定为百分比从 0 到 100（默认）
    * 示例： `myColor.HSV(120,100,50)` 使用 50% 占空比将 myColor 设置为半亮度、完全饱和的绿色
    * 示例： `myColor.HSV(120,100,100,50)` 通过将像素的 LED 电流限制为其最大值的 50%，将 myColor 设置为半亮度的完全饱和绿色
      
请注意，上述两种方法都会返回完整的 **color** 对象本身，因此可以在需要 **color** 对象的任何地方使用：例如： `Dot p(5,6); Dot::Color myColor; p.set(myColor.RGB(255,215,0))` 将连接到引脚 5 和 6 的单个像素设备的颜色设置为亮金色。

 **Pixel** 类还支持以下类级方法作为创建颜色的便捷替代方法：
  
* `static Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t driveLevel=31)`
  * 相当于 `return(Color().RGB(r,g,b,driveLevel));`
  * 示例： `Dot p(8,11);  p.set(Dot::RGB(0,0,255),8);` 将 8 像素设备中每个像素的颜色设置为蓝色

* `static Color HSV(float h, float s, float v, double drivePercent=100)`
  * 相当于 `return(Color().HSV(h,s,v,drivePercent));`
  * 示例： `Dot::Color c[]={Dot::HSV(120,100,100),Dot::HSV(60,100,100),Dot::HSV(0,100,100)};` 创建红黄绿交通灯图案

与 **Pixel** 类不同，**Dot** 类*不是*使用 ESP32 的 RMT 外设，因此没有限制你可以实例化 **Dot** 类对象的数量，并且在同一个文件中同时使用 **Dot** 类和 **RFControl** 库也不会有任何冲突草图。此外，由于时钟信号是由 **Dot** 类本身生成的，因此无需设置定时参数，也不需要 *setTiming()* 方法。

### 示例草图

一个完整的示例展示了如何在 HomeSpan 草图中使用 Pixel 库来控制 RGB Pixel 设备、RGBW Pixel 设备和 RGB DotStar 设备（所有这些都来自 iPhone 上的“家庭”应用），可以在 Arduino IDE 中找到 [*文件→示例→HomeSpan→其他示例→像素*](../examples/Other%20Examples/Pixel/Pixel.ino)。

如需更完整地展示 Pixel 库，请查看 [HomeSpan 项目](https://github.com/topics/homespan)上的 [节日灯](https://github.com/HomeSpan/HolidayLights)。此草图演示了如何使用 Pixel 库通过 60 像素 RGBW 条生成各种特殊效果。该草图还展示了使用 HomeSpan 的 [自定义特征宏](Reference.md#custom) 来实现在 Eve 应用中使用的特效“选择器”按钮。

---

[↩️](../README.md#resources) 返回欢迎页面
