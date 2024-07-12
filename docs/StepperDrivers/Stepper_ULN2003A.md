# Stepper_ULN2003A

这是 **StepperControl** 的派生类，旨在操作由 [Texas Instruments ULN2003A](https://www.ti.com/lit/ds/symlink/uln2003a.pdf)（或同等芯片）驱动的步进电机。

Texas Instruments ULN2003A 芯片包含七个达林顿晶体管对阵列，每个晶体管对都能够将低压/低电流数字信号转换为适合驱动步进电机的高压/高电流输出。[^1]

[^1]: 仅需 ULN2003A 中的七个达林顿晶体管对中的四个即可驱动典型的单极步进电机。

各个制造商已将该芯片纳入专用的步进电机板中，用于驱动单极电机，例如 [28BYJ&#8209;48 5&#8209;伏步进电机](https://opencircuit.shop/product/28byj-48-5v-stepper-motor-4-phase-5-wire) 和 [28BYJ&#8209;48 12&#8209;伏步进电机](https://opencircuit.shop/product/28byj-48-12v-stepper-motor-4-phase-5-wire)。使用此芯片的 [Opencircuit ULN2003 步进电机驱动板](https://opencircuit.shop/product/uln2003-stepper-motor-driver-module) 的接线如下：

#### **电源连接**[^2]
* ➕ - 连接到驱动步进电机的外部直流电源 (5-12V)
* ➖ - 连接到 ESP32 上的 GND，以及外部直流电源的接地
#### **电机连接**
* 将电机直接插入电路板的 5 针连接器
#### **控制连接**
* *IN1、IN2、IN3、IN4* - 连接到 ESP32 上的四个数字引脚 - 用于使电机朝任一方向步进

**Stepper_ULN2003A** 类包括以下构造函数：
* `Stepper_ULN2003A(int IN1, int IN2, int IN3, int IN4)`
  * 仅使用 ESP32 的 4 个数字引脚控制驱动板，其中参数指定引脚编号。支持以下步骤类型模式：

    * FULL_STEP_ONE_PHASE
    * FULL_STEP_TWO_PHASE
    * HALF_STEP

❗注意：ULN2003A 芯片不支持短制动状态。调用 `brake()` 方法以及将 `move()` 和 `moveTo()` 方法中的 *endAction* 参数设置为 **StepperControl::BRAKE** 对电机驱动器没有影响。<br><br>

> [!TIP]
> 如果您将电机设置为非常缓慢地移动（例如每步 500 毫秒），您将能够通过观察内置在驱动板上的四个 LED（标记为 *A、B、C、D*）来跟踪电机线圈每个相的电流如何打开和关闭，具体取决于所选的*步进模式*。另请参阅此 [Last Minute Engineers 教程](https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial)，详细了解 ULN2003A 及其用于驱动 28BYJ&#8209;48 5&#8209;伏步进电机的用途。

[^2]: ULN2003A 是无源芯片 - 驱动板和 ESP32 之间没有 VCC 电源连接

---

[↩️](../Stepper.md) 返回步进电机控制页面
