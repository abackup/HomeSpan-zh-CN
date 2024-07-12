////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////

// 注意：此示例仅用于演示如何在 HomeSpan 中模拟按钮。闪烁例程的长度比 HomeSpan 在 update() 上花费的时间长得多。
// 要查看这对 HomeKit 的影响，请尝试将闪烁次数更改为 50，或将其保持在 3 次并增加 update() 中的延迟时间，
// 以便闪烁例程需要 10 秒或更长时间。激活后，如果设备在一定时间内未收到来自 update() 的返回消息，HomeKit 会认为设备已无响应。

// 实际上，按钮模拟用于非常短的例程，例如驱动红外 LED 或 RF 发射器将代码发送到远程设备。

// 与示例 9 相比，新增和更改的行标记为“新！”

struct DEV_Blinker : Service::LightBulb {           // LED 闪光灯

  int ledPin;                                       // 为此 LED 定义的引脚编号
  int nBlinks;                                      // 新功能！闪烁次数
  
  SpanCharacteristic *power;                        // 引用 On 特性
  
  DEV_Blinker(int ledPin, int nBlinks) : Service::LightBulb(){       // 构造函数（）方法

    power=new Characteristic::On();                 
        
    this->ledPin=ledPin;                            
    this->nBlinks=nBlinks;                          // 新功能！闪烁次数
    pinMode(ledPin,OUTPUT);                         
    
    Serial.print("Configuring LED Blinker: Pin=");   // 初始化消息
    Serial.print(ledPin);
    Serial.print("  Blinks=");                       // 新功能！添加闪烁次数的输出消息
    Serial.print(nBlinks);
    Serial.print("\n");

  } // 结束构造函数

  boolean update(){                              // update() 方法

    // 新功能！我们不再根据 newValue 打开或关闭 LED，而是按照指定的次数闪烁，并在完成后将其保持在关闭位置。此行已删除...
    
    // digitalWrite(ledPin,power->getNewVal());      

    // 并被替换为...

    if(power->getNewVal()){                       // 检查以确保 HomeKit 请求我们“打开”此设备（否则忽略）

      LOG1("Activating the LED Blinker on pin=");
      LOG1(ledPin);
      LOG1("\n");

      for(int i=0;i<nBlinks;i++){                     // 循环指定闪烁次数
        digitalWrite(ledPin,HIGH);                    // 打开引脚
        delay(100);                                   // 等待 100 毫秒
        digitalWrite(ledPin,LOW);                     // 关闭引脚
        delay(250);                                   // 等待 250 毫秒
      }
      
    } // 如果 newVal=true

    // 请注意，上述 100 毫秒和 250 毫秒的延迟仅用于说明目的（因此您可以看到 LED 闪烁）。
   // 实际上，如果您控制的是红外 LED 或 RF 发射器，则整个信号可能总共需要 10 毫秒才能传输完毕。
    
    return(true);                               // 返回 true
  
  } //  更新

  // 新功能！我们在这里实现一个非常简单的 loop() 方法，该方法检查电源特性是否至少处于“开启”状态 3 秒。如果是，则将值重置为“关闭”（false）。

  void loop(){

    if(power->getVal() && power->timeVal()>3000){   // 检查 power 是否正确，以及自上次修改以来的时间是否大于 3 秒
      LOG1("Resetting Blinking LED Control\n");     // 记录消息
      power->setVal(false);                         // 将 power 设置为 false
    }      
    
  } // loop
  
};
      
//////////////////////////////////

// HomeKit 错误说明：在开发此示例期间，发现了 HomeKit 中存在一个明显的错误。

// 如果您有一个具有三个或更多服务的配件，并且该配件从设备收到通知消息，并且 HomeKit 界面已打开以在 HomeKit 应用中显示此
// 服务图块的详细控件，那么出于某种原因，HomeKit 会将 update() 请求发送回设备，要求将特性设置为刚从事件通知中收到的值。

// HomeKit 不应该发送更新请求来响应事件通知。
