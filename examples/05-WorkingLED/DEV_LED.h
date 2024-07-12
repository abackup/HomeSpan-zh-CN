////////////////////////////////////
//        设备专用 LED 服务       //
////////////////////////////////////

// 这就是我们定义我们的新 LED 服务的地方！

struct DEV_LED : Service::LightBulb {               // 首先，我们从 HomeSpan LightBulb 服务创建一个派生类

  int ledPin;                                       // 该变量存储了为该 LED 定义的引脚号
  SpanCharacteristic *power;                        // 这里我们创建了一个指向 SpanCharacteristic 的通用指针，名为“power”，我们将在下面使用它

  // 接下来我们定义 DEV_LED 的构造函数。请注意，它需要一个参数 ledPin，该参数指定 LED 所连接的引脚。
  
  DEV_LED(int ledPin) : Service::LightBulb(){

    power=new Characteristic::On();                 // 这是我们创建之前在 setup() 中定义的 On Characterstic 的地方。将其保存在上面创建的指针中，以供下面使用
    this->ledPin=ledPin;                            // 不要忘记存储 ledPin...
    pinMode(ledPin,OUTPUT);                         // ...并将 ledPin 的模式设置为 OUTPUT（标准 Arduino 功能）
    
  } // 结束构造函数

  // 最后，我们用实际打开/关闭 LED 的指令覆盖默认的 update() 方法。注意 update() 返回布尔类型

  boolean update(){            

    digitalWrite(ledPin,power->getNewVal());        // 使用标准 Arduino 函数根据 power->getNewVal() 调用的返回结果打开/关闭 ledPin（更多信息见下文）
   
    return(true);                                   // 返回 true 表示更新成功（否则，如果由于某种原因无法打开 LED，则创建代码返回 false）
  
  } //  更新
};
      
//////////////////////////////////

// update() 的工作原理：
//------------------
// 每当 HomeKit 控制器请求 HomeSpan 更新特征时，HomeSpan 都会调用包含该特征的服务的 update() 方法。它只调用一次，即使该服务请求了多个特征更新。
// 例如，如果您指示 HomeKit 打开灯并将其亮度设置为 50%，它将向 HomeSpan 发送两个请求：一个请求将 LightBulb 服务的“开”特征从“false”更新为“true”，
// 另一个请求将同一服务的“亮度”特征更新为 50。这非常低效，需要用户处理对同一服务的多个更新。

// 相反，HomeSpan 将两个请求合并为对服务本身的 update() 的单个调用，您可以在其中处理同时更改的所有特征。在上面的例子中，我们只需要处理一个特征，
// 所以这没什么意义。但在后面的例子中，我们将看到它如何与多个特征一起工作。
// 
// 如何访问特征的新值和当前值

// ----------------------------------------------------------------------

// HomeSpan 将其特征值存储在联合结构中，该结构允许使用不同的类型，例如浮点数、布尔值等。HAP 为每个特征定义了具体类型。查找特征是 uint8 还是 uint16 可能很麻烦，
// 因此 HomeSpan 抽象了所有这些细节。由于 C++ 遵循严格的变量类型，因此这是通过使用模板方法来实现的。每个特征都支持以下两种方法：

// getVal<type>() - 返回 Characterisic 的当前值，转换为“type”后
// getNewVal<type>() - 返回 Characteritic 的新值（即要更新的值），转换为“type”后

// 例如，MyChar->getVal<int>() 将 SpanCharacterstic MyChar 的当前值作为 int 返回，而不管 HomeSpan 如何存储该值。同样，MyChar->getVal<double>() 
// 将值作为 double 返回，即使它存储为布尔值（在这种情况下，您将获得 0.00 或 1.00）。当然，您需要确保了解预期值的范围，以便您不会尝试使用 getVal<uint8_t>() 
// 访问存储为 2 字节 int 的值。但是使用 getVal<int>() 来访问 HAP 坚持以浮点数存储的特征的值是完全可以的，即使它的范围严格在 0 到 100 之间，步长为 1。
// 了解范围和步长大小是确定是否可以将其作为 <int> 甚至 <uint8_t> 访问所需的全部知识。

// 由于大多数特征值都可以正确地转换为 int，因此如果未指定模板参数，getVal 和 getNewVal 都默认为 <int>。如上所示，我们通过简单地调用 power->getNewVal() 
// 检索了 HomeKit 为名为“power”的 On特征请求的新值。由于未指定模板参数，getNewVal() 将返回一个 int。并且由于 On特征本身存储为布尔值，
// getNewVal() 将返回 0 或 1，具体取决于 HomeKit 是请求关闭还是打开 Characteristic。
// 您可能还注意到，在上面的例子中，我们需要使用 getNewVal()，但没有在任何地方使用 getVal()。这是因为我们确切地知道如果 HomeKit 请求打开或关闭 LED 时该怎么做。
// LED 的当前状态（打开或关闭）无关紧要。在后面的例子中，我们将看到设备当前状态确实很重要的情况，并且我们需要访问当前值和新值。
// 最后，还有一个用于 Characteristics 的附加方法，上面没有使用，但将在后面的例子中使用：updated()。此方法返回一个布尔值，指示 HomeKit 是否已请求更新特征值，
// 这意味着 getNewVal() 将包含它想要为该特征设置的新值。对于只有一个特征的服务，如上所述，我们不需要询问是否使用 power->updated() 更新了“power”，
// 因为调用服务的 update() 方法这一事实意味着 HomeKit 正在请求更新，而唯一要更新的是“power”。但是对于具有两个或更多个特征的服务，可以调用 update() 并请求仅更新特征的子集。
// 我们将在后面的多特征示例中发现 updated() 方法的良好用途。
// 
// 幕后：UPDATE() 的返回代码的作用
// ------------------------------------------------------
// HomeKit 要求每个特性在尝试更新其值时返回一个特殊的 HAP 状态代码。HomeSpan 会自动处理大多数错误，例如未找到特性，或请求更新只读特性。在这些情况下，update() 甚至不会被调用。
// 但如果调用了，HomeSpan 需要为该服务中要更新的每个特性返回一个 HAP 状态代码。通过返回“true”，您告诉 HomeSpan 请求的新值没问题，并且您已经对物理设备进行了必要的更新。
// 在收到 true 返回值后，HomeSpan 通过将“newValue”数据元素复制到当前“value”数据元素中来更新特性本身。然后，HomeSpan 向 HomeKit 发送一条消息，其中包含表示“OK”的 HAP 代码，
// 让控制器知道它请求的新值已成功处理。HomeKit 绝不会要求或允许 HomeSpan 发回指示特性中数据的数据值。请求更新时，HomeKit 仅期望 HAP 状态代码为 OK，或者表示错误的其他状态代码。
// 要告诉 HomeSpan 向控制器发送错误代码（表示您无法成功处理更新），只需让 update() 返回值“false”。
// HomeSpan 将返回的“false”转换为表示“UNABLE”的 HAP 状态代码，这将导致控制器显示设备未响应。
// 
// 由于 HomeSpan 或 HomeKit 本身会提前进行大量检查，因此您需要返回“false”的原因很少。例如，HomeKit 不允许您使用控制器甚至 Siri 将 LightBulb 的亮度更改为您指定的允许值范围之外的值。
// 这意味着您收到的任何 update() 请求都应仅包含在范围内的 newValue 数据元素。
