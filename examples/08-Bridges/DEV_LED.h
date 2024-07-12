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

struct DEV_DimmableLED : Service::LightBulb {       // 可调光 LED

  LedPin *ledPin;                                   // 引用 Led Pin
  SpanCharacteristic *power;                        // 引用 On 特性
  SpanCharacteristic *level;                        // 参考亮度特征
  
  DEV_DimmableLED(int pin) : Service::LightBulb(){       // 构造函数（）方法

    power=new Characteristic::On();     
                
    level=new Characteristic::Brightness(50);       // 亮度特性初始值为 50%
    level->setRange(5,100,1);                       // 将亮度范围设置为最小 5% 到最大 100%，步长为 1%

    this->ledPin=new LedPin(pin);                   // 配置 PWM LED 以输出至指定引脚
    
  } // 结束构造函数

  boolean update(){                              // update() 方法
    
    ledPin->set(power->getNewVal()*level->getNewVal());    
   
    return(true);                               // 返回 true
  
  } //  更新
};
      
//////////////////////////////////
