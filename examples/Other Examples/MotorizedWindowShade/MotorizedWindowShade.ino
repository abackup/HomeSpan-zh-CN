/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2020-2024 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  特此授予获得此软件和相关文档文件（“软件”）副本的任何人免费许可，以无限制方式处理软件，
 *  包括但不限于使用、复制、修改、合并、发布、分发、再许可和/或销售软件副本的权利，并允许
 *  向其提供软件的人员这样做，但须遵守以下条件：
 *  
 *  上述版权声明和本许可声明均应包含在软件的所有副本或重要部分中。
 *  
 *  软件按“原样”提供，不作任何明示或暗示的保证，包括但不限于适销性、特定用途的适用性和不
 *  侵权性的保证。在任何情况下，作者或版权持有者均不对因软件或使用或其他处理软件而引起的
 *  或与之相关的任何索赔、损害或其他责任承担责任，无论是合同行为、侵权行为还是其他行为。
 *  
 ********************************************************************************/
 
// 此示例演示了如何使用 HomeSpan 的 StepperControl 类通过实现电动窗帘附件来控制现实世界的步进电机。

// 下面的草图基于教程示例 13 中包含的注释更全面的 WindowShade 附件（此草图仅包含与步进电机的使用相关的注释）。

// 我们将在附件中使用两个步进电机：

// * 一个电机用于打开/关闭窗帘，由 Adafruit TB6612 驱动板驱动（https://www.adafruit.com/product/2448）
// * 一个电机用于倾斜窗帘板条，由 SparkFun A3967 驱动板驱动（https://www.sparkfun.com/products/12779）

// 有关用于控制这些驱动板的类的详细信息，以及如何轻松扩展 StepperControl 以创建任何板的自定义驱动程序的说明，请参阅 HomeSpan 的 StepperControl 文档。

#include "HomeSpan.h"

////////////////////////////////////

struct DEV_WindowShade : Service::WindowCovering {

  Characteristic::CurrentPosition currentPos{0,true};
  Characteristic::TargetPosition targetPos{0,true};
  Characteristic::CurrentHorizontalTiltAngle currentTilt{0,true};
  Characteristic::TargetHorizontalTiltAngle targetTilt{0,true};
  
  StepperControl *mainMotor;          // 电机打开/关闭遮阳帘
  StepperControl *slatMotor;          // 电机倾斜遮阳板条

  DEV_WindowShade(StepperControl *mainMotor, StepperControl *slatMotor) : Service::WindowCovering(){

    this->mainMotor=mainMotor;                          // 保存指向电机的指针
    this->slatMotor=slatMotor;
           
    mainMotor->setAccel(10,20);                         //设置主电机加速度参数
    mainMotor->setStepType(StepperControl::HALF_STEP);  // 将主电机的步进类型设置为半步
        
    LOG0("Initial Open/Close Position: %d\n",currentPos.getVal());
    LOG0("Initial Slat Position: %d\n",currentTilt.getVal());
    
    mainMotor->setPosition(currentPos.getVal()*20);       //定义主电机的初始位置
    slatMotor->setPosition(currentTilt.getVal()*11.47);   // 定义板条电机的初始位置
  }

  ///////////
  
  boolean update(){

    if(targetPos.updated()){
      
      // 将电机移动到绝对位置，假设每转 400 步，全开/全关行程 5 转，全行程共计 2000 步。指定电机在到达所需位置时应进入制动状态。
      // 必须将 targetPos（范围从 0-100）乘以 20，以缩放到所需的电机步数
    
      mainMotor->moveTo(targetPos.getNewVal()*20,5,StepperControl::BRAKE);
      LOG1("Setting Shade Position=%d\n",targetPos.getNewVal());
    }

    if(targetTilt.updated()){
      
      // 将电机移至绝对位置，假设每转 2064 步，任一方向的板条倾斜全程为 1/2 转
     // 必须将 targetPos（范围从 -90 到 90）乘以 11.47 以缩放所需的电机步数
     // 请注意，此电机的驱动板不支持“短制动”状态
    
      slatMotor->moveTo(targetTilt.getNewVal()*11.47,5);
      LOG1("Setting Shade Position=%d\n",targetTilt.getNewVal());
    }

    return(true);
  }

  ///////////

  void loop(){

    // 如果当前窗帘位置或倾斜度不等于目标位置，但电机已停止移动，
    //我们一定已经到达目标位置，因此将当前位置设置为等于目标位置

    if(currentPos.getVal()!=targetPos.getVal() && !mainMotor->stepsRemaining()){
      currentPos.setVal(targetPos.getVal());
      LOG1("Main Motor Stopped at Shade Position=%d\n",currentPos.getVal());
    }

    if(currentTilt.getVal()!=targetTilt.getVal() && !slatMotor->stepsRemaining()){
      currentTilt.setVal(targetTilt.getVal());
      LOG1("Slat Motor Stopped at Shade Tilt=%d\n",currentTilt.getVal());
    } 
            
  }
  
};

////////////////////////////////////

void setup() {

  Serial.begin(115200);

  homeSpan.begin(Category::WindowCoverings,"Motorized Shade");

  // 确保更改下面的引脚编号以匹配您的 ESP32 设备！！！
  // 本示例中指定的引脚编号适用于原始 ESP32，但可能会导致 ESP32-S2、-S3 或 -C3 崩溃。

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
    new DEV_WindowShade(new Stepper_TB6612(23,32,22,14,33,27), new Stepper_A3967(18,21,5,4,19));   // 实例化每个开发板的驱动程序并指定 ESP32 上使用的引脚
}

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();  
}
