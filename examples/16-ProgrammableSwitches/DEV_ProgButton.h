////////////////////////////////////////////////////
//           设备专用可编程开关服务                //
////////////////////////////////////////////////////

struct DEV_ProgButton : Service::StatelessProgrammableSwitch {       // 无状态可编程交换机
 
  SpanCharacteristic *switchEvent;                  // 引用 ProgrammableSwitchEvent 特性
  
  DEV_ProgButton(int buttonPin, int index) : Service::StatelessProgrammableSwitch(){

    switchEvent=new Characteristic::ProgrammableSwitchEvent();  // 可编程开关事件特性（将设置为单按、双按或长按）
    new Characteristic::ServiceLabelIndex(index);               // 设置服务标签索引（仅当每个服务有多个无状态可编程交换机时才需要）
                
    new SpanButton(buttonPin);                                  // 创建新的 SpanButton

    Serial.print("Configuring Programmable Pushbutton: Pin=");  // 初始化消息
    Serial.print(buttonPin);
    Serial.print("  Index=");
    Serial.print(index);
    Serial.print("\n");
    
  } // 结束构造函数

  // 我们不需要实现 update() 方法或 loop() 方法 - 只需实现 button() 方法：

  void button(int pin, int pressType) override {

    LOG1("Found button press on pin: ");            // 记录消息总是一个好主意
    LOG1(pin);
    LOG1("  type: ");
    LOG1(pressType==SpanButton::LONG?"LONG":(pressType==SpanButton::SINGLE)?"SINGLE":"DOUBLE");
    LOG1("\n");

    // 所有操作都发生在下面的这一行中。我们只需将可编程开关事件特征的值设置为 pressType 提供的值。
    // pressType 的值（0=SpanButton::SINGLE、1=SpanButton::DOUBLE 和 2=SpanButton::LONG）旨在匹配可编程开关事件特征所需的值。

    switchEvent->setVal(pressType);                // 设置 switchEvent 特性的值

  }

};
      
//////////////////////////////////
