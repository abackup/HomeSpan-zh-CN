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

// 此示例演示了如何使用自定义分区方案文件：“partitions.csv”

// 在编译期间，如果将具有此确切名称的文件放在草图文件夹中，则执行编译的 esptool 将使用“partitions.csv”中的分区方案，无论您在 Arduino IDE 中选择了哪种分区方案。

// 请注意，如果您更改了分区方案，强烈建议您在下次编译/上传时通过从 Arduino IDE 菜单中启用“擦除所有闪存”选项完全擦除闪存。
// 注意：成功上传具有新分区方案的草图后，请记住关闭此选项，否则每次上传新草图时，您都会继续擦除 NVS 中保存的所有内容（这可能不是您想要发生的）。

// 想要创建自己的分区方案的主要原因是扩展 NVS 空间。您可以从 Arduino IDE 中选择的所有预配置分区方案都提供了 504 条 NVS 空间记录。对于大多数 HomeSpan 项目来说，
// 这通常已经足够了，但如果您有大量附件（如下所示）并且您正在 NVS 中保存它们的状态，则可能会用尽所有 NVS 空间。如果发生这种情况，HomeSpan 会在启动时警告您 NVS 空间不足。

// 此草图文件夹中包含的自定义分区方案通过消除
// SPIFFs 分区（HomeSpan 通常不使用）并使用闪存的这一部分
// 来提供具有 3906 条记录的 NVS 空间来解决此问题 --- 甚至对于最大的项目来说也绰绰有余。

// 作为参考，除了 HomeSpan 内部使用的 NVS（约 32 条记录）外，保存数字特征会消耗一条额外的 NVS 记录，保存字符串特征（少于 32 个字符）会消耗两条 NVS 记录。
// 此外，ESP32 WiFi 堆栈在初始化后会消耗大约 130 条额外的 NVS 记录。因此，下面的草图需要：

// 32 条记录（HomeSpan 内部使用）
// + 320 条记录（80 个附件 * 4 个保存的数字 Characterstic）
// + 160 条记录（80 个附件 * 每个保存的字符串 Characterstic 2 条记录）
// + 130 条记录（初始化 WiFi）
// ----------------------------------------
// = 需要 642 条 NVS 记录（超过正常的 504 条限制，除非使用自定义分区方案）

// 请注意，一旦 HomeSpan 与 HomeKit 配对，将使用额外的 NVS 记录来存储每个经过验证的 HomeKit 控制器的配对信息。

// 还要注意，在 Arduino IDE 下编译时，IDE 会根据您在 IDE 菜单中选择的分区方案报告分区大小，即使如果您有自己的“partition.csv”文件，实际上不会使用该方案，
// 如本例所示。这可能会导致 IDE 报告错误的分区大小。

///////////////////////////////////////////////////////////////////////////////////////////

#include "HomeSpan.h"

#define MAX_LIGHTS  80                            // 配置 80 个灯珠配件

struct RGB_Light : Service::LightBulb {

  Characteristic::On power{0,true};               // 保存这 4 个数值特征（4*80 = 320 个 NVS 记录）
  Characteristic::Hue H{0,true};
  Characteristic::Saturation S{0,true};
  Characteristic::Brightness V{0,true};

  int lightNumber;
  
  RGB_Light(int n) : Service::LightBulb(){

    lightNumber=n;
    LOG0("Configured RGB Light-%0d\n",lightNumber);           
  }

  boolean update(){

    if(power.updated())
      LOG0("Light-%d: Power=%s",lightNumber,power.getNewVal()?"ON":"OFF");
 
    if(H.updated())
      LOG0("Light-%d: Hue=%d",lightNumber,H.getNewVal());

    if(S.updated())
      LOG0("Light-%d: Saturation=%d",lightNumber,S.getNewVal());

    if(V.updated())
      LOG0("Light-%d: Brightness=%d",lightNumber,V.getNewVal());    
      
    return(false);
  
  }
};

void setup() {
 
  Serial.begin(115200);

  homeSpan.begin(Category::Lighting,"HomeSpan Max");

   new SpanAccessory();
    new Service::AccessoryInformation();  
      new Characteristic::Identify();

  for(int i=1;i<=MAX_LIGHTS;i++){
    char c[60];
    sprintf(c,"Light-%02d",i);
    
    new SpanAccessory();
      new Service::AccessoryInformation();
        new Characteristic::Identify();
        new Characteristic::Name(c,true);        // 保存此字符串特征（2*80 = 160 个 NVS 记录）
        
      new RGB_Light(i);
  }

}

//////////////////////////////////////

void loop(){
 
  homeSpan.poll();  
}
