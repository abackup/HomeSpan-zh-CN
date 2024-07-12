/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2020-2022 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  特此授予获得此软件和相关文档文件（“软件”）副本的任何人免费许可，以无限制方式处理软件，
 *  包括但不限于使用、复制、修改、合并、发布、分发、再许可和/或销售软件副本的权利，并允许
 *  向其提供软件的人员这样做，但须遵守以下条件：
 *  
 *  上述版权声明和本许可声明均应包含在软件的所有副本或重要部分中。
 *  
 *  软件按“原样”提供，不作任何明示或暗示的保证，包括但不限于适销性、特定用途的适用性和不
 *  侵权性的保证。在任何情况下，作者或版权持有者均不对因软件或使用或其他处理软件而引起的
 *  或与之相关的任何索赔、损害或其他责任承担责任，无论是合同行为、侵权行为还是其他行为。
 *  
 ********************************************************************************/
 
////////////////////////////////////////////////////////////
//                                                        //
//              HomeSpan：ESP32 的 HomeKit 实现           //
//    ------------------------------------------------    //
//                                                        //
//            示例 15：真实按钮                            //
//                    * 手动控制可调光 LED                 //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h" 
#include "DEV_LED.h"     

void setup() {

  // 在示例 14 中，我们了解了如何通过自动重置特性来模拟 HomeKit 中的 PushButton 磁贴，使其在短时间后“关闭”。但是，
  // 有时我们希望能够通过实际的 PushButton（或瞬时开关）来物理控制设备，从而触发操作，例如打开灯或风扇，或打开车库门。
  // 此外，我们希望 HomeKit 能够反映此类手动操作导致的设备变化 - HomeKit 应该知道何时手动打开或关闭灯。

  // 实现的一种方法是通过将自定义代码添加到派生服务的 loop() 方法中，该方法监视按钮、检查按下时间、消除按钮噪音、按下时执行某些操作，
  // 并使用 setVal() 方法通知 HomeKit 这些操作。或者您可以简单地使用 HomeSpan 的内置 SpanButton() 对象。

  // SpanButton() 是一个服务级对象，这意味着它将自身附加到您定义的最后一个服务。通常，您会在派生服务的构造函数中直接实例化一个
  // 或多个 SpanButton() 对象。

  // SpanButton() 支持三种类型的触发器：单次按下按钮、双击按钮和长按按钮。

  // 触发这些不同类型按钮所需的按下时间长度可以通过 SpanButton() 的可选参数指定。由于大多数按钮在按下时（然后释放时再次）会产生杂散噪音，
  // 因此触发单次按下的默认时间为 5 毫秒。可以将其更改为更长的值，但不建议使用更短的值，因为除非您使用硬件对开关进行去抖动，否则这可能会允许杂散触发。

  // SpanButton() 构造函数有 5 个参数，顺序如下：
  //                 pin - 按钮所连接的引脚号（必需）
  //                 longTime - 按钮按下的最小时间长度（以毫秒为单位）才被视为长按（可选；默认值 = 2000 毫秒）
  //                 singleTime - 按钮按下的最小时间长度（以毫秒为单位）才被视为单按（可选；默认值 = 5 毫秒）
  //                 doubleTime - 按钮按下之间的最大时间长度（以毫秒为单位）才被视为双按（可选；默认值 = 200 毫秒）
  //                 triggerType - 导致引脚触发的操作（可选；默认值 = SpanButton::TRIGGER_ON_LOW）。内置选项包括：

  // SpanButton::TRIGGER_ON_LOW：用于将引脚连接到 GROUND 的按钮 SpanButton::TRIGGER_ON_HIGH：用于将引脚连接到 VCC（通常为 +3.3V）的按钮

  // SpanButton::TRIGGER_ON_TOUCH：当引脚连接到触摸板/传感器时使用

  // 首次实例化 SpanButton() 时，HomeSpan 会根据所选的触发器类型配置指定的引脚。

  // 然后，HomeSpan 持续轮询所有与 SpanButton() 对象关联的引脚并检查触发器，这些触发器表明按钮已按下但尚未释放。然后它启动一个计时器。
  // 如果按下按钮的时间少于 singleTime 毫秒后释放，则不会发生任何事情。如果按下按钮的时间超过 singleTime 毫秒但少于 longTime 毫秒后释放，
  // 则触发单次按下，除非您在 doubleTime 毫秒内再次按下以触发双击。如果按钮按住超过 longTime 毫秒而不释放，则触发长按。一旦触发长按，
  // 计时器就会重置，因此如果您继续按住按钮，将在另一个 longTime 毫秒内触发另一次长按。这种情况持续到您最终释放按钮。

  // 请注意，如果您设置 longTime > singleTime，SpanButton() 将仅触发长按。此外，如果您将 doubleTime 设置为零，SpanButton() 将无法触发双击。

  // 要在派生服务中使用 SpanButton()，您需要实现 button() 方法。与 loop() 方法类似，您的 button() 方法通常包含 getVal() 函数和
  // setVal() 函数的某种组合，以及在物理设备上执行某些操作的代码（将引脚设置为高电平或低电平、打开风扇等）。但是，
  // 与 HomeSpan 在每个轮询周期调用的 loop() 方法不同，HomeSpan 仅在连接到服务的按钮注册了单击、双击或长按时调用 button() 方法。

  // 此外，与循环方法相反，button() 方法采用两个“int”参数，应定义如下：

  // void button(int pin, int pressType)

  // 其中“pin”是触发的 PushButton 的引脚号，pressType 设置为 0（表示单次按下）、1（表示双次按下）和 2（表示长按）。您还可以使用预定义常量 SpanButton::SINGLE、SpanButton::DOUBLE 和 SpanButton::LONG 代替数字 0、1 和 2（建议这样做，但您将在示例 16 中看到为什么这些整数不能被 C++ 枚举类替换）。

  // 当然，您可以用自己的名称替换变量“pin”和“pressType”。唯一的要求是定义符合“void button(int, int)”签名。 HomeSpan 首次启动时，它会检查所有包含一个或多个 SpanButton() 实例的服务，以确保您已实现自己的 button(int, int) 方法。如果没有，HomeSpan 将在串口监视器上打印一条警告消息。如果您实例化 SpanButton() 但忘记创建 button() 方法，或者使用错误的参数创建它，则不会发生任何不好的事情。但也不会发生任何好事 - 按钮按下会被忽略。

  //  C++ 注释：为了进行额外检查，您还可以在方法定义后放置上下文关键字“override”，如下所示：

  // void button(int buttonPin, int pressType) override {...your code...}

  // 这样做可以让编译器检查您是否确实覆盖了基类 button() 方法，而不是无意中创建了具有错误签名的新 button() 方法，而该方法永远不会被 SpanButton() 调用。事实上，您也可以在 update() 和 loop() 方法的定义中添加“override”，因为这些方法始终应该覆盖基类方法。

  // 为了演示 SpanButtons 在实践中的工作原理，我们将实现一个可调光 LED，从示例 11 中使用的相同 LED 代码开始，但使用 3 个 SpanButton() 对象执行不同的功能来展示不同类型的按压。

  //       * 一个“电源”SpanButton，在单击时切换电源，在双击时打开电源并将亮度设置为“最喜欢的”级别，在长按时设置新的“最喜欢的”级别。
  //       * 一个“提高亮度”SpanButton，在单击时将亮度提高 1%，在长按时将亮度反复提高 10%，在双击时将亮度跳转到最大亮度。
  //       * 一个“降低亮度”SpanButton，在单击时将亮度降低 1%，在长按时将亮度反复降低 10%，在双击时将亮度跳转到最小亮度。

  // 与往常一样，所有代码都在 DEV_LED.h 中实现，并带有新代码！注释突出显示了与示例 11 相比的变化。您还会注意到，我们已扩展了此版本派生的可调光 LED 服务的构造函数，以包含每个按钮的引脚编号。有关详细信息，请参阅 DEV_LED.h。

  Serial.begin(115200);

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("PushButton LED");
    
    new DEV_DimmableLED(17,23,5,18);          // 新功能！添加了三个额外参数来指定三个 SpanButtons() 的引脚号 - 请参阅 DEV_LED.h
 
} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束

////////////////  补充笔记 ////////////////////////
  
  // 默认值和替代构造函数
// --------------------------------------------

// 如本例所示，下面创建一个 SpanButton，适合通过按钮将引脚 23 连接到 GROUND，并使用
// SpanButton 的 longTime、singleTime 和 doubleTime 默认值：
//
// new SpanButton(23);
//
// 这与明确将每个参数设置为其默认值完全相同：
//
// new SpanButton(23,2000,5,200,SpanButton::TRIGGER_ON_LOW); // 相当于上面的内容
//
// 如果您想创建一个 SpanButton，使用 SpanButton::TRIGGER_ON-HIGH 通过按钮将引脚 23 连接到 VCC，
// 您需要明确设置所有其他参数，即使您对它们的默认值感到满意，因为 triggerType 是构造函数中的最后一个参数：
//
// new SpanButton(23,2000,5,200,SpanButton::TRIGGER_ON_HIGH);
//
// 由于这可能很麻烦，SpanButton 包含一个替代构造函数，其中 triggerType 是第二个参数，而不是最后一个参数。在这种情况下，triggerType 是必需的，但 longTime、singleTime 和 doubleTime 仍然是可选的。
//
// 例如，以下代码创建一个适合将引脚 23 连接到触摸板/传感器的 SpanButton，并使用 SpanButton 的 longTime、singleTime 和 doubleTime 默认值：
//
// new SpanButton(23,SpanButton::TRIGGER_ON_TOUCH);
//
// 这当然相当于：
//
// new SpanButton(23,SpanButton::TRIGGER_ON_TOUCH,2000,5,200);

// 触摸板/传感器校准
// ----------------------------

// SpanButton 利用 ESP32 的内部触摸传感器外设来监控引脚的“触摸”。必须指定许多参数才能准确检测触摸，具体取决于触摸板的确切尺寸和形状。在实例化具有 triggerType=SpanButton::TRIGGER_ON_TOUCH 的 SpanButton() 后，SpanButton 将方便地执行自动校准，为检测触摸设置适当的阈值水平。

// 但是，如果需要，您可以使用以下两个类级函数覆盖此校准过程：
//
// SpanButton::setTouchThreshold() - 明确设置检测触摸的阈值（即覆盖自动校准）
// SpanButton::setTouchCycles() - 明确设置 ESP32 内部触摸外设使用的测量和休眠时间
//
// 有关如何使用这些可选函数的详细信息，请参阅参考 API 的 SpanButton 部分。

// triggerType 函数
// -------------------------

// 虽然 SpanButton 支持的三个 triggerType 对象（SpanButton::TRIGGER_ON_LOW 等）可能看起来只不过是常量，但它们实际上是布尔函数，每个函数都接受一个整数参数。当 SpanButton 调用 triggerType 函数时，它会将构造函数中指定的引脚号作为整数参数传递，并且如果与引脚号关联的“按钮”被“按下”，则 triggerType 函数返回 TRUE，否则返回 FALSE。
//
// 例如，SpanButton::TRIGGER_ON_LOW 和 SpanButton::TRIGGER_ON_HIGH 的定义如下：
//
// boolean TRIGGER_ON_LOW(int pinArg) { return( !digitalRead(pinArg) ); }
// boolean TRIGGER_ON_HIGH(int pinArg) { return( digitalRead(pinArg) ); }
//
// SpanButton::TRIGGER_ON_TOUCH 的定义更加复杂，因为当读取配置为触摸传感器的引脚状态时，ESP32 触摸传感器库会返回 2 字节或 4 字节的数值，而不是简单的 0 或 1。因此，triggerType 函数必须将从触摸传感器引脚读取的值与某个预先计算的“阈值”进行比较，以确定触摸板是否确实被触摸。这是 HomeSpan 为您自动校准的阈值，如上所述。
//
// 让事情变得更加复杂的是，ESP32 触摸引脚的工作方向与 ESP32-S2 和 ESP32-S3 上的触摸引脚相反。
// 在前者中，当触摸触摸板时，从触摸传感器读取的值会减少。在后者中，当触摸触摸板时，值会增加。这意味着对于 ESP32 设备，HomeSpan 对 SpanButton::TRIGGER_ON_TOUCH 使用以下定义：
//
// boolean TRIGGER_ON_TOUCH(int pinArg) { return ( touchRead(pinArg) < Threshold ); }

// 而在 ESP32-S2 和 ESP32-S3 设备上，HomeSpan 使用了一个翻转比较方向的定义：
//
// boolean TRIGGER_ON_TOUCH(int pinArg) { return ( touchRead(pinArg) > Threshold ); }
//
// 对于 ESP32-C3 设备，HomeSpan 根本没有定义 TRIGGER_ON_TOUCH，因为 ESP32-C3 设备上没有触摸引脚！如果您尝试使用 triggerType=SpanButton::TRIGGER_ON_TOUCH 创建 SpanButton，或者调用上述任一校准函数，编译器将抛出错误。
//

// 创建您自己的 triggerType 函数
// --------------------------------------

// 您不限于在 HomeSpan 的三个内置 triggerType 函数中进行选择。您可以创建自己的 triggerType 函数并将其作为 SpanButton 构造函数中的 triggerType 参数传递给 SpanButton。您的函数必须采用 `boolean func(int)` 的形式，并且如果 HomeSpan 传递给您的函数的整数参数所对应的引脚号关联的“按钮”已被“按下”，则应返回 TRUE，否则应返回 FALSE。这样您就可以扩展 SpanButton 的用途，以便与引脚多路复用器、引脚扩展器或可能需要通过第三方库进行自定义处理的任何设备配合使用。
//
// 例如，如果您将 MCP I/O 端口扩展器与 Adafruit mcp 库一起使用，则可以为 MCP 设备上通过按钮接地的引脚创建一个触发器类型函数，如下所示：
//
// boolean MCP_READ(int mcpPin) { return ( !mcp.digitalRead(mcpPin) ); }
//
// 然后只需使用任何 SpanButton 构造函数将 MCP_READ 作为 triggerType 参数传递给 SpanButton：
//
// new SpanButton(23,MCP_READ); // 使用默认的 longTime、singleTime 和 doubleTime
// new SpanButton(23,MCP_READ,2000,5,200); // 明确设置 longTime、singleTime 和 doubletime
// new SpanButton(23,2000,5,200,MCP_READ); // 具有不同顺序参数的替代构造函数
//
// 或者，您可以使用 lambda 函数作为 triggerType 参数，从而在实例化 SpanButton 时动态创建函数：
//
// new SpanButton(23,[](int mcpPin)->boolean{ return ( !mcp.digitalRead(mcpPin) ); });
//
// 注意：如果您创建自己的 triggerType 函数，请不要忘记在实例化使用自定义函数的 SpanButton 之前执行“pin”的任何初始化，或 pin 扩展器的设置/配置等。HomeSpan 无法为您执行此操作。
//
