////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////

struct DEV_GarageDoor : Service::GarageDoorOpener {     // 车库门开启器

  Characteristic::CurrentDoorState *current;            // 参考当前门状态特征（特定于车库门开启器）
  Characteristic::TargetDoorState *target;             // 参考目标门状态特征（特定于车库门开启器）
  SpanCharacteristic *obstruction;        // 参考障碍物检测特性（特定于车库门开启器）

  DEV_GarageDoor() : Service::GarageDoorOpener(){       // 构造函数（）方法
        
    current=new Characteristic::CurrentDoorState(1);              // 初始值 1 表示关闭
    target=new Characteristic::TargetDoorState(1);                // 初始值 1 表示关闭
    obstruction=new Characteristic::ObstructionDetected(false);   // 初始值为 false 表示未检测到任何障碍物
    
    Serial.print("Configuring Garage Door Opener");   // 初始化消息
    Serial.print("\n");

  } // 结束构造函数

  boolean update(){                              // update() 方法

    // 有关每个值所代表的详细信息，请参阅 HAP 文档

    if(target->getNewVal()==0){                     // 如果目标状态值设置为 0，则 HomeKit 请求门处于打开位置
      LOG1("Opening Garage Door\n");
      current->setVal(2);                           // 将当前状态值设置为 2，表示“打开”
      obstruction->setVal(false);                   // 清除所有先前的障碍物检测
    } else {
      LOG1("Closing Garage Door\n");                // 否则，目标状态值设置为 1，并且 HomeKit 请求门处于关闭位置
      current->setVal(3);                           // 将当前状态值设置为 3，表示“关闭”
      obstruction->setVal(false);                   // 清除所有先前的障碍物检测
    }
    
    return(true);                               // 返回 true
  
  } //  更新

  void loop(){                                     // loop() 方法

    if(current->getVal()==target->getVal())        // 如果当前状态与目标状态匹配则不执行任何操作——退出循环（）
      return;

    if(current->getVal()==3 && random(100000)==0){    // 这里我们模拟一个随机障碍物，但前提是门正在关闭（而不是打开）
      current->setVal(4);                             // 如果我们的模拟阻塞被触发，则将当前状态设置为 4，这意味着“停止”
      obstruction->setVal(true);                      // 并将“检测到阻塞”设置为 true
      LOG1("Garage Door Obstruction Detected!\n");
    }

    if(current->getVal()==4)                       // 如果当前状态已停止，则无需执行任何操作 - 退出循环（）
      return;

    // 仅当门处于主动打开或主动关闭的状态时，才会调用这最后一部分代码。
    // 如果有障碍物，门将“停止”，并且不会再次启动，直到 HomeKit 控制器请求新的打开或关闭操作

    if(target->timeVal()>5000)                     // 通过监控自目标状态上次修改以来的时间来模拟需要 5 秒才能运行的车库门
      current->setVal(target->getVal());           // 将当前状态设置为目标状态
       
  } // loop
  
};

////////////////////////////////////

struct DEV_WindowShade : Service::WindowCovering {     // 具有保持功能的电动窗帘

  SpanCharacteristic *current;                     // 引用“通用”当前位置特征（由各种不同的服务使用）
  SpanCharacteristic *target;                      // 引用“通用”目标位置特征（由各种不同的服务使用）

  DEV_WindowShade() : Service::WindowCovering(){       // 构造函数（）方法
        
    current=new Characteristic::CurrentPosition(0);     // 窗帘的位置范围从 0（完全降低）到 100（完全升起）
    
    target=new Characteristic::TargetPosition(0);       // 窗帘的位置范围从 0（完全降低）到 100（完全升起）
    target->setRange(0,100,10);                         // 将允许的目标位置范围设置为 0-100，步长为 10
        
    Serial.print("Configuring Motorized Window Shade");   // 初始化消息
    Serial.print("\n");

  } // 结束构造函数

  boolean update(){                              // update() 方法

    // 以下逻辑基于 HomeKit 在实践中的操作方式，与 HAP 文档不一致。在该文档中，HomeKit 似乎支持完全打开或完全关闭窗帘，
    // 并带有一个可选控件，用于在窗帘移动时将其保持在给定的中间位置。

    // 实际上，即使您实例化该特性，HomeKit 似乎也没有实现任何形式的“保持”控制按钮。
    // 相反，HomeKit 提供了一个完整的滑块控件，类似于灯泡的亮度控件，可让您将窗帘的精确位置设置为 0-100%。
    // 这样就不需要任何类型的“保持”按钮了。由此产生的逻辑也非常简单：

    if(target->getNewVal()>current->getVal()){      // 如果请求的目标位置大于当前位置，则只需记录“提升”消息
      LOG1("Raising Shade\n");                      // ** 没有其他事情要做 - HomeKit 会跟踪当前位置，因此知道需要提升
    } else 
    if(target->getNewVal()<current->getVal()){      // 如果请求的目标位置小于当前位置，则只需记录“提升”消息
      LOG1("Lowering Shade\n");                     // ** 没有其他事情要做 - HomeKit 会跟踪当前位置，因此知道需要降低
    }
        
    return(true);                               // 返回 true
  
  } //  更新

  void loop(){                                     // loop() 方法

    // 这里我们模拟了窗帘需要 5 秒才能移动到新的目标位置
    
    if(current->getVal()!=target->getVal() && target->timeVal()>5000){          // 如果自目标位置上次修改以来已经过去了 5 秒……
      current->setVal(target->getVal());                                        // ...将当前位置设置为等于目标位置
    }

    // 请注意，没有必要将当前位置的连续更新发送到 HomeKit。HomeKit 不会显示当前位置。相反，
    // 它只是将当前位置的值与用户在 “家庭”应用中设置的目标位置值进行比较。如果它发现当前位置和目标位置相同，
    // 它就知道窗帘已停止。否则，它将根据指定的目标状态是大于还是小于当前状态来报告窗帘正在升起或降低。

    // 根据 HAP，特征位置状态也是必需的。然而，这似乎是重复的，而且考虑到 HomeKit 使用当前位置的方式，根本不需要。
    
  } // 循环
  
};
