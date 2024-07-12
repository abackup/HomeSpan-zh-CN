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
//              示例 7：更改配件的默认名称以区分            //
//                     开/关和可调光 LED                   //
//                                                        //
////////////////////////////////////////////////////////////


#include "HomeSpan.h" 
#include "DEV_LED.h"          

void setup() {

  // 如前面的示例所述，“家庭”应用会根据 homeSpan.begin() 的第二个参数中提供的名称自动为每个附件图块生成默认名称。
  // 虽然您可以在配对期间和配对后的任何时间更改这些名称，但 HAP 还允许您自定义默认名称本身，因此在设备首次配对时会向用户呈现更直观的内容。


  // 更改配件的默认名称是通过向配件信息服务添加可选名称特征来完成的。这会导致 “家庭”应用使用该特征的值作为配件图块的默认名称，
  // 而不是根据 homeSpan.begin() 中使用的名称生成名称。


  // 但是，有一个警告：名称特征在设备的第一个配件中使用时不起作用。相反，无论名称特征是否已添加到配件信息服务，
  // “家庭”应用始终会将第一个配件图块的默认名称显示为 homeSpan.begin() 中指定的名称。


  // 下面是示例 6 的重放，展示了如何使用名称特征来更改第二个和第三个附件图块的默认名称，但不能更改第一个附件图块的默认名称。
  
  Serial.begin(115200);

  homeSpan.begin(Category::Lighting,"HomeSpan LED");    // 请注意，这会导致每个附件块的默认名称为“HomeSpan LED”、“HomeSpan LED 2”等
  
  new SpanAccessory(); 
  
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
      new Characteristic::Name("Simple LED");     // “家庭”应用将忽略此 Name() 的使用。配件的默认名称将继续显示为“HomeSpan LED” 

    new DEV_LED(16);

  new SpanAccessory(); 
  
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Dimmable LED");   // 这确实会将配件的默认名称从“HomeSpan LED 2”更改为“可调光 LED”
  
    new DEV_DimmableLED(17);

  new SpanAccessory(); 
  
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name(u8"Special chars ÄÖÜß");  // 对非 ASCII 字符使用 UTF-8 编码字符串
  
    new DEV_DimmableLED(18);

} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束
