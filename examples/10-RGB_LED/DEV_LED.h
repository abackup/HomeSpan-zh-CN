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
    
    Serial.print("Configuring On/Off LED: Pin=");   // 初始化消息
    Serial.print(ledPin);
    Serial.print("\n");

  } // 结束构造函数

  boolean update(){                              // update() 方法

    LOG1("Updating On/Off LED on pin=");
    LOG1(ledPin);
    LOG1(":  Current Power=");
    LOG1(power->getVal()?"true":"false");
    LOG1("  New Power=");
    LOG1(power->getNewVal()?"true":"false");
    LOG1("\n");

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
};
      
//////////////////////////////////

struct DEV_RgbLED : Service::LightBulb {       // RGB LED（命令阴极）

  LedPin *redPin, *greenPin, *bluePin;
  
  SpanCharacteristic *power;                   // 引用 On 特性
  SpanCharacteristic *H;                       // 参考色调特征
  SpanCharacteristic *S;                       // 参考饱和特性
  SpanCharacteristic *V;                       // 参考亮度特征
  
  DEV_RgbLED(int red_pin, int green_pin, int blue_pin) : Service::LightBulb(){       // 构造函数（）方法

    power=new Characteristic::On();                    
    H=new Characteristic::Hue(0);              // 实例化色调特征，初始值为 360 中的 0
    S=new Characteristic::Saturation(0);       // 实例化饱和度特性，初始值为 0%
    V=new Characteristic::Brightness(100);     // 实例化亮度特征，初始值为 100%
    V->setRange(5,100,1);                      // 将亮度范围设置为最小 5% 到最大 100%，步长为 1%
    
    this->redPin=new LedPin(red_pin);        // 配置 PWM LED 以输出至红色引脚
    this->greenPin=new LedPin(green_pin);    // 配置 PWM LED 以输出至绿色引脚
    this->bluePin=new LedPin(blue_pin);      // 配置 PWM LED 以输出至蓝色引脚
 
    char cBuf[128];
    sprintf(cBuf,"Configuring RGB LED: Pins=(%d,%d,%d)\n",redPin->getPin(),greenPin->getPin(),bluePin->getPin());
    Serial.print(cBuf);
    
  } // 结束构造函数

  boolean update(){                         // update() 方法

    boolean p;
    float v, h, s, r, g, b;

    h=H->getVal<float>();                      // 获取并存储所有当前值。请注意使用 <float> 模板来正确读取值
    s=S->getVal<float>();
    v=V->getVal<float>();                      // 虽然 HAP 中的 H 和 S 定义为 FLOAT，但 V（亮度）定义为 INT，但会进行适当的重新转换
    p=power->getVal();

    char cBuf[128];
    sprintf(cBuf,"Updating RGB LED: Pins=(%d,%d,%d): ",redPin->getPin(),greenPin->getPin(),bluePin->getPin());
    LOG1(cBuf);

    if(power->updated()){
      p=power->getNewVal();
      sprintf(cBuf,"Power=%s->%s, ",power->getVal()?"true":"false",p?"true":"false");
    } else {
      sprintf(cBuf,"Power=%s, ",p?"true":"false");
    }
    LOG1(cBuf);
      
    if(H->updated()){
      h=H->getNewVal<float>();
      sprintf(cBuf,"H=%.0f->%.0f, ",H->getVal<float>(),h);
    } else {
      sprintf(cBuf,"H=%.0f, ",h);
    }
    LOG1(cBuf);

    if(S->updated()){
      s=S->getNewVal<float>();
      sprintf(cBuf,"S=%.0f->%.0f, ",S->getVal<float>(),s);
    } else {
      sprintf(cBuf,"S=%.0f, ",s);
    }
    LOG1(cBuf);

    if(V->updated()){
      v=V->getNewVal<float>();
      sprintf(cBuf,"V=%.0f->%.0f  ",V->getVal<float>(),v);
    } else {
      sprintf(cBuf,"V=%.0f  ",v);
    }
    LOG1(cBuf);

    // 这里我们调用 LedPin 的一个静态函数，将 HSV 转换为 RGB。
    // 参数必须全部是 H[0,360]、S[0,1] 和 V[0,1] R、G、B 范围内的浮点数，也返回 [0,1] 范围

    LedPin::HSVtoRGB(h,s/100.0,v/100.0,&r,&g,&b);   // 由于 HomeKit 以百分比提供 S 和 V，因此缩小 100

    int R, G, B;

    R=p*r*100;                                      // 由于 LedPin 使用百分比，因此按比例增加 100，并按电源状态乘以 0 或 1
    G=p*g*100;
    B=p*b*100;

    sprintf(cBuf,"RGB=(%d,%d,%d)\n",R,G,B);
    LOG1(cBuf);

    redPin->set(R);                      // 使用新值更新每个 ledPin
    greenPin->set(G);    
    bluePin->set(B);    
      
    return(true);                               // 返回 true
  
  } //  更新
};
      
//////////////////////////////////
