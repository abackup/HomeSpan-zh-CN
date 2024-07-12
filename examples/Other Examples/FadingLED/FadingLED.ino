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

//HomeSpan 渐变 LED 示例。演示了以下用法：
//
// LedPin::fade() 和 LedPin::fadeStatus() 方法
//
// 在此草图中，我们使用 “家庭”应用以及 SpanButton 控制单个可调光 LED。
// 您可以从 “家庭”应用控制 LED 的亮度，但 SpanButton 只会将 LED 完全打开（如果已关闭）或完全关闭（如果已打开）。
//
// 此草图并未将 LED 设置为特定亮度，而是使用 ESP32 基于硬件的渐变功能将 LED 从一个级别渐变到下一个级别。我们将每次渐变的时间设置为 2000 毫秒，
// 与当前亮度和所需亮度之间的差异成比例。这意味着 LED 从 0 渐变到 100 需要整整 2 秒，但从半亮渐变到熄灭仅需 1 秒。

#include "HomeSpan.h" 

////////////////////////////////////

struct FadingLED : Service::LightBulb {
  
  LedPin *ledPin;                                   // 引用 Led Pin
  SpanCharacteristic *power;                        // 引用 On 特性
  SpanCharacteristic *level;                        // 参考亮度特征
  
  FadingLED(int _ledPin, int _buttonPin) : Service::LightBulb(){

    power=new Characteristic::On();     
    level=new Characteristic::Brightness(0);
    ledPin=new LedPin(_ledPin);    
    new SpanButton(_buttonPin);
    
  }

  boolean update(){
    
    ledPin->fade(power->getNewVal()*level->getNewVal(),2000,LedPin::PROPORTIONAL);      // 使用 fade() 设置新级别；计时 = 2 秒，比例缩放
    while(ledPin->fadeStatus()==LedPin::FADING);                                        // 等待淡出完成

    return(true);
  
  }

  void button(int pin, int pressType) override {

    // 下面我们根据电源是否打开来完全打开或关闭 LED，与上面不同，我们不会等待淡入淡出完成，而是立即返回
    
    if(ledPin->fade(100-(power->getVal())*100,2000,LedPin::PROPORTIONAL)!=0)            // 使用淡入淡出来完全打开或完全关闭；检查返回状态以查看调用是否成功
      Serial.printf("Button Press Ignored\n");
  }

  void loop() override {

    // 下面我们设置按钮按下完成后的功率和级别
    
    if(ledPin->fadeStatus()==LedPin::COMPLETED){
      power->setVal(1-power->getVal());
      level->setVal(power->getVal()?100:0);
    }
  }

};
      
//////////////////////////////////

void setup() {
  
  Serial.begin(115200);

  homeSpan.begin(Category::Lighting,"Fading LED");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
    
    new FadingLED(26,4);             // 第一个参数是 LED 引脚，第二个参数是 PushButton 引脚
 
}

//////////////////////////////////////

void loop(){
    
  homeSpan.poll(); 
}
