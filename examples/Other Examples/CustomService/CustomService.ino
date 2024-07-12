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
 
#include "HomeSpan.h" 

// Apple 的 HomeKit 不提供任何用于测量大气压力的原生服务或特性。

// 但是，Eve 应用支持压力测量。

// 这个简短的草图演示了如何使用 HomeSpan 的自定义服务和自定义特性功能来创建压力传感器附件，该附件将被 Eve 应用识别。请注意，
// Apple“家庭”应用会将其显示为“不支持”附件图块，表示它无法在“家庭”应用中使用。但是，这不会在“家庭”应用中造成任何问题或错误。

// 步骤 1：

// 使用 CUSTOM_SERV 宏创建一个名为 AtmosphericPressureSensor 的新服务，其 UUID=E863F00A-079E-48FF-8F27-9C2605A29F52。此新服务将添加到 HomeSpan 的服务命名空间中，
// 可以使用完全限定名称 Service::AtmosphericPressureSensor 进行访问。指定的 UUID 不会被 Apple 的 “家庭”应用识别，但会被 Eve 应用识别。请注意，不要将任何一个参数括在引号中！

  CUSTOM_SERV(AtmosphericPressureSensor, E863F00A-079E-48FF-8F27-9C2605A29F52);

// 步骤 2：

// 使用 CUSTOM_CHAR 宏创建一个名为 AtmosphericPressure 的新特性，其 UUID=E863F10F-079E-48FF-8F27-9C2605A29F52。此新特性将添加到 HomeSpan 的特征命名空间，可以使用完全限定名称 Characteristic::AtmosphericPressure 进行访问。Apple 的 “家庭”应用无法识别指定的 UUID，但 Eve 应用可以识别。请注意，不要将任何参数括在引号中！
//
// 参数含义如下：
//
// PR+EV：设置“读取”和“通知”权限
// FLOAT：将格式设置为浮点十进制数
// 1013：将默认起始值​​设置为 1013，即 1 atm 毫巴
// 700：将默认允许值下限设置为 700 毫巴
// 1200：将默认允许值上限设置为 1200 毫巴
// false：将“静态范围”标志设置为 false，表示用户可以根据需要覆盖默认范围 setRange()


  CUSTOM_CHAR(AtmosphericPressure, E863F10F-079E-48FF-8F27-9C2605A29F52, PR+EV, FLOAT, 1013, 700, 1200, false);

// 现在已经创建了 AtmosphericPressureSensor 和 AtmosphericPressure，它们可以像任何其他本机 HomeSpan 服务和特性一样使用。

//////////////////////////////////////

struct PressureSensor : Service::AtmosphericPressureSensor {         // 独立气压传感器

  Characteristic::AtmosphericPressure pressure;                      // Eve 气压范围为 700-1200 hPa（毫巴），其中 1 atm=1013 hPa
  
  PressureSensor() : Service::AtmosphericPressureSensor{} {
        
    Serial.print("Configuring Air Pressure Sensor");                 // 初始化消息
    Serial.print("\n");

  } // 结束构造函数

  void loop(){

    if(pressure.timeVal()>5000)                       // 这里我们通过每 5 秒生成一个随机压力读数来模拟实际的传感器
      pressure.setVal((double)random(900,1100));
       
  } // 结束循环

}; // 结束压力传感器

//////////////////////////////////////

void setup() {
 
  Serial.begin(115200);

  homeSpan.begin(Category::Sensors,"Eve Air Pressure");

  SPAN_ACCESSORY();  
    new PressureSensor();      

}

//////////////////////////////////////

void loop(){ 

  homeSpan.poll();  
}
