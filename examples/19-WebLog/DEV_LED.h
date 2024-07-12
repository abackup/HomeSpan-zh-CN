////////////////////////////////////
//       设备专用 LED 服务        //
////////////////////////////////////

struct DEV_LED : Service::LightBulb {

  int ledPin;
  SpanCharacteristic *power;
  
  DEV_LED(int ledPin) : Service::LightBulb(){

    power=new Characteristic::On();
    this->ledPin=ledPin;
    pinMode(ledPin,OUTPUT);    
    WEBLOG("Configuring LED on Pin %d",ledPin);                        // 新功能！这将创建一个 Web 日志消息，宣布设备的配置
    
  } // 结束构造函数

  boolean update(){            

    digitalWrite(ledPin,power->getNewVal());    
    WEBLOG("LED on Pin %d: %s",ledPin,power->getNewVal()?"ON":"OFF");  // 新功能！每当 LED 亮起或熄灭时，都会创建一条 Web 日志消息
    return(true);
  
  } //  更新
};
      
//////////////////////////////////

// 有关 WEB 日志的更多信息
// ---------------------------
//
// * WEBLOG() 宏通过调用 Serial.printf() 进行操作，因此第一个参数始终需要是包含类似 printf 格式指令的文本字符串。
// 其余参数（如果有）是要打印的变量。例如，您不能简单地写入 WEBLOG(ledPin)。这将导致编译时出现错误，但您可以写入 LOG1(ledPin) 或 LOG2(ledPin) 以将日志消息仅输出到串口监视器。
//
// * 您不需要在格式字符串末尾包含“\n”，因为所有 Web 日志消息在显示时都会格式化为 HTML 表，而 HTML 会忽略“\n”。
//
// * 每条 Web 日志消息都记录有两个时间戳。第一个时间戳与设备首次启动的时间有关，并以 DAYS:HH:MM:SS 的形式显示。此时间戳始终存在。第二个时间戳是绝对时钟时间，
//采用标准 Unix 格式，例如“2020 年 8 月 10 日星期一 13:52:48”。此时间戳仅在设置了设备的时钟时间时才会出现，否则将显示为“未知”。请注意，在上面的示例中，
//第一条 Web 日志消息（“正在配置...”）将具有“未知”的时钟时间戳，即使我们启用了带有时间服务器的 Web 日志。这是因为在建立 WiFi 之前无法配置时间服务器，
//并且上面的第一条 Web 日志消息是在设备初始配置期间创建的，在建立 WiFi 连接之前。这样做完全没问题。
//
// * 每条 Web 日志消息还包括发出请求的客户端的 IP 地址，除非 Web 日志消息是独立于任何客户端请求生成的，
// 例如上面的第一条消息。在这些情况下，IP 地址将显示为 0.0.0.0。
//
// * 每当 HomeSpan 日志级别设置为 1 或更高时，Web 日志消息就会打印到串口监视器。因此，没有必要同时使用 WEBLOG() 和 LOG1() 重复相同的消息。
