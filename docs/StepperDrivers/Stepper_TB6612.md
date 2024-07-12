# Stepper_TB6612

这是 **StepperControl** 的派生类，旨在操作由 [Toshiba TB6612](https://cdn-shop.adafruit.com/datasheets/TB6612FNG_datasheet_en_20121101.pdf)（或同等芯片）驱动的步进电机，无论是否使用 ESP32 PWM 引脚均可。

Toshiba TB6612 是一款通用电机驱动器，可直接控制两个全 H 桥。使用此芯片的 [Adafruit TB6612 1.2A DC/步进电机驱动器分线板](https://learn.adafruit.com/adafruit-tb6612-h-bridge-dc-stepper-motor-driver-breakout) 接线如下：

#### **电源连接**
* *VCC* - 连接到 ESP32 上的 +3.3V
* *VM+* - 连接到驱动步进电机的外部直流电源 (5-13V)
* *GND* - 连接到 ESP32 上的 GND，以及外部直流电源的接地
#### **电机连接**
* *MOTORA* - 连接到步进电机的 "A" 线圈
* *MOTORB* - 连接到步进电机的 "B" 线圈
#### **控制连接**
* *AIN1、AIN2* - 连接到 ESP32 上的两个数字引脚 - 用于控制线圈 *A* 的方向和状态
* *BIN1、BIN2* - 连接到 ESP32 上的两个数字引脚 - 用于控制线圈 *B* 的方向和状态
* *PWMA、PWMB* - 如果使用 PWM，则连接到 ESP32 上的两个数字引脚；如果不使用 PWM，则连接到 ESP32 上的 +3.3V 以拉高
* *STBY* - 连接到 ESP32 上的 +3.3V 以拉高

**Stepper_TB6612** 类包含两个构造函数：
* `Stepper_TB6612(int AIN1, int AIN2, int BIN1, int BIN2)`
  * 仅使用 ESP32 上的 4 个数字引脚控制驱动板，其中参数指定引脚编号。支持以下步骤类型模式：

    * FULL_STEP_ONE_PHASE
    * FULL_STEP_TWO_PHASE
    * HALF_STEP

* `Stepper_TB6612(int AIN1, int AIN2, int BIN1, int BIN2, int PWMA, int PWMB)`
  * 使用 ESP32 的 4 个数字引脚和 2 个 PWM 引脚控制驱动板，其中参数指定引脚编号。支持以下步骤类型模式：

    * FULL_STEP_ONE_PHASE
    * FULL_STEP_TWO_PHASE
    * HALF_STEP
    * QUARTER_STEP
    * EIGHTH_STEP
                
---

[↩️](../Stepper.md) 返回步进电机控制页面
