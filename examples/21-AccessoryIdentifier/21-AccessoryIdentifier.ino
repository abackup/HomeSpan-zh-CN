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
//              示例 21：使用识别特征                      //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

// 这个草图与示例 5 类似，其中我们实现了两个简单的灯泡配件，但现在我们还将添加识别特征的功能
// （我们还将设备配置为桥接配件，而不是两个独立的配件）。

// 回想一下，识别特征已在每个示例草图中实例化，因为它是附件信息服务的必需特征，并且该服务本身需要存在于每个附件中。
// 因此，每个附件（包括桥接附件，如果使用）都有自己的识别特征实例。

// 虽然在配件正常运行期间通常不会使用识别特征，但在首次将设备与 HomeKit 配对时，识别特征会很有用。
// 在使用“家庭”应用配对设备时，您可能已经注意到，每个屏幕底部都有“识别”字样，询问您要为每个配件命名什么，
// 配件应分配到哪个房间等。

// 在任意一个屏幕上点击“识别”都会导致 HomeKit 向与相应配件关联的识别特征发送更新请求。与通过“家庭”应用
// 更新的任何特征一样，这将触发对封闭服务的 update() 方法的调用。

// 这样做的目的是让您的设备在收到请求时能够运行某种“识别程序”，让您直观地确认您确实配对了正确的设备。例如，
// 如果您有三个独立的设备连接到三个不同的灯或电器，您需要确保当您开始将它们中的每一个与“家庭”应用配对时，您已连接到您想要的设备。

// 识别例程可以是您选择的任何程序。唯一的 HAP 要求是它运行时间不应超过 5 秒。在下面的草图中，我们创建了一个识别例程，
// 该例程将一条消息记录到串行监视器，并且每当其识别特征更新时，与附件关联的 LED 都会闪烁 3 次。

#include "HomeSpan.h" 

//////////////////////////////////////

// 下面是我们在之前的许多示例中使用的相同的 DEV_LED 灯泡服务

struct DEV_LED : Service::LightBulb {

  int ledPin;
  SpanCharacteristic *power;
  
  DEV_LED(int ledPin) : Service::LightBulb(){

    power=new Characteristic::On();
    this->ledPin=ledPin;
    pinMode(ledPin,OUTPUT);    
  }

  boolean update(){            
    digitalWrite(ledPin,power->getNewVal());
    LOG0("LED %d: Power %s\n",ledPin,power->getNewVal()?"ON":"OFF");
    return(true);
  }
};

//////////////////////////////////////

// 新功能：我们从附件信息服务中派生出一个新类，DEV_INFO

// 该结构采用单个参数（ledPin），从中创建一个名称，并将其分配给名称特征。

// 它还实例化所需的识别特征，并实现 update() 方法，该方法将消息记录到串行监视器并使相关 LED 闪烁三次。

// 请注意，在 update() 方法中我们不必检查哪个特征已被更新。
// 这是因为唯一的可能性是识别特征。

// 此外，我们不需要使用 getNewVal() 来检查值。“家庭”应用始终发送值 1，因为它只是试图触发识别例程（值本身没有意义）。

struct DEV_INFO : Service::AccessoryInformation {

  int ledPin;
  
  DEV_INFO(int ledPin) : Service::AccessoryInformation(){

    new Characteristic::Identify();
    char c[64];
    sprintf(c,"LED-%d",ledPin);
    new Characteristic::Name(c);               
    this->ledPin=ledPin;
    pinMode(ledPin,OUTPUT);    
  }

  boolean update(){
    LOG0("Running Identification for LED %d\n",ledPin);
    for(int i=0;i<3;i++){
      digitalWrite(ledPin,HIGH);
      delay(500);
      digitalWrite(ledPin,LOW);
      delay(500);
    }
    return(true);
  }
};

//////////////////////////////////////

void setup() {

  Serial.begin(115200);

  homeSpan.setLogLevel(1);
  homeSpan.begin(Category::Lighting,"HomeSpan LEDS");

// 在这里我们替换通常的构造：

// new SpanAccessory();
// new Service::AccessoryInformation();
// new Characteristic::Identify();

// 用这个：

  new SpanAccessory();  
    new DEV_INFO(13);         // 实例化一个新的 DEV_INFO 结构，它将运行我们的自定义识别例程，使引脚 13 上的 LED 闪烁三次

  new SpanAccessory();
    new DEV_INFO(16);         // 注意，我们为此设备中的每个配件实例化了一个新的 DEV_INFO 结构。
    new DEV_LED(16);          // 这里我们实例化了在正常运行期间控制 LED 的常用 DEV_LED 结构

  new SpanAccessory();        // 这里我们添加第二个 LED 配件
    new DEV_INFO(17);               
    new DEV_LED(17);    
}

//////////////////////////////////////

void loop(){ 
  homeSpan.poll();
}

//////////////////////////////////////

// 注意：设备配对后，将无法再从“家庭”应用触发识别特性。
// Apple 认为不再需要识别程序，因为您只需操作设备即可识别设备。
// 但是，Eve 应用应用程序确实在每个配件的界面中提供了一个“ID”按钮，可在设备配对后随时用于触发该配件的识别程序。
