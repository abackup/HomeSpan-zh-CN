<!-- 原文时间：2024.2.18，翻译时间：2024.5.15，校对时间：2024.7.12 -->


# 远程控制射频/红外信号生成

ESP32 具有片上远程控制 (RMT) 信号发生器，旨在驱动射频或红外发射器。HomeSpan 包含一个易于使用的专用类 **RFControl()**，它与 ESP32 RMT 外设连接，这样你就可以通过一些额外的电子组件创建一个控制射频或红外设备的 HomeSpan 设备直接从 iPhone 上的“家庭”应用或通过 Siri 进行操作。

## *RFControl(int pin, boolean refClock=true)*

创建此**类**的实例会初始化 RF/IR 信号发生器并指定 ESP32 *pin* 输出信号。如果驱动多个 RF/IR 发射器（每个发射器连接到不同的 *pin* ），你可以创建该类的多个实例，但须遵守以下限制：ESP32 8 个实例；ESP32-S2 和 ESP32-S3 4 个实例；ESP32-C3 2 个实例。下面在 `start()` 方法下进一步更全面地描述可选参数 *refClock*。

信号被定义为一系列高电平和低电平相位，它们一起形成脉冲序列，你可以在其中指定每个高电平和低电平相位的持续时间（以时钟周期为单位 *ticks*），分别显示为 H1-H4 和 L1-L4 如下图中的：

![Pulse Train](images/pulseTrain.png)

由于大多数 RF/IR 信号多次重复相同的脉冲序列，因此应延长最后一个低电平阶段的持续时间，以考虑脉冲序列重复之间的延迟。脉冲串被编码为 32 位字的顺序数组，其中每个 32 位字代表使用以下协议的单个脉冲：

  * 位 0-14：要传输的脉冲第一部分的持续时间，以 *ticks* 表示，范围为 0-32767
  * 位 15：指示要发送的脉冲的第一部分是高电平 (1) 还是低电平 (0)
  * 位 16-30：要传输的脉冲第二部分的持续时间，以 *ticks* 为单位，范围为 0-32767
  * 位 31：指示要发送的脉冲的第二部分是高电平 (1) 还是低电平 (0)

HomeSpan 提供了两种简单的方法来创建、存储和传输脉冲序列。第一种方法依赖于这样一个事实：RFControl 的每个实例都维护自己的内部存储器结构来存储任意长度的脉冲序列。下面描述的函数 `clear()`、 `add()` 和 `pulse()` 允许你使用此内部存储器结构创建脉冲序列。然后使用 `start()` 函数开始传输完整的脉冲序列。当需要根据需要动态创建脉冲串时，通常使用此方法，因为每个 RFControl 实例一次只能存储一个脉冲串。

在第二种方法中，你可以使用上述协议在 32 位字的外部数组中创建一个或多个脉冲序列。要开始传输特定脉冲串，请使用指向包含该脉冲串的外部数组的指针引用来调用 `start()` 函数。当你想要预先计算许多不同的脉冲序列并让它们根据需要准备好传输时，通常会使用此方法。请注意，此方法要求数组存储在 RAM 中，而不是 PSRAM 中。

各功能详情如下：

*  `void clear()`

   * 清除 RFControl 特定实例的脉冲串内存结构

*  `void phase(uint32_t numTicks, uint8_t phase)`

   * 将高电平或低电平相位附加到特定 RFControl 实例的脉冲序列内存缓冲区

     * *numTicks* - 脉冲相位的持续时间，以 *ticks* 为单位。允许超过 32767 个刻度的持续时间（系统自动创建每个最多 32767 个刻度的重复脉冲，直到达到指定的持续时间 *numTicks*）
    
     * *phase* - 设置为 0 以创建低电平阶段；设置为 1（或任何非零数字）以创建高相位
    
    * 允许相同类型的重复阶段（例如，高电平后跟另一个高电平），并导致单个高电平或低电平阶段，其持续时间等于为每个重复阶段指定的 *numTicks* 之和（此在生成曼彻斯特编码信号时很有帮助）

*  `void add(uint32_t onTime, uint32_t offTime)`

   * 将持续时间 *onTime* 后跟 *offTime* 的单个高/低脉冲附加到 RFControl 特定实例的脉冲序列。这在功能上相当于调用 `phase(onTime,HIGH);` 后跟上面定义的 `phase(offTime,LOW);`

*  `void enableCarrier(uint32_t freq, float duty=0.5)`

    * 能够使用“方波”载波调制脉冲串。实际上，这仅用于红外信号（不是 RF）
  
      * *freq* - 载波的频率（以 Hz 为单位）。如果 freq=0，则禁用载波
    
      * *duty* - 载波的占空比，从 0-1. 如果未指定，默认值为 0.5

    * 如果指定频率和占空比的组合超出支持的配置范围，RFControl 将报告错误

*  `void disableCarrier()`

    * 禁用载波。相当于 `enableCarrier(0);`

*  `void start(uint8_t _numCycles, uint8_t tickTime)`
*  `void start(uint32_t *data, int nData, uint8_t nCycles, uint8_t tickTime)`

* 在第一个变体中，这开始传输存储在给定 RFControl 实例的内部存储器结构中的脉冲串，该实例是使用 `clear()`、 `add()`、和上面的 `phase()` 功能。在第二种变体中，这开始传输存储在包含 *nData* 32 位字的外部数组 *data* 中的脉冲串。信号将输出到实例化 RFControl 时指定的引脚上。请注意，这是一个阻塞调用 - 该方法会等到传输完成后再返回。这不会在程序操作中产生明显的延迟，因为大多数 RF/IR 脉冲序列只有几十毫秒长
 
   * *numCycles* - 传输脉冲串的总次数（即值 3 表示脉冲串将传输一次，然后再传输 2 次）。这是一个可选参数，如果未指定，则默认值为 1。
   
   * *tickTime* - *tick* 的持续时间，以 ***clock units*** 为单位。这是一个可选参数，如果未指定，则默认值为 1  *clock units*。有效范围为 1-255 *clock units*，或设置为 0 表示 256 *clock units*。*clock units* 的持续时间由 *refClock* 参数（上述 RFControl 构造函数中的第二个可选参数）确定。如果 *refClock* 设置为 true（默认），RFControl 使用 ESP32 的 1MHz 参考时钟进行计时，以便每个 *clock units* 等于 1𝛍s。如果 *refClock* 设置为 false，RFControl 使用 ESP32 更快的 80MHz APB 时钟，以便每个 *clock units* 等于 0.0125𝛍s（微秒的 1/80）
   
* 为了帮助创建存储在外部 32 位字数组中的脉冲序列，RFControl 包含宏 *RF_PULSE(highTicks,lowTicks)*，它返回代表单个高/低脉冲的格式正确的 32 位值持续时间 *highTicks* 后跟 *lowTicks*。这基本上类似于 `add()` 函数。例如，以下代码片段显示了创建和传输相同 3 脉冲脉冲串的两种方法 —— 唯一的区别是一种使用 RFControl 的内部存储器结构，第二种使用外部阵列：

```C++

RFControl rf(11);  // 创建 RFControl 的实例

rf.clear();        // 清除内部存储器结构
rf.add(100,50);    // 创建 100 个高点滴答脉冲，然后创建 50 个低点滴答脉冲
rf.add(100,50);    // 创建第二个脉冲，先是 100 个高点滴答，然后是 50 个低点滴答
rf.add(25,500);    // 创建第三个脉冲，先有 25 个高点滴答，然后有 500 个低点滴答
rf.start(4,1000);  // 开始传输脉冲串；重复4个循环；一刻 = 1000𝛍s

uint32_t pulseTrain[] = {RF_PULSE(100,50), RF_PULSE(100,50), RF_PULSE(25,500)};    // 在外部阵列中创建相同的脉冲序列
rf.start(pulseTrain,3,4,1000);  // 使用相同的参数开始传输
```
#### 诊断信息

**RFControl** 类根据使用 Arduino IDE 编译草图时选择的*核心调试级别*将 *Warning \[W\]* 消息输出到串口监视器。当通道资源不足阻止创建新的 RFControl 对象时，会生成非致命警告消息。对于未能正确创建的对象，对 `start()` 方法的调用将被静默忽略。

## 射频控制草图示例

下面是一个完整的草图，它产生两个不同的脉冲序列，信号输出链接到 ESP32 设备的内置 LED（而不是射频或红外发射器）。出于说明目的，滴答持续时间已设置为非常长的 100𝛍s，脉冲时间范围为 1000-10,000 滴答，以便在 LED 上轻松辨别各个脉冲。请注意，此示例草图也可以在 Arduino IDE 中的 [*文件→示例→HomeSpan→其他示例→远程控制*](../examples/Other%20Examples/RemoteControl/RemoteControl.ino) 下找到。

```C++
/* HomeSpan Remote Control Example */

#include "HomeSpan.h"             // 包括 HomeSpan 库

void setup() {     
 
  Serial.begin(115200);           // 启动串口接口
  Serial.flush();
  delay(1000);                    // 等待接口刷新

  Serial.print("\n\nHomeSpan RF Transmitter Example\n\n");

  RFControl rf(13);               // 创建一个 RFControl 实例，并将信号输出到 ESP32 上的引脚 13

  rf.clear();                     // 清除脉冲串内存缓冲区

  rf.add(5000,5000);              // 创建具有三个 5000 刻度高/低脉冲的脉冲串
  rf.add(5000,5000);
  rf.add(5000,10000);             // 最终低谷期持续时间加倍

  Serial.print("Starting 4 cycles of three 500 ms on pulses...");
  
  rf.start(4,100);                // 开始传输脉冲串的 4 个周期，1 个刻度 = 100 微秒

  Serial.print("Done!\n");

  delay(2000);

  rf.clear();

  for(int i=1000;i<10000;i+=1000)
    rf.add(i,10000-i);
  rf.add(10000,10000);
  
  Serial.print("Starting 3 cycles of 100-1000 ms pulses...");
  
  rf.start(3,100);                // 开始传输 3 个脉冲序列周期，1 个刻度 = 100

  Serial.print("Done!\n");
  
  Serial.print("\nEnd Example");
  
} // setup() 结束

void loop(){

} // loop() 结束
```

---

[↩️](../README.md#resources) 返回欢迎页面
