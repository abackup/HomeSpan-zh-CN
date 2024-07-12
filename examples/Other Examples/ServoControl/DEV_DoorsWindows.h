////////////////////////////////////
//        设备专用 LED 服务       //
////////////////////////////////////

struct DEV_WindowShade : Service::WindowCovering {     // 具有保持功能的电动窗帘

  SpanCharacteristic *current;                     // 引用“通用”当前位置特征（由各种不同的服务使用）
  SpanCharacteristic *target;                      // 引用“通用”目标位置特征（由各种不同的服务使用）
  SpanCharacteristic *hTiltCurrent;                // 参考窗帘水平倾斜度 - 当前位置
  SpanCharacteristic *hTiltTarget;                 // 参照窗帘水平倾斜度-目标位置

  ServoPin *hTiltServo;                            // 参考伺服引脚控制水平倾斜

  DEV_WindowShade(uint8_t hTiltServoPin) : Service::WindowCovering(){       // 构造函数（）方法
        
    current=new Characteristic::CurrentPosition(0);     // 窗帘的位置范围从 0（完全降低）到 100（完全升起）
    target=new Characteristic::TargetPosition(0);       // 窗帘的位置范围从 0（完全降低）到 100（完全升起）
    target->setRange(0,100,10);                         // 将允许的目标位置范围设置为 0-100，步长为 10

    hTiltCurrent=new Characteristic::CurrentHorizontalTiltAngle();      // 倾斜角度以度为单位；HAP 默认值为 -90 至 +90
    hTiltTarget=new Characteristic::TargetHorizontalTiltAngle();

    // 在这里我们使用 HomeSpan 的 ServoPin 类定义我们的 Servo。
    // 请参阅 HomeSpan API 参考来了解完整详细信息和所有参数的列表。
    
    hTiltServo=new ServoPin(hTiltServoPin);
       
    Serial.print("Configuring Motorized Window Shade");   // 初始化消息
    Serial.print("\n");

  } // end constructor

  boolean update(){                              // update() 方法

    if(target->updated()){                            // 检查阴影目标位置是否已更新
      if(target->getNewVal()>current->getVal()){      // 如果请求的目标位置大于当前位置，则只需记录“raise”消息
        LOG1("Raising Shade\n");                      // ** 没有其他事情要做 - HomeKit 会跟踪当前位置，因此知道需要提升
      } else 
      if(target->getNewVal()<current->getVal()){      // 如果请求的目标位置小于当前位置，则只需记录“raise”消息
        LOG1("Lowering Shade\n");                     // ** 没什么可做的 - HomeKit 会跟踪当前位置，因此知道需要降低
      }
    }

    if(hTiltTarget->updated()){                         // 检查遮阳倾斜角度是否已更新
      hTiltCurrent->setVal(hTiltTarget->getNewVal());   // 设置倾斜的当前值以匹配目标值
      hTiltServo->set(hTiltTarget->getNewVal());        // <--- 使用 ServoPin->set(degrees) 方法更新实际伺服位置
    }
        
    return(true);                               // 返回 true
  
  } // update

  void loop(){                                     // loop() 方法

    // 这里我们模拟了窗帘需要 5 秒才能移动到新的目标位置
    
    if(current->getVal()!=target->getVal() && target->timeVal()>5000){          // 如果自上次修改目标位置以来已经过去了5秒......
      current->setVal(target->getVal());                                        // ...将当前位置设置为等于目标位置
    }
    
  } // loop
  
};
