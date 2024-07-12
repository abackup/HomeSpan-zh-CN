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
 
////////////////////////////////////////////////////////////
//                                                        //
//             HomeSpan：ESP32 的 HomeKit 实现            //
//    ------------------------------------------------    //
//                                                        //
//     演示如何使用 SpanPoint() 在不同的 ESP32             //
//     设备上实现两个远程温度传感器。                      //
//                                                        //
//     此草图适用于远程设备。它们非常简单，不需要          //
//     任何常规 HomeSpan 逻辑（SpanPoint 除外）。          //
//                                                        //
//     请注意，此草图仅通过以 0.5 C 的步长缓慢设置         //
//     温度从 -30.0 到 35.0 C 来模拟温度传感器。该         //
//     草图不包含实际物理温度传感器的逻辑。                //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h"

float temperature=-10.0;
SpanPoint *mainDevice;

void setup() {
  
  Serial.begin(115200);
  delay(1000);

  Serial.printf("\n\nThis is a REMOTE Device with MAC Address = %s\n",WiFi.macAddress().c_str());
  Serial.printf("NOTE: This MAC Address must be entered into the corresponding SpanPoint() call of the MAIN Device.\n\n");

  // 在下面的行中，将 MAC 地址替换为您的主 HOMESPAN 设备的 MAC 地址

  mainDevice=new SpanPoint("84:CC:A8:11:B4:84",sizeof(float),0);    // 创建一个 SpanPoint，发送大小 size=sizeof(float) 且接收大小 size=0    

  homeSpan.setLogLevel(1);
}

void loop() {

  boolean success = mainDevice->send(&temperature);                 // 只要主设备正在运行，这将显示成功
  Serial.printf("Send %s\n",success?"Succeded":"Failed");
  temperature+=0.5;
  if(temperature>35.0)
    temperature=-30.0;
   
  delay(20000);
}
