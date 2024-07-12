////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////

struct DEV_LED : Service::LightBulb {               // 开/关 LED

  int ledPin;                                       // 为此 LED 定义的引脚编号
  SpanCharacteristic *power;                        // 引用 On 特性
  
  DEV_LED(int ledPin) : Service::LightBulb(){       // 构造函数（）方法

    power=new Characteristic::On();                 
    this->ledPin=ledPin;                            
    pinMode(ledPin,OUTPUT);                         
    
  } // 结束构造函数

  boolean update(){                              // update() 方法

    digitalWrite(ledPin,power->getNewVal());      
   
    return(true);                               // 返回 true
  
  } //  更新
};
      
//////////////////////////////////

// 这是定义 DEV_DimmableLED 的新代码 - 上面的更改已在注释中注明

struct DEV_DimmableLED : Service::LightBulb {       // 可调光 LED

  LedPin *ledPin;                                   // 新功能！创建对下面实例化的 LED 引脚的引用
  SpanCharacteristic *power;                        // 引用 On 特性
  SpanCharacteristic *level;                        // 新功能！创建对下面实例化的亮度特征的引用
  
  DEV_DimmableLED(int pin) : Service::LightBulb(){       // 构造函数（）方法

    power=new Characteristic::On();     
                
    level=new Characteristic::Brightness(50);       // 新功能！实例化亮度特征，初始值为 50%（与示例 4 中相同）
    level->setRange(5,100,1);                       // 新功能！亮度范围设置为最小 5% 到最大 100%，步长为 1%（与示例 4 的值不同）

    this->ledPin=new LedPin(pin);                   // 新功能！配置 PWM LED 以输出到指定引脚。注意无需提前调用 pinMode()
    
  } // 结束构造函数

  boolean update(){                              // update() 方法

    // 在这里，我们通过调用 ledPin->set(brightness) 来设置 LED 的亮度，其中亮度 = 0-100。注意 HomeKit 会单独设置灯泡的开/关状态，
    // 而不是亮度，这意味着 HomeKit 可以请求关闭灯泡，但仍保留亮度级别，这样灯泡重新打开后就无需重新发送请求。

    // 将开启特性的新值（“power”，布尔值）与亮度特性的新值（“level”，整数）相乘是一种简便方法，可以创建逻辑，
    // 在灯泡关闭时将 LED 级别设置为零，在灯泡打开时将其设置为当前亮度级别。
    
    ledPin->set(power->getNewVal()*level->getNewVal());    
   
    return(true);                               // 返回 true
  
  } //  更新
};
      
//////////////////////////////////
