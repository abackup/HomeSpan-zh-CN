<!--   原文时间：2023.7.3,翻译时间：2024.5.6，校对时间：2024.7.12  -->

# 使用可闪烁接口创建自定义状态指示灯

HomeSpan 通过状态指示灯不同的闪烁模式向用户传达有关 HomeSpan 状态的信息。*HomeSpan* 允许你使用 `setStatusPin()` and `setStatusPixel()` 方法分别选择标准 LED 或 NeoPixel LED 作为状态指示灯。另外，可以使用 *homeSpan* 的`setStatusDevice(Blinkable *sDev)`方法将状态指示设置为实现**闪烁**接口[^1]的任何对象，其中 *sDev*  是闪烁对象。

要创建你自己的闪烁对象，请首先创建从 **Blinkable** 派生的子类。接下来，添加一个定义引脚的构造函数并根据需要执行任何初始化。最后，定义 **Blinkable** 调用以使设备闪烁的以下*必需*方法：


* `void on()` - 打开设备（例如使 LED 亮起）
* `void off()` - 关闭设备（例如使 LED 熄灭）
* `int getPin()` - 返回设备的引脚号（任何数字都可以；不必是实际的 ESP32 引脚）


例如，以下代码为倒置 LED 定义了一个 Blinkable 对象，当 ESP32 引脚为低电平时，该 LED 会*打开*，当 ESP32 引脚为高电平时，该 LED 会*关闭*：

```C++

// 在草图顶部附近创建此结构

struct invertedLED : Blinkable {        // 创建一个派生自 Blinkable 的子类

  int pin;                              // 用于存储引脚号的变量
  
  invertedLED(int pin) : pin{pin} {     // 初始化 pin 参数的构造函数
    pinMode(pin,OUTPUT);                // 将引脚设置为输出
    digitalWrite(pin,HIGH);             // 将引脚设置为高电平（对于倒置的 LED，该引脚处于关闭状态）
  }

  void on() override { digitalWrite(pin,LOW); }        // 将必需的函数 on() - 将引脚设置为低电平
  void off() override { digitalWrite(pin,HIGH); }      // 必需的函数 off() - 将引脚设置为高电平
  int getPin() override { return(pin); }               // 必需函数 getPin() - 返回引脚号
};

...

// 然后使用 SETUP() 中的结构体设置状态指示灯

void setup(){

  homeSpan.setStatusDevice(new invertedLED(13));    // 将状态指示灯设置为连接到引脚 13 的新的可闪烁设备

...
}
```

[^1]: 在 C++ 中，*接口*是仅包含纯虚函数的任何抽象类。你无法实例化接口，但可以实例化该接口的任何派生子类，前提是你定义了每个所需的虚拟函数。

---

[↩️](Reference.md) 返回到API页面
