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
 
 // HomeSpan 电视服务示例

// 涵盖 iOS 15 版“家庭”应用中似乎支持的电视服务的所有特性。请注意，这些服务未由 Apple 记录，并且不是 HAP-R2 的正式组成部分。
//
// 对于 Service::Television():
//
//    * Characteristic::Active()
//    * Characteristic::ConfiguredName()
//    * Characteristic::ActiveIdentifier()
//    * Characteristic::RemoteKey()
//    * Characteristic::PowerModeSelection()
//
// 对于 Service::InputSource():
//
//    * Characteristic::ConfiguredName()
//    * Characteristic::ConfiguredNameStatic()        //HomeSpan 特定的 ConfiguredName() 变体
//    * Characteristic::Identifier()
//    * Characteristic::IsConfigured()
//    * Characteristic::CurrentVisibilityState()
//    * Characteristic::TargetVisibilityState()

// 注意：此示例仅用于演示电视服务和特征在 Home 应用中的显示方式以及它们各自控制的内容。为简单起见，
// 以下代码中未实现输入源的操作。例如，以下代码不包含任何在点击 TargetVisibilityState 复选框时更新
// CurrentVisibilityState 的逻辑。

#include "HomeSpan.h"

struct HomeSpanTV : Service::Television {

    SpanCharacteristic *active = new Characteristic::Active(0);                     // 电视开/关（启动时设置为关闭）
    SpanCharacteristic *activeID = new Characteristic::ActiveIdentifier(3);         // 启动时设置 HDMI 3
    SpanCharacteristic *remoteKey = new Characteristic::RemoteKey();                // 用于接收来自远程控制小部件的按钮按下
    SpanCharacteristic *settingsKey = new Characteristic::PowerModeSelection();     // 在选择屏幕中添加“查看电视设置”选项

    HomeSpanTV(const char *name) : Service::Television() {
      new Characteristic::ConfiguredName(name);             // 电视名
      Serial.printf("Configured TV: %s\n",name);
    }

    boolean update() override {

      if(active->updated()){
        Serial.printf("Set TV Power to: %s\n",active->getNewVal()?"ON":"OFF");
      }

      if(activeID->updated()){
        Serial.printf("Set Input Source to HDMI-%d\n",activeID->getNewVal());        
      }

      if(settingsKey->updated()){
        Serial.printf("Received request to \"View TV Settings\"\n");
      }
      
      if(remoteKey->updated()){
        Serial.printf("Remote Control key pressed: ");
        switch(remoteKey->getNewVal()){
          case 4:
            Serial.printf("UP ARROW\n");
            break;
          case 5:
            Serial.printf("DOWN ARROW\n");
            break;
          case 6:
            Serial.printf("LEFT ARROW\n");
            break;
          case 7:
            Serial.printf("RIGHT ARROW\n");
            break;
          case 8:
            Serial.printf("SELECT\n");
            break;
          case 9:
            Serial.printf("BACK\n");
            break;
          case 11:
            Serial.printf("PLAY/PAUSE\n");
            break;
          case 15:
            Serial.printf("INFO\n");
            break;
          default:
            Serial.print("UNKNOWN KEY\n");
        }
      }

      return(true);
    }
};

///////////////////////////////

void setup() {
  
  Serial.begin(115200);
 
  homeSpan.begin(Category::Television,"HomeSpan Television");

  SPAN_ACCESSORY();

  // 下面我们使用不同的特征组合来定义 10 个不同的 InputSource 服务，
  // 以演示它们如何在“家庭”应用中交互并向用户显示
       
  SpanService *hdmi1 = new Service::InputSource();    // 源包含在选择列表中，但排除在设置屏幕之外
    new Characteristic::ConfiguredName("HDMI 1");
    new Characteristic::Identifier(1);

  SpanService *hdmi2 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 2");
    new Characteristic::Identifier(2);
    new Characteristic::IsConfigured(0);              // 从选择列表和设置屏幕中排除的源

  SpanService *hdmi3 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 3");
    new Characteristic::Identifier(3);
    new Characteristic::IsConfigured(1);              // 源包含在选择列表和设置屏幕中

  SpanService *hdmi4 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 4");
    new Characteristic::Identifier(4);
    new Characteristic::IsConfigured(1);              // 源包含在设置屏幕中...
    new Characteristic::CurrentVisibilityState(1);    // ...但被排除在选择名单之外

  SpanService *hdmi5 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 5");
    new Characteristic::Identifier(5);
    new Characteristic::IsConfigured(1);              // 源包含在设置屏幕中...
    new Characteristic::CurrentVisibilityState(0);    // ...并列入选择清单

  SpanService *hdmi6 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 6");
    new Characteristic::Identifier(6);
    new Characteristic::IsConfigured(0);              // 从选择列表和设置屏幕中排除的源
    new Characteristic::CurrentVisibilityState(0);    // 如果指定了 IsConfigured(0)，则 CurrentVisibilityState() 无效

  SpanService *hdmi7 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 7");
    new Characteristic::Identifier(7);
    new Characteristic::IsConfigured(1);              // 源包含在设置屏幕中...
    new Characteristic::CurrentVisibilityState(0);    // ...并列入选择列表中...
    new Characteristic::TargetVisibilityState(0);     // ...并且在设置屏幕上提供了一个“选中”复选框，可用于切换 CurrentVisibilityState()

  SpanService *hdmi8 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 8");
    new Characteristic::Identifier(8);
    new Characteristic::IsConfigured(1);              // 源包含在设置屏幕中...
    new Characteristic::CurrentVisibilityState(1);    // ...但被排除在选择名单之外......
    new Characteristic::TargetVisibilityState(1);     // ...并且在设置屏幕上提供了一个“未选中”复选框，可用于切换 CurrentVisibilityState()
   
  SpanService *hdmi9 = new Service::InputSource();
    new Characteristic::ConfiguredName("HDMI 9");
    new Characteristic::IsConfigured(1);              // 源包含在设置屏幕中...
    new Characteristic::CurrentVisibilityState(0);    // ...但若未设置 Identifier()，则无论 CurrentVisibilityState（0）如何，源都会被排除在选择列表中
    new Characteristic::TargetVisibilityState(0);
    
  SpanService *hdmi10 = new Service::InputSource();
    (new Characteristic::ConfiguredName("HDMI 10"))->removePerms(PW);    // 源名称权限已更改，现在无法在设置屏幕中编辑
    new Characteristic::Identifier(10);
    new Characteristic::IsConfigured(1);              // 源包含在设置屏幕中...
    new Characteristic::CurrentVisibilityState(0);    // ...并列入选择列表中...
    new Characteristic::TargetVisibilityState(0);     // ...并且在设置屏幕上提供了一个“选中”复选框，可用于切换 CurrentVisibilityState()

  SpanService *speaker = new Service::TelevisionSpeaker();
    new Characteristic::VolumeSelector();
    new Characteristic::VolumeControlType(3);

  (new HomeSpanTV("Test TV"))                         // 定义电视服务。必须链接到输入源！
    ->addLink(hdmi1)
    ->addLink(hdmi2)
    ->addLink(hdmi3)
    ->addLink(hdmi4)
    ->addLink(hdmi5)
    ->addLink(hdmi6)
    ->addLink(hdmi7)
    ->addLink(hdmi8)
    ->addLink(hdmi9)
    ->addLink(hdmi10)
    ->addLink(speaker)
    ;
      
}

///////////////////////////////

void loop() {
  homeSpan.poll();
}
