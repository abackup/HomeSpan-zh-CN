////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////

struct DEV_DimmableLED : Service::LightBulb {       // 可调光 LED

  // 此版本的可调光 LED 服务包括一个按钮，可用于打开/关闭 LED。
  // 电源状态和 LED 的亮度存储在 NVS 中，以便在设备重启时恢复。

  LedPin *LED;                                      // 引用 LedPin
  SpanCharacteristic *power;                        // 引用 On 特性
  SpanCharacteristic *level;                        // 参考亮度特征
 
  DEV_DimmableLED(int ledPin, int buttonPin) : Service::LightBulb(){

    power=new Characteristic::On(0,true);           // 新功能！第二个参数为真，因此 On特征的值（初始设置为 0）将保存在 NVS 中
    level=new Characteristic::Brightness(5,true);   // 新功能！第二个参数为真，因此亮度特征值（初始设置为 5）将保存在 NVS 中
    level->setRange(5,100,1);                       // 将亮度范围设置为最小 5% 到最大 100%，步长为 1%

    new SpanButton(buttonPin);                      // 创建一个新的 SpanButton，使用引脚号“buttonPin”上的 PushButton 来控制电源

    this->LED=new LedPin(ledPin);                   // 配置 PWM LED 以输出至引脚号“ledPin”

    Serial.print("Configuring Dimmable LED: Pin="); // 初始化消息
    Serial.print(LED->getPin());
    Serial.print("\n");

    LED->set(power->getVal()*level->getVal());      // 新功能！重要提示：在启动时将 LED 设置为其初始状态。请注意，我们在此处使用 getVal()，因为它是在实例化时设置的。
  } // 结束构造函数

  boolean update(){                              // update() 方法

    LOG1("Updating Dimmable LED on pin=");
    LOG1(LED->getPin());
    LOG1(":  Current Power=");
    LOG1(power->getVal()?"true":"false");
    LOG1("  Current Brightness=");
    LOG1(level->getVal());
  
    if(power->updated()){
      LOG1("  New Power=");
      LOG1(power->getNewVal()?"true":"false");
    }

    if(level->updated()){
      LOG1("  New Brightness=");
      LOG1(level->getNewVal());
    } 

    LOG1("\n");
    
    LED->set(power->getNewVal()*level->getNewVal());        // 更新物理 LED 以反映新值
    return(true);                               // 返回 true
  
  } //  更新

  void button(int pin, int pressType) override {

    if(pressType==SpanButton::SINGLE){                      // 仅响应单次按下
      power->setVal(1-power->getVal());                     // 切换电源特性的值
      LED->set(power->getVal()*level->getVal());            // 更新物理 LED 以反映新值
    }   

  } // button

};
      
//////////////////////////////////
