////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////


struct DEV_GarageDoor : Service::GarageDoorOpener {    // 车库门开启器

  Characteristic::CurrentDoorState *current;           // 参考当前门状态特征（特定于车库门开启器）
  Characteristic::TargetDoorState *target;             // 参考目标门状态特征（特定于车库门开启器）
  SpanCharacteristic *obstruction;                     // 参考障碍物检测特性（特定于车库门开启器）

  DEV_GarageDoor() : Service::GarageDoorOpener(){      // constructor() 方法

    // 下面我们使用枚举常量而不是整数来设置特征值。
   // 使用枚举常量意味着不必记住每个州的整数代码。您可以在 HomeSpan 的服务和特征页面上找到所有可用枚举常量的完整列表。
   // 请注意，枚举常量的使用是可选的 - 您始终可以使用代表每个州的整数代码。
        
    current=new Characteristic::CurrentDoorState(Characteristic::CurrentDoorState::CLOSED);      // 这里我们使用常量的完全限定名称 "CLOSED"
    target=new Characteristic::TargetDoorState(target->CLOSED);                                  // 这里我们使用对象的名称，而不是完全限定的名称（更少的输入）

    // 下面我们必须使用枚举常量的完全限定名称，不能使用“obstruction->NOT_DETECTED”。
    // 为什么？因为上面我们将“obstruction”声明为指向通用 SpanCharacteristic 的指针，而不是指向更具体的 Characteristic::ObstructionDetected 的指针。
    // 两者都可以，这只是编程偏好的问题（正如您所见，我们在此草图中使用了这两种约定）。但使用 SpanCharacteristic 声明包含枚举常量的 Characteristic 的缺点是对象本身不知道这些常量。
    // 这是因为所有枚举常量都是在各自特定的 Characteristic 类中唯一定义的，而不是在所有特定 Characteristics 派生自的通用 SpanCharacteristic 类中唯一定义的。
    
    obstruction=new Characteristic::ObstructionDetected(Characteristic::ObstructionDetected::NOT_DETECTED);   // 这有效
//  obstruction=new Characteristic::ObstructionDetected(obstruction->NOT_DETECTED);                           // 这会产生编译器错误（试一下看看）
    
    Serial.print("Configuring Garage Door Opener");   // 初始化消息
    Serial.print("\n");

  } // end constructor

  boolean update(){                              // update() 方法

    // 有关每个值所代表的详细信息，请参阅 HAP 文档

    if(target->getNewVal()==target->OPEN){          // HomeKit 要求门处于打开位置
      LOG1("Opening Garage Door\n");
      current->setVal(current->OPENING);            // 将当前状态值设置为 OPENING
      obstruction->setVal(false);                   // 清除任何先前的阻塞检测 - 注意我们在这里不使用枚举常量
    } else {
      LOG1("Closing Garage Door\n");                // 否则 HomeKit 必须要求门处于关闭位置
      current->setVal(current->CLOSING);            // 将当前状态值设置为 CLOSING
      obstruction->setVal(false);                   // 清除所有先前的阻塞检测
    }
    
    return(true);                               // 返回 true
  
  } // update

  void loop(){                                     // loop() 方法

    if(current->getVal()==target->getVal())        // 如果当前状态与目标状态匹配则不执行任何操作——退出循环（）
      return;

    if(current->getVal()==current->CLOSING && random(100000)==0){    // 这里我们模拟一个随机障碍物，但前提是门正在关闭（而不是打开）
      current->setVal(current->STOPPED);                             // 如果我们的模拟阻塞被触发，则将当前状态设置为 STOPPED
      obstruction->setVal(true);                                     // 并将 disorders-detected 设置为 true
      LOG1("Garage Door Obstruction Detected!\n");
    }

    if(current->getVal()==current->STOPPED)                          // 如果当前状态已停止，则无需执行任何操作 - 退出循环（）
      return;

    // 仅当门处于主动打开或主动关闭的状态时，才会调用这最后一部分代码。
    // 如果有障碍物，门将“停止”，并且不会再次启动，直到 HomeKit 控制器请求新的打开或关闭操作

    if(target->timeVal()>5000)                     // 通过监控自目标状态上次修改以来的时间来模拟需要 5 秒才能操作的车库门
      current->setVal(target->getVal());           // 将当前状态设置为目标状态
       
  } // loop
  
};

////////////////////////////////////

struct DEV_WindowShade : Service::WindowCovering {     // 具有保持功能的电动窗帘

  SpanCharacteristic *current;                     // 引用“通用”当前位置特征（由各种不同的服务使用）
  SpanCharacteristic *target;                      // 引用“通用”目标位置特征（由各种不同的服务使用）

  DEV_WindowShade() : Service::WindowCovering(){       // constructor() 方法
        
    current=new Characteristic::CurrentPosition(0);     // 窗帘的位置范围从 0（完全降低）到 100（完全升起）
    
    target=new Characteristic::TargetPosition(0);       // 窗帘的位置范围从 0（完全降低）到 100（完全升起）
    target->setRange(0,100,10);                         // 将允许的目标位置范围设置为 0-100，步长为 10
        
    Serial.print("Configuring Motorized Window Shade");   // 初始化消息
    Serial.print("\n");

  } // end constructor

  boolean update(){                              // update() 方法

    if(target->getNewVal()>current->getVal()){      // 如果请求的目标位置大于当前位置，则只需记录“raise”消息
      LOG1("Raising Shade\n");                      // ** 没有其他事情要做 - HomeKit 会跟踪当前位置，因此知道需要提升
    } else 
    if(target->getNewVal()<current->getVal()){      // 如果请求的目标位置小于当前位置，则只需记录“raise”消息
      LOG1("Lowering Shade\n");                     // ** 没什么可做的 - HomeKit 会跟踪当前位置，因此知道需要降低
    }
        
    return(true);                               // 返回 true
  
  } // update

  void loop(){                                     // loop() 方法

    // 这里我们模拟了窗帘需要 5 秒才能移动到新的目标位置
    
    if(current->getVal()!=target->getVal() && target->timeVal()>5000){          // 如果自上次修改目标位置以来已经过去了5秒......
      current->setVal(target->getVal());                                        // ...将当前位置设置为等于目标位置
    }

    // 请注意，没有必要向 HomeKit 发送当前位置的持续更新。HomeKit 不会显示当前位置。相反，它只是将当前位置的值与用户在“家庭”应该中
    // 设置的目标位置值进行比较。如果它发现当前位置和目标位置相同，则它知道窗帘已停止。否则，它将根据指定的目标状态是大于还是小于当前状态
    // 来报告窗帘正在升起或降低。
   
  } // loop
  
};
