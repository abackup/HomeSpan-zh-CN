/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2020-2022 Gregg E. Berman
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
 
////////////////////////////////////////////////////////////
//                                                        //
//              HomeSpan：ESP32 的 HomeKit 实现           //
//    ------------------------------------------------    //
//                                                        //
//              示例 10：控制全彩 RGB LED                  //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h" 
#include "DEV_LED.h"     

void setup() {

  // 示例 10 说明如何控制 RGB LED 以设置任何颜色和亮度。
  // 下面的配置现在应该看起来很熟悉。我们创建了一个名为 DEV_RgbLED 的新派生服务来容纳所有必需的逻辑。
  // 您将在 DEV_LED.h 中找到所有代码。为了完整起见，此配置还包含一个开/关 LED 和一个可调光 LED，如前面的示例所示。
  
  Serial.begin(115200);

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
      new Characteristic::Name("Simple LED");
    new DEV_LED(16);                                                               // 创建一个连接到引脚 16 的开/关 LED

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Dimmable LED"); 
    new DEV_DimmableLED(17);                                                       // 使用连接到引脚 17 的 LED 创建可调光（PWM 驱动）LED

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("RGB LED"); 
    new DEV_RgbLED(32,22,23);                                                      // 创建一个连接到引脚 32、22、23 的 RGB LED（用于 R、G 和 B LED 阳极）
      
} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束
