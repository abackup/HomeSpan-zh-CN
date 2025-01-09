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
//     演示如何使用 SpanPoint() 在不同的                   //
//     ESP32 设备上实现两个远程温度传感器。                //
//                                                        //
//     该草图用于包含所有常见的 HomeSpan 逻辑的主设备，    //
//     以及两个 SpanPoint 实例，用于从另外两个远程         //
//     设备读取温度。                                      //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h"

struct RemoteTempSensor : Service::TemperatureSensor {

  SpanCharacteristic *temp;
  SpanCharacteristic *fault;
  SpanPoint *remoteTemp;
  const char *name;
  float temperature;
  
  RemoteTempSensor(const char *name, const char*macAddress, boolean is8266=false) : Service::TemperatureSensor(){

    this->name=name;
    
    temp=new Characteristic::CurrentTemperature(-10.0);      // 设置初始温度
    temp->setRange(-50,100);                                 // 扩大温度范围以允许负值

    fault=new Characteristic::StatusFault(1);                // 设置初始 state = fault

    remoteTemp=new SpanPoint(macAddress,0,sizeof(float),1,is8266);    // 创建一个 SpanPoint，其发送大小 size=0 ，接收大小 size=sizeof(float)

  } // 结束构造函数

  void loop(){
       
    if(remoteTemp->get(&temperature)){      // 如果有来自遥感器的数据
      temp->setVal(temperature);            // 更新温度
      fault->setVal(0);                     // 清除故障
       
      LOG1("Sensor %s update: Temperature=%0.2f\n",name,temperature*9/5+32);
      
    } else if(remoteTemp->time()>60000 && !fault->getVal()){    // 否则，如果自上次更新以来已经有一段时间了（60 秒），并且当前没有故障
      fault->setVal(1);                                         // 设置故障状态
      LOG1("Sensor %s update: FAULT\n",name);
    }
    
  } // 循环
  
};

//////////////////////////////////////

void setup() {
  
  Serial.begin(115200);

  homeSpan.setLogLevel(1);

  homeSpan.begin(Category::Bridges,"Sensor Hub");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Indoor Temp");
    new RemoteTempSensor("Device 1","BC:FF:4D:40:8E:71",true);        // 传递远程设备的 MAC 地址，并标记它是 ESP8266

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Outdoor Temp");
    new RemoteTempSensor("Device 2","84:CC:A8:11:B4:84");        // 传递远程设备的 MAC 地址

  
} // 设置结束()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // 循环结束()

//////////////////////////////////////
