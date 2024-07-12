////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////

struct DEV_DimmableLED : Service::LightBulb {       // 可调光 LED

  // 此版本的可调光 LED 服务与示例 11 中使用的最后一个版本类似，但现在支持 3 个物理按钮，可执行以下操作：
  // 
  // 电源按钮：短按可打开/关闭电源；长按可将当前亮度保存为收藏级别；双击可将亮度设置为收藏级别
  // 升高按钮：短按可将亮度增加 1%；长按可将亮度增加 10%；双击可将亮度增加至最大
  // 降低按钮：短按可将亮度降低 1%；长按可将亮度降低 10%；双击可将亮度降低至最小


  LedPin *ledPin;                                   // 引用 Led Pin
  int powerPin;                                     // 新功能！带按钮的引脚可打开/关闭 LED
  int raisePin;                                     // 新功能！带按钮的插针可增加亮度
  int lowerPin;                                     // 新功能！带按钮的引脚可降低亮度
  SpanCharacteristic *power;                        // 引用 On 特性
  SpanCharacteristic *level;                        // 参考亮度特征
  int favoriteLevel=50;                             // 新功能！跟踪“最喜爱”的级别

  // 新功能！Construction 包含 3 个附加参数，用于指定电源、升高和降低按钮的引脚编号
  
  DEV_DimmableLED(int pin, int powerPin, int raisePin, int lowerPin) : Service::LightBulb(){

    power=new Characteristic::On();     
                
    level=new Characteristic::Brightness(favoriteLevel);       // 亮度特征的初始值等于喜爱程度
    level->setRange(5,100,1);                                  // 将亮度范围设置为最小 5% 到最大 100%，步长为 1%

    // 新功能！下面我们创建三个 SpanButton() 对象。在第一个对象中，我们根据需要指定引脚号，但允许 SpanButton() 使用其默认值，即长按（2000 毫秒）、单击（5 毫秒）和双击（200 毫秒）。在第二个和第三个对象中，我们将默认长按时间更改为 500 毫秒，这非常适合反复增加或减少亮度。由于我们没有指定触发器类型，SpanButton 使用默认的 TRIGGER_ON_TOUCH，这适用于按下时将引脚连接到接地的按钮。

    // 增加/减少亮度、打开/关闭电源以及设置/重置喜欢的亮度级别的所有逻辑都可以在下面的 button() 方法中找到。

    new SpanButton(powerPin);                       // 新功能！创建新的 Span 按钮，使用引脚号“powerPin”上的按钮来控制电源
    new SpanButton(raisePin,500);                   // 新功能！创建新的 SpanButton，使用引脚号“raisePin”上的按钮增加亮度
    new SpanButton(lowerPin,500);                   // 新功能！创建新的 SpanButton，使用引脚号“lowerPin”上的按钮降低亮度

    this->powerPin=powerPin;                        // 新功能！保存电源按钮引脚号
    this->raisePin=raisePin;                        // 新功能！保存增加亮度按钮引脚号
    this->lowerPin=lowerPin;                        // 新功能！保存降低亮度按钮引脚号
    this->ledPin=new LedPin(pin);                   // 配置 PWM LED 以输出至指定引脚

    Serial.print("Configuring Dimmable LED: Pin="); // 初始化消息
    Serial.print(ledPin->getPin());
    Serial.print("\n");
    
  } // 结束构造函数

  boolean update(){                              // update() 方法

    LOG1("Updating Dimmable LED on pin=");
    LOG1(ledPin->getPin());
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
    
    ledPin->set(power->getNewVal()*level->getNewVal());    
   
    return(true);                               // 返回 true
  
  } //  更新

  // 新功能！这是 button() 方法，其中定义了所有 PushButton 操作。请注意签名和“override”一词的使用

  void button(int pin, int pressType) override {

    LOG1("Found button press on pin: ");            // 记录消息总是一个好主意
    LOG1(pin);
    LOG1("  type: ");
    LOG1(pressType==SpanButton::LONG?"LONG":(pressType==SpanButton::SINGLE)?"SINGLE":"DOUBLE");
    LOG1("\n");

    int newLevel;

    if(pin==powerPin){
      if(pressType==SpanButton::SINGLE){            // 如果仅按下电源按钮...
        power->setVal(1-power->getVal());           // ...切换功率特性的值
      } else
      
      if(pressType==SpanButton::DOUBLE){            // 如果双击电源按钮...
        power->setVal(1);                           // ...打开电源
        level->setVal(favoriteLevel);               // ...并将亮度设置为喜欢的级别
      } else
      
      if(pressType==SpanButton::LONG) {             // 如果长按电源按钮...
        favoriteLevel=level->getVal();              // ...保存当前亮度级别
        LOG1("Saved new brightness level=");        // ...并输出日志消息
        LOG1(favoriteLevel);
        LOG1("\n");        
        ledPin->set((1-power->getVal())*level->getVal());       // 闪烁 LED 表示新级别已保存
        delay(100);
        ledPin->set((1-power->getVal())*level->getVal());
      }
      
    } else

    if(pin==raisePin){                                                   
      if(pressType==SpanButton::DOUBLE){            // 如果双击升起按钮...
        power->setVal(1);                           // ...打开电源
        level->setVal(100);                         // ...并将亮度设置为最大级别
      } else {
      
      newLevel=level->getVal()+(pressType==SpanButton::LONG?10:1);   // 获取当前亮度级别并增加 10%（长按）或 1%（单击）
      if(newLevel>100)                                               // 不允许新级别超过最大值 100%
        newLevel=100;
      level->setVal(newLevel);                                       // 将亮度特征值设置为这个新级别
      }
      
    } else

    if(pin==lowerPin){                                                   
      if(pressType==SpanButton::DOUBLE){            // 如果双击下方按钮...
        power->setVal(1);                           // ...打开电源
        level->setVal(5);                           // ...并将亮度设置为最低级别
      } else {
      
      newLevel=level->getVal()-(pressType==SpanButton::LONG?10:1);   // 获取当前亮度级别并降低 10%（长按）或 1%（单击）
      if(newLevel<5)                                                 // 不允许新水平低于最低 5%
        newLevel=5;
      level->setVal(newLevel);                                       // 将亮度特征值设置为这个新级别
      }

    }

    // 不要忘记为实际 LED 设置新的功率和级别 - 上述代码本身只会更改 HomeKit 中 Characteristics 的值！我们仍然需要对实际 LED 本身采取行动。

    // 请注意，下面的代码行与上面 update() 方法中使用的 ledPin->set 函数类似，但不完全相同。在 update() 方法中，我们使用了 getNewVal()，
    // 因为我们想要更改 LED 以匹配用户通过 HomeKit 控制器请求的新值。我们不需要（也不应该）使用 setVal() 在 update() 方法中修改这些值，
    // 因为 HomeSpan 会自动为我们执行此操作，只要我们在 update() 方法末尾返回 StatusCode::OK。

    // 但在 button() 方法中，getNewVal() 没有任何意义，因为 HomeKit 不会在响应来自 HomeKit 控制器接口的用户请求时调用 button() 方法。
    // 相反，我们正在使用 setVal() 手动更改一个或多个特征的值以响应 SINGLE、DOUBLE 和 LONG SpanButton 请求。这些更改是即时的，
    // 因此我们可以通过随后调用 getVal() 来检索新值，如下所示。

    // 与往常一样，HomeSpan 将向所有已注册的 HomeKit 控制器发送事件通知，让它们知道我们使用 setVal() 所做的任何更改。
    
    ledPin->set(power->getVal()*level->getVal());       // 更新物理 LED 以反映新值

  }

};
      
//////////////////////////////////
