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

// 有时您需要从创建特性的服务结构外部访问特性，以便您可以在草图的其他部分（例如在主 Arduino 循环() 中）读取和/或修改它们。

// 此草图与教程示例 5 基本相同，其中我们创建了两个连接到引脚 16 和 17 的工作 LED。但是，在此草图中，我们将创建指向 LED 服务的全局指针，
// 然后我们可以在主循环() 中使用这些指针来执行一些独特的操作。

#include "HomeSpan.h" 

//////////////////////////////////////

// 首先，我们定义 DEV_LED 服务，与教程示例 5 中完全相同。
// 此服务包含一个名为“power”、类型为 Chacracteristic::On 的特性


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
    return(true);  
  }
};
   
//////////////////////////////////////

// 接下来，我们创建两个指向 DEV_LED 服务的指针。它们是在任何类或函数之外创建的，因此它们是全局范围的，可以从此草图中的任何其他位置访问。

// 请注意，只有指向 DEV_LED 对象的指针。对象本身尚未创建。

DEV_LED *led16;     // 指向下面要使用的 DEV_LED 结构的指针，用于引用分配给引脚 16 的 DEV_LED 对象
DEV_LED *led17;     // 指向下面要使用的 DEV_LED 结构的指针，用于引用分配给引脚 17 的 DEV_LED 对象

//////////////////////////////////////

void setup() {

  Serial.begin(115200);

  homeSpan.begin(Category::Lighting,"HomeSpan LED");
  
  new SpanAccessory();  
    new Service::AccessoryInformation(); 
      new Characteristic::Identify();                
    led16=new DEV_LED(16);                  // 这是关键的一步 - 我们将“new DEV_LED(16)”返回的指针保存在上面创建的全局变量 led16 中


  new SpanAccessory();   
    new Service::AccessoryInformation();    
      new Characteristic::Identify();                       
    led17=new DEV_LED(17);                  // 还使用上面创建的全局变量 led17 保存指向第二个 LED 对象的指针

}

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();

//因为指针 led16 和 led17 是在全局范围内创建的，所以即使在 setup() 完成后它们仍然存在。
// 这意味着我们可以使用它们来访问每个服务中的特性。

// 在这里，我们访问两个服务的电源特性，并检查它们是否都处于打开状态，如果是，我们将它们都关闭并打印“电源过载”消息。

// 请注意，您可以使用所有相同的方法，例如 getVal() 和 setVal()，就像在服务本身中一样。
// 注意：始终使用 getVal()，而不是 getNewVal()，后者仅在服务 update() 方法中正式定义。


  if(led16->power->getVal() && led17->power->getVal()){
    Serial.printf("Power overload!  Can't have both LED's on at the same time.  Turn off both LEDs...\n");
    led16->power->setVal(false);
    led17->power->setVal(false);
  }
  
}

//////////////////////////////////////
