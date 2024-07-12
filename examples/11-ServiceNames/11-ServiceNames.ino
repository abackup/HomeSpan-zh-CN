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
 
////////////////////////////////////////////////////////////////
//                                                            //
//                HomeSpan：ESP32 的 HomeKit 实现             //
//    ------------------------------------------------        //
//                                                            //
//             示例 11：服务名称：                             //
//                      * 设置各个服务的名称                   //
//                      * “更改”桥接附件中的图标               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "HomeSpan.h" 

  // 初始说明：Apple 不断更新 “家庭”应用图标的选择方式，以及如何/是否/在何处/何时显示配件和服务的名称。
 // 此示例已在 iOS 17.2.1 中经过测试和验证。

void setup() {

  // 如前面的示例所述，在配对设备时，“家庭”应用将为每个附件图块选择默认名称，除非您通过向每个附件
 // （第一个除外，通常是桥接附件）的配件信息服务添加名称特征，用自己的名称覆盖这些默认名称。

  // 对于具有多个服务的附件中的服务名称，相同的过程也适用：如果某个服务未命名，“家庭”应用将生成一个。
 // 当然，您可以在配对过程中出现提示时更改各个服务的名称，或者在配对后的任何时间从 “家庭”应用中的相应设置页面中更改名称。

  // 但更重要的是，您可以在草图中命名服务，以便在配对时显示这些名称，从而无需从 “家庭”应用中的设置页面重命名它们。

  // 我们之前使用 *Name* 特征为附件图块提供名称，而现在我们使用 *ConfiguredName* 特征为每个附件中的单个服务提供名称。

  // Name 和 ConfigureName 之间的一个重要区别是，Name 仅在配对期间由 “家庭”应用使用。此后，您在 “家庭”应用中
 // 对附件图块名称所做的任何更改都不会传达回 HomeSpan，并且除非您取消配对并重新配对设备，否则您在草图中对
 // 这些名称所做的任何更改都不会反映在 “家庭”应用中。相比之下，ConfiguredName 的工作方式与任何其他特性一样：
 // 在草图中对 ConfiguredName 所做的更改会传播到“家庭”应用，并且您在 “家庭”应用中对服务名称所做的任何编辑都会
 // 触发对 HomeSpan 中 update() 的相应调用，因此 HomeSpan 和 “家庭”应用始终与包含 ConfiguredName 特性的任何服务的名称保持同步。

  // 注意：服务名称（无论是由 “家庭”应用生成的还是通过 ConfiguredName 特性指定的）仅在存在两个以上相同类型的服务时才
 // 显示在附件图块的控制屏幕上。但即使服务名称未出现在 “家庭”应用中，Siri 仍会使用它通过语音控制配件中的特定服务。

  // 在下面的示例中，我们创建了 5 个不同的功能配件，每个配件都说明了 “家庭”应用如何选择名称和图标


  Serial.begin(115200);

  // 此设备将配置为网桥，类别设置为网桥

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");

  // 因此，我们的初始配件是“桥梁”配件 
  
  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 

  // 我们的第一个“功能性”配件是灯泡、插座和开关的组合。请注意，配对时，“家庭”应用会为这三个服务生成默认名称“灯”、“插座”和“开关”，
  // 但由于每个服务只有一种类型，因此这些名称不会显示在配件的控制屏幕上。另请注意，“家庭”应用为配件磁贴选择了一个灯泡图标

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Light First");       // 设置附件图块的名称
    new Service::LightBulb();                       //附件图块的图标将是一个灯泡，因为这是第一个功能服务
      new Characteristic::On();
    new Service::Outlet();
      new Characteristic::On();
      new Characteristic::OutletInUse();
    new Service::Switch();
      new Characteristic::On();

  // 我们的第二个配件与第一个配件类似，但在这里我们首先定义 Switch 服务。请注意，“家庭”应用现在为配件磁贴选择一个 Switch 图标

    new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Switch First");      // 设置附件图块的名称
    new Service::Switch();                           //附件图块的图标将是一个 Switch，因为这是第一个功能性服务
      new Characteristic::On();      
    new Service::Outlet();
      new Characteristic::On();
      new Characteristic::OutletInUse();
    new Service::LightBulb();
      new Characteristic::On();

  // 我们的第三个配件与第二个配件类似，但这里我们定义了 2 个开关、2 个灯泡，但仍然只有 1 个插座。这次，在配对期间，
  // “家庭”应用会生成默认名称 Switch、Switch 2、Light、Light 2 和 Outlet。重要的是，请注意，在此配件的控制屏幕上，
  // “家庭”应用现在在每个相应控件下显示开关的名称（“Switch”和“Switch 2”）以及灯泡的名称（“Light”和“Light 2”），
  // 但它不会在 Outlet 控件下显示名称“Outlet”，因为只有一个 Outlet 服务

    new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Two Switches");      // 设置附件图块的名称
    new Service::Switch();                           //附件图块的图标将是一个 Switch，因为这是第一个功能性服务
      new Characteristic::On();      
    new Service::Switch();
      new Characteristic::On();      
    new Service::Outlet();
      new Characteristic::On();
      new Characteristic::OutletInUse();
    new Service::LightBulb();
      new Characteristic::On();
    new Service::LightBulb();
      new Characteristic::On();

  // 我们的第四个也是最后一个配件与第三个相同，但这次我们使用 ConfiguredName 特性来为每个服务定义一个名称。

  // 配对时，您应该看到 “家庭”应用现在使用下面的名称，而不是像其他示例中那样生成默认名称。您还应该看到这些名称显示在配件控制屏幕上的每个控件下，但插座服务除外。

  // 虽然我们确实为插座提供了一个名称，但由于此配件中只有一个插座服务，因此 “家庭”应用不会显示其名称。

  // 但是，如果您从此配件的设置屏幕进一步导航到“配件”页面，您确实会看到每个服务的名称与下面指定的完全一致，包括插座名称“Aux Power”
  
    new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Central Control");    // 设置附件图块的名称
    new Service::Switch();                            //附件图块的图标将是一个 Switch，因为这是第一个功能性服务
      new Characteristic::On();
      new Characteristic::ConfiguredName("High Voltage");   // 这将设置第一个 Switch 服务的名称
    new Service::Switch();
      new Characteristic::On();      
      new Characteristic::ConfiguredName("Low Voltage");    // 这将设置第二个 Switch 服务的名称
    new Service::Outlet();
      new Characteristic::On();
      new Characteristic::OutletInUse();
      new Characteristic::ConfiguredName("Aux Power");      // 设置 Outlet Service 的名称
    new Service::LightBulb();
      new Characteristic::On();
      new Characteristic::ConfiguredName("Main Lights");    // 这将设置第一个 LightBulb 服务的名称
    new Service::LightBulb();
      new Characteristic::On();
      new Characteristic::ConfiguredName("Accent Lights");  // 这将设置第二个 LightBulb 服务的名称


} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束
