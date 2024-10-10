<!-- 原文时间：2024.2.18，翻译时间：2024.5.7，校对时间：2024.7.12 -->

# 步进电机控制

HomeSpan 包括专门的类，用于轻松控制通过步进电机驱动器板连接到 ESP32 的步进电机。这些类允许一个或多个步进电机在后台平稳异步运行，同时 HomeSpan 继续在前台运行。在具有双处理器的设备上，步进电机控制可以在与 HomeSpan 相同或不同的处理器上运行。

调用 **StepperControl** 包含控制步进电机的所有方法的 HomeSpan 类。但是，这是一个抽象类，不能直接实例化。相反，你可以使用特定于驱动程序的子类（派生自 **StepperControl**）来实例化步进电机对象，这些子类包含用于配置和操作特定驱动板的所有逻辑。每个子类都支持一个或多个构造函数，允许你指定 ESP32 设备上的哪些输出引脚将连接到驱动板上所需的引脚。

HomeSpan 中当前包含以下驱动程序：

* **[Stepper_TB6612](StepperDrivers/Stepper_TB6612.md)**
  * 该类用于操作由 [东芝 TB6612](https://cdn-shop.adafruit.com/datasheets/TB6612FNG_datasheet_en_20121101.pdf) 芯片（或等效芯片）驱动的步进电机
  * 可以与 ESP32 PWM 引脚一起使用，也可以不与 ESP32 PWM 引脚一起使用
  * 例如，[Adafruit TB6612 1.2A DC/步进电机驱动器分线板](https://www.adafruit.com/product/2448) 请参见
   
* **[Stepper_A3967](StepperDrivers/Stepper_A3967.md)**
  * 该类用于操作由 [Allegro A3967](https://cdn.sparkfun.com/datasheets/Robotics/A3967-Datasheet.pdf) 芯片（或等效芯片）驱动的步进电机
  * 例如，[Sparkfun EasyDriver 步进电机板](https://www.sparkfun.com/products/12779) 请参见

* **[Stepper_ULN2003A](StepperDrivers/Stepper_ULN2003A.md)**
  * 该类用于操作由 [Texas Instruments ULN2003A](https://www.ti.com/lit/ds/symlink/uln2003a.pdf) 芯片（或等效芯片）驱动的步进电机
  * 例如，[Stepper_ULN2003 步进电机驱动板](https://opencircuit.shop/product/uln2003-stepper-motor-driver-module) 请参见

* **[Stepper_UNIPOLAR](StepperDrivers/Stepper_UNIPOLAR.md)**
  * 此类提供用于任何中心抽头单极步进电机的通用驱动程序
  * 使用要求驱动板能够将来自 ESP32 上 4 个引脚的低电压/低电流数字信号转换为适合操作步进电机的更高电压/更高电流信号
  
单击上面的任何特定于驱动程序的类，了解有关如何连接和配置特定驱动程序板的完整详细信息。

## 步进控制方法

该类 **StepperControl** 提供以下方法来操作和控制使用上述特定于驱动程序的类之一实例化的步进电机对象：

*  `StepperControl *enable()`
    * 使电流流向步进电机线圈，有效地将电机保持在其位置
    * 返回指向自身的指针，因此方法可以以 daisy 链形式连接。
      * 例如： `myMotor=(new Stepper_TB6612(23,32,22,14,33,27))->enable();`
      
*  `StepperControl *disable()`
    * 禁用流向步进电机线圈的电流，使其处于高阻抗状态，从而允许电机自由转动
    * 返回指向自身的指针，因此方法可以以 daisy 链形式连接。
      * 例如： `myMotor=(new Stepper_TB6612(23,32,22,14,33,27))->disable();`

*  `StepperControl *brake()`
    * 禁用流向步进电机线圈的电流，但使其处于低阻抗状态，防止电机自由转动
    * 仅适用于支持“短制动”模式的驱动芯片，否则无效
    * 返回指向自身的指针，因此方法可以以 daisy 链形式连接。
      * 例如： `myMotor=(new Stepper_TB6612(23,32,22,14,33,27))->brake();`

*  `void move(int nSteps, uint32_t msDelay, endAction_t endAction=NONE)`
    * 启用步进电机并使其 *nSteps* 步进。请注意，这是一个 **non-blocking** 函数，在被调用后立即返回，同时电机在后台转动 *nSteps*。
    
      * *msDelay*-要转动的步数。正数使电机向一个方向转动；负数使电机反向转动；如果值为零，则会导致电机转动（*stop*如果已转动
      * *msDelay*-步骤之间暂停的延迟（以毫秒为单位）。必须大于零。数字越小，电机转得越快，受电机本身的限制
      * *endAction*-电机完成移动*nSteps*步骤时要执行*之后*的可选操作。选择包括：
    
        *  **StepperControl::NONE** - 不采取任何行动；步进电机处于启用状态（这是默认设置）
        *  **StepperControl::DISABLE** - 到步进电机的电流被禁用
        *  **StepperControl::BRAKE** - 步进电机处于制动状态
    * 如果在步进电机已经转动时调用此方法，则要转动的步数将重置为新*nSteps*值。当电机转动时，可以改变的*nSteps*符号来反转电机的方向，但这可能并不理想，这取决于你的电机在现实世界中连接到什么
    * 如果电机已在转动，则使用值 *nSteps = 0* 调用此方法会导致电机停止。如果电机未转动，则使用*nSteps =0*调用此方法只会启用电机，并立即执行 *endAction*（如果已指定）。
    * 示例： `myMotor.move(200,5,StepperControl::BRAKE);` 启动电机转动 200 步，每步之间延迟 5ms.当电机完成所有 200 步时，将其置于制动状态。
    
*  `int stepsRemaining()`
    * 返回要旋转的剩余步数
    * 可以是正的或负的，这取决于电机转动的方向
    * 如果电机不转动，则返回零
    * 示例： `myMotor.move(200,5); while(myMotor.stepsRemaining()!=0); myMotor.move(-300,5);` 启动电机转动，等待其完成所有 200 步，然后以相反方向转动电机 300 步

*  `int position()`
    * 返回步进电机的绝对位置，该位置定义为自初始启动后转动的所有正负步进的累积和
    * 可以在步进电机转动或停止时调用
    * 示例： `myMotor.move(-800,5); while(myMotor.stepsRemaining()); myMotor.move(200,5); while(myMotor.stepsRemaining()); Serial.print(myMotor.position())` 将在电机完成转动后打印-600 的值（首先一个方向为 800 步，然后另一个方向为 200 步）
 
*  `void setPosition(int pos)`
    * 将当前位置计数器重置为 *pos*
    * 这种方法确实可以*不*转动电机；它只重置由返回 `position()` 的内部位置计数器。
    * 此方法仅在电机**不**转动时有效（如果在电机转动时调用，则内部位置计数器保持不变）
    * 示例： `myMotor.move(300,5); while(myMotor.stepsRemaining()); myMotor.setPosition(-200); myMotor.move(600,5); while(myMotor.stepsRemaining()); Serial.print(myMotor.position())` 在电机完成转动后，将打印 +400 的值
    
*  `void moveTo(int nPosition, uint32_t msDelay, endAction_t endAction=NONE)`
    * 启用步进电机并将其转到位置 *nPosition*。请注意，这是一个 **non-blocking** 函数，在被调用后立即返回，同时步进电机转动，直到达到*nPosition*
    
      * *nPosition* - 步进器应转向的位置，其中，位置定义为自初始启动以来电动机转动的正向和负向步进的累积步数，返回值为 `position()`
      * *msDelay* - 在两步之间暂停的延迟，以毫秒为单位。必须大于零。数字越小，电机转速越快，但受电机本身的限制
      * *endAction* - 电机已达到要*之后*执行的可选动作*nPosition*。选项包括：
      
        *  **StepperControl::NONE** - 未采取任何操作；步进电机处于启用状态（这是默认设置）
        *  **StepperControl::DISABLE** - 到步进电机的电流被禁用
        *  **StepperControl::BRAKE** - 步进电机处于制动状态
    * 当步进电机已经转动时，可以调用此方法；电机将继续以相同方向或相反方向转动，直到达到规定的*nPosition*
    * 调用后 `moveTo()` 的 `stepsRemaining()` 调用按预期工作-返回的值将是电机达到*nPosition*指定之前的剩余步数
    * 请注意，`moveTo(nPosition)` 在数学上与 `move(nPosition-position())` 相同，但该 `moveTo()` 方法更精确，因为它直接在实际控制电机的任务中计算电机的位置
 
*  `StepperControl *setAccel(float accelSize, float accelSteps)`
    * 在步骤之间添加一组额外的延迟，以便电机在启动时逐渐加速，在停止时逐渐减速
  
      * *accelSize* - 附加延迟的最大大小，表示为要与和 `moveTo()` 中 `move()` 使用的 *msDelay* 参数相乘的系数。必须是大于或等于 0 的值。值越大，加速和减速的幅度越大。零值不产生加速/减速
      
      * *accelSteps* - *accelSize* 系数指数淡化的步数，此时电机开始以 *msDelay* 参数指定的全速转动。必须是大于或等于 1 的值。值越大，加速和减速周期越长
      
    * 步骤之间的总延迟（*stepsRemaining* 当不为零时）由以下公式给出：
    $$totalDelay = msDelay \times (1 + accelSize \times (e^{\frac{-\mid nSteps-stepsRemaining \mid}{accelSteps}} + e^{\frac{-(\mid stepsRemaining \mid - 1)}{accelSteps}}))$$
    * 例如：`myMotor.setAccel(10,20);myMotor.move(200,5);`
      * 在第一步之后产生 55ms 延迟，在第二步之后产生 52ms 延迟，在第三步之后产生 50ms 延迟等等，直到在步骤 82，附加延迟已经完全淡化，使得步骤之间的延迟保持固定在规定的 5ms *msDelay* 参数。然后，从步骤 118 开始（剩余 82 步），延迟增加到 6ms；在步骤 134，它进一步增加到 7ms，依此类推，直到在步骤 200 电动机停止转动之前，在步骤 199 延迟再次达到其最大值 55ms

    * 返回指向自身的指针，因此方法可以以 daisy 链形式连接。
      * 例如： `myMotor=(new Stepper_TB6612(23,32,22,14,33,27))->setAccel(10,20);`
          
*  `StepperControl *setStepType(int mode)`
    * 将电动机的步进类型设置为以下 *mode* 枚举之一：
      
      * **StepperControl::FULL_STEP_ONE_PHASE** (0)      
      * **StepperControl::FULL_STEP_TWO_PHASE** (1)
      * **StepperControl::HALF_STEP** (2)
      * **StepperControl::QUARTER_STEP** (4)
      * **StepperControl::EIGHTH_STEP** (8) 
    *  *mode* 可以使用枚举的名称或其等效整数来指定
    * 返回指向自身的指针，因此方法可以以 daisy 链形式连接。
      * 例如： `myMotor=(new Stepper_TB6612(23,32,22,14,33,27))->setStepType(StepperControl::HALF_STEP);`
    * 步进类型越小，电机运行越平稳，但转一整圈所需的步数越多
      * 并非 *modes* 所有驱动程序芯片都支持
      * 四分之一和八分之一步模式需要微步进 PWM 功能
    * 可以在电机转动时更改步进类型 *modes*，但不建议这样做
    * 有关步进电机通常如何驱动的简介，请参见 [步进电机模式](StepperModes.md)

### CPU 和任务优先级

所有**StepperControl** 构造函数都支持 *optional* 由一对 *brace-enclosed* 数字组成的 final 参数（上面未显示）。大括号中的第一个数字指定用于控制步进电机的后台任务的*优先权*。大括号中的第二个数字指定将用于运行后台控制任务的 CPU（0 或 1）**StepperControl**（单处理器芯片忽略此数字）。这个可选的 final 参数的默认值（也是建议值）是 {1，0}。示例：

*  `new Stepper_TB6612(23,32,22,14,{0,1})` 实例化由 TB6612 芯片驱动的步进电机的控制，其中 ESP32 引脚 23、32、22 和 14 分别连接到 TB6612 上的 AIN1、AIN2、BIN1 和 BIN2 引脚；后台任务的优先级设置为 0；并且任务将在 CPU 1 上运行（仅适用于双处理器芯片）

## 示例草图

下面是演示上述方法的简单草图：

```C++
// 使用基于 TB6612 的驱动板和半步 PWM 模式的步进控制示例

#include "HomeSpan.h"         // HomeSpan 包含所有 StepperControl 类

StepperControl *motor;        // 创建指向 StepperControl 的全局指针，以便可以在 setup() 和 loop() 中访问它

///////////////////

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.printf("\nHomeSpan Stepper Control\n\n");

  motor=new Stepper_TB6612(23,32,22,14,33,27);      // 使用指定的可选 PWM 引脚实例化电机对象（33 和 27）

  motor->setStepType(StepperControl::HALF_STEP);    // 将模式设置为半步，这意味着 200 步电机旋转一圈需要 400 步
  motor->setAccel(10,20);                           // 添加加速参数：额外延迟为 10 倍，衰减 20 步

  Serial.printf("Moving motor 400 steps and waiting until motor stops...\n");
  
  motor->move(-400,5);              // 使电机移动 400 步（1 转），每步间隔 5 毫秒。

  while(motor->stepsRemaining());   // 等到没有剩余步骤

  Serial.printf("Moving motor to absolute position of +1200 (i.e reverse direction for 1600 steps, or 4 revolutions) without waiting...\n");
  
  motor->moveTo(1200,2,StepperControl::BRAKE);    // 将电机移动到 1200 步的绝对位置，每步间隔 2ms；完成后进入制动状态

  // 即使 setup() 退出并且下面的 loop() 开始，电机仍会在后台继续移动
}

///////////////////

void loop(){
  
  Serial.printf("Motor has %d remaining steps\n",motor->stepsRemaining());
  
  delay(1000);      // 电机不受 delay（）的影响
  
  if(motor->position()==1200){
    Serial.printf("Motor has reached final position and is now stopped.\n");
    while(1);
  }
}
```

### 电动窗帘示例

在 [*文件→示例→HomeSpan→其他示例→MotorizedWindowShade*](../examples/Other%20Examples/MotorizedWindowShade/MotorizedWindowShade.ino) 的 Arduino IDE 中有一个完整的示例，显示了如何在完整的 HomeSpan 草图中使用 *StepperControl* 该类来控制使用 TB6612 和 A3967 驱动板的电动窗帘。

## 创建自己的 *StepperControl* 驱动程序

如果上述电机驱动程序类都不适用于你的特定芯片或驱动板，则创建一个新的驱动程序以在草图中使用是相对简单的。这是因为在后台操作步进电机的所有逻辑都已经嵌入到抽象类 *StepperControl* 中。要创建自己的驱动程序，首先要创建一个从 *StepperControl* 派生的子类。接下来，添加一个定义管脚并在需要时执行任何初始化的构造函数。最后，定义以下调用以操作电机的方法 *StepperControl* ：

*  `void onStep(boolean direction)` - 包含根据 *direction* 参数以单步推进电机的逻辑
*  `void onEnable()` - 包含启用电机驱动器的逻辑
*  `void onDisable()` - 包含禁用电机驱动器的逻辑
*  `void onBrake()` - 包含将电机置于短路制动状态的逻辑
*  `StepperControl *setStepType(int mode)` - 包含根据*mode*参数设置步骤类型模式的逻辑

只需要定义第一个方法 `onStep()`。如果其他方法不适用于你的特定驱动板，你可以不定义这些方法。当然，你可以创建其他方法来反映你的驱动板可能支持的任何其他功能。

作为示例，下面是 **Stepper_A3967** 该类的完整代码：

```C++
#include "HomeSpan.h"

//////////////////////////
 
struct Stepper_A3967 : StepperControl {

  int m1Pin;
  int m2Pin;
  int stepPin;
  int dirPin;
  int enablePin;

//////////////////////////

  Stepper_A3967(int m1Pin, int m2Pin, int stepPin, int dirPin, int enablePin, std::pair<uint32_t, uint32_t> taskParams = {1,0}) : StepperControl(taskParams.first,taskParams.second){
    this->m1Pin=m1Pin;
    this->m2Pin=m2Pin;
    this->stepPin=stepPin;
    this->dirPin=dirPin;
    this->enablePin=enablePin;

    pinMode(m1Pin,OUTPUT);
    pinMode(m2Pin,OUTPUT);
    pinMode(stepPin,OUTPUT);
    pinMode(dirPin,OUTPUT);
    pinMode(enablePin,OUTPUT);

    setStepType(FULL_STEP_TWO_PHASE);
  }

//////////////////////////

  void onStep(boolean direction) override {
    digitalWrite(dirPin,direction);
    digitalWrite(stepPin,HIGH);
    digitalWrite(stepPin,LOW);      
  }

//////////////////////////

  void onEnable() override {
    digitalWrite(enablePin,0);
  }

//////////////////////////

  void onDisable() override {
    digitalWrite(enablePin,1);
  }

//////////////////////////

  StepperControl *setStepType(int mode) override {
    switch(mode){
      case FULL_STEP_TWO_PHASE:
        digitalWrite(m1Pin,LOW);
        digitalWrite(m2Pin,LOW);
        break;
      case HALF_STEP:
        digitalWrite(m1Pin,HIGH);
        digitalWrite(m2Pin,LOW);
        break;
      case QUARTER_STEP:
        digitalWrite(m1Pin,LOW);
        digitalWrite(m2Pin,HIGH);
        break;
      case EIGHTH_STEP:
        digitalWrite(m1Pin,HIGH);
        digitalWrite(m2Pin,HIGH);
        break;
      default:
        ESP_LOGE(STEPPER_TAG,"Unknown StepType=%d",mode);
    }
    return(this);
  }
  
};
```

---

[↩️](../README.md#resources) 返回欢迎页面
