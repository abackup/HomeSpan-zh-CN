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
 
// 此示例演示了如何使用 HomeSpan 的 ServoPin 类（包含在“extras/PwmPin.h”中）控制真实世界的伺服电机。
// 该代码基于示例 13 中的 WindowShade 配件，通过添加由连接到 ESP32 的伺服器控制的水平倾斜特性。

#include "HomeSpan.h" 
#include "DEV_DoorsWindows.h" 

void setup() {

  Serial.begin(115200);

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Window Shade");
    new DEV_WindowShade(18);                                                            // 创建一个电动窗帘，并将伺服器连接到引脚 18，以控制窗帘的水平倾斜度

} // 设置结束()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // 循环结束()
