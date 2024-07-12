/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2024 Gregg E. Berman
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
//           示例 19：带计时功能的 Web 日志记录             //
//                                                        //
////////////////////////////////////////////////////////////


#include "HomeSpan.h" 
#include "DEV_LED.h"

void setup() {

// 这是示例 5（两个工作 LED）的副本，并添加了 HomeSpan Web 日志记录

  Serial.begin(115200);

// 下面我们启用 Web 日志。第一个参数设置要保存的最大日志消息数。随着日志中消息的增加，较旧的消息将被较新的消息替换。
// 第二个参数指定 HomeSpan 调用以设置设备时钟的计时器服务器。设置时钟是可选的，如果您不关心设置设备的绝对时间，
// 则可以将此参数留空（或设置为 NULL）。第三个参数定义用于设置设备时钟的时区。第四个参数指定 Web 日志的 URL 页面。
// 有关与此函数调用相关的完整详细信息以及其他选项，请参阅 HomeSpan API 参考。

  homeSpan.enableWebLog(10,"pool.ntp.org","UTC","myLog");           // 在 URL /HomeSpan-[DEVICE-ID].local:[TCP-PORT]/myLog 上创建网络日志

// T启动时，串口监视器中将显示 Web 日志的完整 URL，以供参考。
// Web 日志输出显示各种设备参数，以及您选择使用 WEBLOG() 宏提供的任何日志消息（参见 DEV_LED.h）

//请注意，下面的其余草图与示例 5 相同。所有 Web 日志记录都发生在 DEV_LED.h 中
  
  homeSpan.begin(Category::Lighting,"HomeSpan LEDs");           //请注意，即使设备配置为桥接器，我们也可以将类别设置为照明
  
  new SpanAccessory(); 
    new Service::AccessoryInformation(); 
      new Characteristic::Identify();               

  new SpanAccessory(); 
    new Service::AccessoryInformation(); 
      new Characteristic::Identify();               
      new Characteristic::Name("LED #1"); 
    new DEV_LED(16);

  new SpanAccessory();   
    new Service::AccessoryInformation();    
      new Characteristic::Identify();                       
      new Characteristic::Name("LED #2");      
    new DEV_LED(17);

} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束
