/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2021-2024 Gregg E. Berman
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
//          示例 18：在 NVS 中保存特性状态                 //
//                  * 保存两个可调光 LED 的状态            //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h" 
#include "DEV_LED.h"     

void setup() {

  // 在之前的许多示例中，我们看到了在首次实例化时如何初始化 Characteristics。您可以包含一个参数：new Characteristic::Brightness(25); 
  // 在这种情况下，当 HomeSpan 通电时，亮度 Characterisrtic 的值设置为 25，或者您可以将参数留空：new Characteristic::Brightness(); 
  // 在这种情况下，HomeSpan 将应用默认值。

  // 这些方法工作正常，但如果 HomeSpan 设备断电，它将根据上述参数启动，而不是在您通过 “家庭”应用或任何 PushButtons 进行任何更改后记住每个特征的状态。

  // 在此示例 18 中，我们将了解如何指示 HomeSpan 自动将一个或多个 Characteristics 的值保存在非易失性存储器 (NVS) 中，以便在断电时将它们恢复到最新状态。
  // 为此，我们调用特征的构造函数，其中包含两个参数，如下所示：new Characteristic::Brightness(25, true);这指示 HomeSpan 在设备首次通电时将
  // 亮度设置为 25，但要保存 NVS 中对此特性的任何更改，并在每次通电时恢复上次保存的值！

  // 请注意，尽管 HomeSpan 会自动保存和恢复任何将构造函数的第二个参数设置为“true”的特性，但 HomeSpan 无法自动执行物理设备所需的任何初始化。换句话说，
  // 如果您从 “家庭”应用将亮度更改为 55，然后一段时间后设备断电，HomeSpan 将在启动时将亮度特性的值恢复为 55，但您需要添加一些代码来在恢复值后设置实际 LED 的亮度。

  // 为了了解这在实践中是如何工作的，我们将配置 HomeSpan 来操作两个可调光 LED，每个 LED 都有自己的开/关按钮。与往常一样，所有代码都在 DEV_LED.h 中实现，
  // 注释突出显示了所有新功能。请参阅 DEV_LED.h 了解详细信息。

  
  Serial.begin(115200);

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("LED 1");   
    new DEV_DimmableLED(17,19);                                         // 第一个参数指定 LED 引脚；第二个参数指定 PushButton 引脚
 
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("LED 2");  
    new DEV_DimmableLED(16,18);                                         // 第一个参数指定 LED 引脚；第二个参数指定 PushButton 引脚

} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束

//////////////////////////////////////

// 操作说明
//
// 当特性值保存在 NVS 中时，它们会基于将特性的 UUID 与其 AID 和 IID 相结合的唯一密钥进行存储。如果您正在积极开发配置，
// 添加或减去新的 SpanAccessory 或 SpanService 可能会改变其他特性的 AID 和 IID，这些特性的值已存储在 NVS 中。
// 如果未使用新的 UUID/AID/IID 组合，则不会在下次启动时恢复先前存储的值，而是使用构造函数第一个参数中指定的值并将其存储在 NVS 中作为初始值。
//
// 如果新的 UUID/AID/IID 恰好与先前使用的组合匹配，则特性的值将恢复为在 NVS 中该密钥下找到的任何值。
//
// *** 要清除存储在 NVS 中的所有值，请在 HomeSpan CLI 中键入“V”。这可确保 NVS 中不存在来自您配置的先前迭代的杂散键/值对。
//
