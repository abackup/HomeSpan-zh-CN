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
 
//////////////////////////////////////////////////////////////
//                                                          //
//              HomeSpan：ESP32 的 HomeKit 实现             //
//    ------------------------------------------------      //
//                                                          //
//     此草图用于远程温度传感器，与在单独的 ESP32            //
//     上运行的“MainDevice.ino”草图一起使用                  //
//                                                          //
//     这些草图的目的是演示如何使用 SpanPoint()              //
//     在从传感器获取测量值的远程 ESP32 设备与运行完整       //
//     HomeSpan 代码的单独“主”ESP32 设备之间进行通信，       //
//     从而连接到 HomeKit。                                 //
//                                                          //
//     此草图实现了 Adafruit ADT7410 I2C 温度传感器。        //
//     如果您没有这样的设备，请使用草图“RemoteDevice.ino”。  //
//     该草图模拟温度传感器，因此即使温度数据本身不是真实的， //
//     您也可以了解 SpanPoint() 的工作原理。                 //
//                                                          //
//////////////////////////////////////////////////////////////

#include "HomeSpan.h"
#include <Wire.h>           // 包括 I2C 库

#define DIAGNOSTIC_MODE

#define SAMPLE_TIME   30000     // 温度采样之间的时间（毫秒）
#define I2C_ADD       0x48      // 用于 Adafruit ADT7410 的 I2C 地址

SpanPoint *mainDevice;

void setup() {

  setCpuFrequencyMhz(80);       // 降低 CPU 频率以节省电池电量

#if defined(DIAGNOSTIC_MODE)  
  homeSpan.setLogLevel(1);
  Serial.begin(115200);
  delay(1000);
  Serial.printf("Starting Remote Temperature Sensor.  MAC Address of this device = %s\n",WiFi.macAddress().c_str());
#endif

  // 在下面的行中，将 MAC 地址替换为您的主 HOMESPAN 设备的 MAC 地址

  mainDevice=new SpanPoint("7C:DF:A1:61:E4:A8",sizeof(float),0);    // 创建一个 SpanPoint，发送大小 size=sizeof(float) 且接收大小 size=0    

  Wire.begin();                         // 在控制器模式下启动 I2C
  
#if defined(DIAGNOSTIC_MODE)  
  Wire.beginTransmission(I2C_ADD);      // 设置传输
  Wire.write(0x0B);                     // ADT7410 识别寄存器
  Wire.endTransmission(0);              // 传输并保持重启模式以允许读取
  Wire.requestFrom(I2C_ADD,1);          // 请求读取单个字节
  uint8_t id = Wire.read();             // 接收一个字节
  LOG1("Configuring Temperature Sensor ADT7410 version 0x%02X with address 0x%02X.\n",id,I2C_ADD);           // initialization message
#endif

  Wire.beginTransmission(I2C_ADD);      // 设置传输
  Wire.write(0x03);                     // ADT740 配置寄存器
  Wire.write(0xC0);                     // 设置16位温度分辨率，每秒1个样本
  Wire.endTransmission();               // 发送
          
  Wire.beginTransmission(I2C_ADD);      // 设置传输
  Wire.write(0x00);                     // ADT7410 2字节温度
  Wire.endTransmission(0);              // 传输并保持重启模式以允许读取
  Wire.requestFrom(I2C_ADD,2);          // 请求读取两个字节

  int16_t iTemp = ((int16_t)Wire.read()<<8)+Wire.read();    
  float temperature = iTemp/128.0;
  
  boolean success = mainDevice->send(&temperature);                 // 将温度发送到主设备
  
  LOG1("Send temp update of %0.2f F: %s\n",temperature*9/5+32,success?"Succeded":"Failed");  

  esp_deep_sleep(SAMPLE_TIME*1000);     // 进入深度睡眠模式——恢复后重新启动
}
