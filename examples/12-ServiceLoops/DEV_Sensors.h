////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////

struct DEV_TempSensor : Service::TemperatureSensor {     // 独立的温度传感器

  SpanCharacteristic *temp;                        // 参考当前温度特性
  
  DEV_TempSensor() : Service::TemperatureSensor(){       // 构造函数（）方法
    
    // 首先，我们实例化温度传感器的主要特性，即当前温度，并将其初始值设置为 20 度。对于真正的传感器，我们将读取读数并将其初始化为该值。
   // 注意：HomeKit 对所有温度设置都使用摄氏度。HomeKit 将根据您 iPhone 上的设置在 HomeKit 应用中显示温度。虽然 HAP 文档中包含一个特性，
  // 该特性似乎允许设备通过为每个服务指定摄氏度或华氏度显示来覆盖此设置，但它似乎并没有像宣传的那样工作。
    
    temp=new Characteristic::CurrentTemperature(-10.0);        // 实例化当前温度特征
    temp->setRange(-50,100);                                  // 将范围从 HAP 默认值 0-100 扩大到 -50 到 100，以允许负温度
        
    Serial.print("Configuring Temperature Sensor");           // 初始化消息
    Serial.print("\n");

  } // 结束构造函数

  // 接下来我们创建 loop() 方法。此方法不带参数，也不返回任何值。为了模拟实际传感器的温度变化，我们将使用 getVal() 函数读取温度特征的当前值，
  // 以 <float> 作为模板参数；添加 0.5 摄氏度；然后将结果存储在名为“temperature”的浮点变量中。这将模拟 0.5 摄氏度（略小于 1 华氏度）的增量。
  // 我们将温度上限设为 35.0 摄氏度，之后重置为 10.0 并重新开始。最重要的是，我们将每 5 秒执行一次此操作，使用 timeVal() 检查自上次修改以来经过的时间。

  // 所有操作都发生在 setVal() 行中，我们将 temp 特性的值设置为新的温度值。
  // 这会告诉 HomeKit 向所有可用的控制器发送事件通知消息，让它们知道新的温度。
  // 请注意，setVal() 不是模板函数，不需要您指定 <float> 作为模板参数。这是因为 setVal() 可以根据您指定的参数确定类型。
  // 如果存在任何歧义，您可以随时具体转换参数，例如：setVal((float)temperature)。

  void loop(){

    if(temp->timeVal()>5000){                               // 检查自上次更新以来经过的时间，如果超过 5 秒则继续
      float temperature=temp->getVal<float>()+0.5;          //“模拟”半度的温度变化……
      if(temperature>35.0)                                  // ...但最高温度限制在 35C，然后从 -30C 开始
        temperature=-30.0;
      
      temp->setVal(temperature);                            // 设置新温度；这会生成事件通知并重置经过的时间
      
      LOG1("Temperature Update: ");
      LOG1(temperature*9/5+32);
      LOG1("\n");            
    }
    
  } // loop
  
};
      
//////////////////////////////////

struct DEV_AirQualitySensor : Service::AirQualitySensor {     // 独立的空气质量传感器

  // 空气质量传感器与温度传感器类似，不同之处在于它支持多种测量。
  // 我们将使用其中三个。第一个是必需的，后两个是可选的。

  SpanCharacteristic *airQuality;                 // 引用空气质量特征，它是一个从 0 到 5 的整数
  SpanCharacteristic *o3Density;                  // 引用臭氧密度特征，它是一个从 0 到 1000 的浮点数
  SpanCharacteristic *no2Density;                 // 引用二氧化氮特征，它是一个从 0 到 1000 的浮点数
  
  DEV_AirQualitySensor() : Service::AirQualitySensor(){       // 构造函数（）方法
    
    airQuality=new Characteristic::AirQuality(1);                         // 实例化空气质量特征并将初始值设置为 1
    o3Density=new Characteristic::OzoneDensity(300.0);                    // 实例化臭氧密度特征并将初始值设置为 300.0
    no2Density=new Characteristic::NitrogenDioxideDensity(700.0);         // 实例化二氧化氮密度特征并将初始值设置为 700.0
    
    Serial.print("Configuring Air Quality Sensor");   // 初始化消息
    Serial.print("\n");

  } // 结束构造函数

  void loop(){

    // 请注意，我们不会更新二氧化氮密度特性。因此，该值应保持稳定，即初始值 700.0

    if(airQuality->timeVal()>5000)                            // 每 5 秒修改一次空气质量特征
      airQuality->setVal((airQuality->getVal()+1)%6);         // 通过将当前值增加 1 来模拟空气质量的变化，并保持在 0-5 的范围内

    if(o3Density->timeVal()>10000)                            //每10秒修改一次臭氧密度特征值
      o3Density->setVal((double)random(200,500));             // 用 200 到 499 之间的随机值模拟变化。注意使用 (double) 强制类型转换，因为 random() 返回一个整数
       
  } // loop

};
      
//////////////////////////////////


// 您应该在 HomeKit 应用程序中看到的内容
// -----------------------------------------------
// 如果您加载上述示例，您的 HomeKit 应用程序将显示两个新图块：一个标记为“温度传感器”，另一个标记为“空气质量”。
// 温度传感器图块应指示 10C 至 35C（50F 至 95F）范围内的温度，该图块每 5 秒自动递增并更新 0.5C。
// 空气质量图块应每 10 秒循环一次“质量”状态。状态在 HomeKit 中显示为“未知”、“优秀”、“良好”、“一般”、“较差”和“较差”。

// 请注意，HomeKit 仅显示图块本身内特性子集的值。对于空气质量传感器，仅显示空气质量的质量状态。要查看其他特性的值，
// 例如臭氧密度和二氧化氮密度，您需要单击图块，然后打开设置屏幕（如果 HomeKit 在控制屏幕上显示这些值而不是让您打开设置屏幕，那就更好了）。
// 在设置屏幕上，您应该看到我们实例化的所有三个特性的值：空气质量、二氧化氮密度和臭氧密度。空气质量和臭氧密度都应每 10 秒更改一次。
// 二氧化氮密度应保持稳定在初始值 700.0，因为我们从未使用 setVal() 来更新此特性。

// 如果您以 VERBOSITY 级别 2 运行 HomeSpan（如库的 Settings.h 文件中所指定），您可以看到 HomeSpan 在后台每 5 秒向所有注册控制器
// 发送事件通知消息（针对温度传感器），每 5 秒和 10 秒发送一次（针对空气质量传感器）。如果仔细观察，您会发现空气质量传感器的事件通
// 知消息仅包含两个值 - 一个用于空气质量状态，一个用于臭氧密度。HomeSpan 不会发送二氧化氮密度特性的值，因为它没有通过 setVal() 函数更改。

// 最后说明：HomeKit 在 HomeKit 应用中显示的温度小数位数与值本身的步长无关。这似乎是由 HomeKit 硬编码的：对于华氏度，温度传感器图块
// 不显示小数，并四舍五入到最接近的整数度（例如 72、73、74 度）；对于摄氏度，图块允许半度分辨率并相应地四舍五入
// （例如 22.7 显示为 22.5，22.8 显示为 23.0）。
