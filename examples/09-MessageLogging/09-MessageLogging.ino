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
//            示例 9：将消息记录到串口监视器                //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h" 
#include "DEV_LED.h"     

void setup() {

  // 每当设备连接到计算机时，HomeSpan 都会向 Arduino IDE 的串口监视器发送各种消息。消息输出可以通过常用的 Serial.print() 
  // 或 Serial.printf() 函数执行，也可以通过三个用户宏之一执行：LOG0()、LOG1() 和 LOG2()。这三个宏根据 HomeSpan 的日志
  // 级别设置将消息输出到串口监视器：

  // 设置为 0 时，仅输出 LOG0() 消息；LOG1() 和 LOG2() 消息被忽略
  // 设置为 1 时，LOG0() 和 LOG1() 消息都会输出；LOG2() 消息会被忽略
  // 设置为 2 时，LOG0()、LOG1() 和 LOG2() 消息都会输出

  // 示例 9 说明了如何添加此类日志消息。代码与示例 8 相同（没有注释），只是 LOG0() 和 LOG1() 消息已添加到 DEV_LED.h。
  //  LOG0() 消息将始终输出到 Arduino 串口监视器。仅当日志级别设置为 1 或 2 时，才会输出 LOG1() 消息。

  // homeSpan 的 setLogLevel() 方法可用于更改日志级别，如下所示：

  // homeSpan.setLogLevel(0) - 将日志级别设置为 0
  // homeSpan.setLogLevel(1) - 将日志级别设置为 1
  // homeSpan.setLogLevel(2) - 将日志级别设置为 2

  // 该方法应在 homeSpan.begin() 之前调用 - 请参阅下文以了解正确用法。请注意，还可以通过 HomeSpan CLI 在运行时动态更改日志级别，
  // 方法是在串口监视器中键入“L0”、“L1”或“L2”

  // LOG0()、LOG1() 和 LOG2() 宏有两种形式。第一种形式只接受一个参数，并使用 Serial.print(var) 函数输出消息。这允许您输出任何
  // 单个变量或文本消息，但不允许您控制格式或一次输出多个变量。第二种形式采用多个参数，其中第一个是标准 C++ 格式字符串，
  // 其余参数根据格式字符串使用。此形式使用可变参数 Serial.printf(char *fmt [,var1, var2...]) 函数。

  // 建议：由于 HomeSpan ESP32 旨在物理连接到现实世界的设备，因此您可能会发现自己拥有许多 ESP32，每个都配置了一组不同的附件。
  // 为了帮助识别，您可能需要将包含某种初始化消息的 LOG0() 语句添加到每个派生服务的构造函数中，例如 DEV_LED。这样做允许 HomeSpan 
  // 在启动时“报告”其配置。有关示例，请参阅 DEV_LED。
  
  Serial.begin(115200);

  homeSpan.setLogLevel(1);                                // NEW - 将日志级别设置为 1，这将导致输出 LOG1() 消息

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");
  
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
 
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
      new Characteristic::Name("Simple LED");
    new DEV_LED(16);

  new SpanAccessory();
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Dimmable LED");  
    new DEV_DimmableLED(17);                                                                                     

} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束
