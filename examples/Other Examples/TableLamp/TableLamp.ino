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

#include "HomeSpan.h"         // 包括 HomeSpan 库

struct TableLamp : Service::LightBulb{

  int lampPin;                               // 存储连接到假设的继电器的引脚号，该继电器可以打开/关闭台灯
  SpanCharacteristic *lampPower;             // 存储对 On Characteristic 的引用
  
  TableLamp(int lampPin) : Service::LightBulb(){       // TableLamp 的构造函数() 方法使用一个参数定义。请注意，我们还调用了 LightBulb 服务的构造函数() 方法。

    lampPower=new Characteristic::On();      // 实例化 On Characteristic 并将其保存为 lampPower
    this->lampPin=lampPin;                   // 保存假设继电器的引脚号
    pinMode(lampPin,OUTPUT);                 // 使用标准 Arduino pinMode 函数将引脚配置为输出
    
  } // 结束 constructor()
  
  boolean update(){                          // update() 方法

    digitalWrite(lampPin,lampPower->getNewVal());      // 使用标准 Arduino digitalWrite 函数根据 HomeKit 请求的值将 ledPin 更改为高或低
   
    return(true);                            // 返回 true 让 HomeKit（和“家庭”应用Client）知道更新成功
  
  } // 结束 update()
  
};

void setup() {     
 
  Serial.begin(115200);       //启动串行接口
  
  homeSpan.begin();           // 初始化HomeSpan

  new SpanAccessory();           // 台灯配件
  
    new Service::AccessoryInformation();            // HAP 要求每个配件都实现配件信息服务
      new Characteristic::Identify();               // HAP 要求附件信息服务包含识别特征
        
    new TableLamp(17);                              // 实例化 TableLamp 服务（定义如下），并将 lampPin 设置为 17
  
} // 结束 setup()

void loop(){

 homeSpan.poll(); 

} // 结束 loop()
