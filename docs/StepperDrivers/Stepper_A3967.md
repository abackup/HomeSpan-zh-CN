# Stepper_A3967

这是 **StepperControl** 的派生类，旨在操作由 [Allegro A3967](https://cdn.sparkfun.com/datasheets/Robotics/A3967-Datasheet.pdf)（或同等芯片）驱动的步进电机。

Allegro A3967 是专为步进电机设计的专用驱动器。它包含内置 PWM 发生器和预编程的步进模式。使用此芯片的 [Sparkfun EasyDriver 步进电机板](https://learn.sparkfun.com/tutorials/easy-driver-hook-up-guide?_ga=2.152816825.1841726212.1688220137-156607829.1686369274) 接线如下：

#### **电源连接**
* *GND* - 连接到 ESP32 上的 GND，以及外部直流电源的接地
* *M+* - 连接到将驱动步进电机的外部直流电源。板载调节器也使用此电源为电路板的其余部分提供 VCC。要与 ESP32 一起使用，您必须用一团焊料短接 3.3V/5V 跳线以选择 3.3V
#### **电机连接**
* *电机 A* - 连接到步进电机的“A”线圈
* *电机 B* - 连接到步进电机的“B”线圈
#### **控制连接**
* *MS1、MS2* - 连接到 ESP32 上的两个数字引脚 - 用于设置步进类型模式
* *STEP、DIR* - 连接到 ESP32 上的两个数字引脚 - 用于步进电机并设置方向
* *ENABLE* - 连接到 ESP32 上的数字引脚 - 用于启用/禁用电机驱动器
* *SLEEP、RESET* - 已在 EasyDriver 板上拉高，因此无需连接。如果使用不同的驱动板，请确保这些引脚被拉高，否则连接到 VCC
* *PFD* - 未使用

**Stepper_A3967** 类包括以下构造函数：
* `Stepper_A3967(int MS1, int MS2, int STEP, int DIR, int ENABLE)`
  * 使用来自 ESP32 的 5 个数字引脚控制驱动板，其中参数指定引脚编号。支持以下步骤类型模式：

    * FULL_STEP_TWO_PHASE
    * HALF_STEP
    * QUARTER_STEP
    * EIGHTH_STEP

❗注意：A3967 芯片不支持短制动状态。调用 `brake()` 方法以及将 `move()` 和 `moveTo()` 方法中的 *endAction* 参数设置为 **StepperControl::BRAKE** 对电机驱动器没有影响。


---

[↩️](../Stepper.md) 返回步进电机控制页面
