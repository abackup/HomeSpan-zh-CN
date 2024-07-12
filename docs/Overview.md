<!-- 原文：2023.3.19，翻译时间：2024.5.13，校对时间：2024.7.12  -->

# HomeSpan API 概述

 HomeSpan 草图的基本结构如下：

```C++
#include " HomeSpan.h"         // 包括 HomeSpan 库

void setup() {     
 
  Serial.begin(115200);       // 启动串口接口
  
  /// HomeSpan 设置代码在这里 ///
  
} // 设置结束

void loop(){

  /// HomeSpan 循环代码在这里 ///

} // 循环结束
```

包含  HomeSpan.h 会创建一个名为 `HomeSpan` 的全局对象，它实现了各种方法。最重要的两个是 `begin()` 和 `poll()`。`begin()` 方法采用许多可选参数，初始化 HomeSpan 并放置在 `setup()` 部分的开头附近。不带参数的 `poll()` 方法被放置在 `loop()` 中，这也是 HomeSpan 运行其所有代码的原因。它通常是放在 `loop()` 部分中的唯一函数。我们的草图现在看起来像这样：

```C++
#include " HomeSpan.h"         // 包括 HomeSpan 库

void setup() {     
 
  Serial.begin(115200);       // 启动串口接口
  
   HomeSpan.begin();           // 初始化 HomeSpan 
  
  /// HAP 附件属性数据库的定义在此处 ///
  
  // HomeSpan.autoPoll()；需要注释掉 loop 函数中的 HomeSpan.poll()
  
} //设置结束

void loop(){

  HomeSpan.poll(); 

} // 循环结束
```
请注意，作为**替代**方法，你可以指示 HomeSpan 创建单独的任务，在后台重复调用 `HomeSpan.poll()`。为此，请将 `loop()` 中对 `HomeSpan.poll()` 的调用替换为 `setup()` 函数末尾对 `HomeSpan.autoPoll()` 的调用：

```C++
#include " HomeSpan.h"         // 包括 HomeSpan 库

void setup() {     
 
  Serial.begin(115200);       // 启动串口接口
  
   HomeSpan.begin();           // 初始化 HomeSpan 
  
  /// HAP 附件属性数据库的定义位于此处 ///
  
  HomeSpan.autoPoll();         // 启动一个在后台重复调用 HomeSpan.poll() 的任务

} // 设置结束

void loop(){

} //循环结束
```

这在使用双核处理器时特别有效，因为 HomeSpan 将在“空闲”处理器上运行轮询任务，否则该处理器不会执行任何其他 Arduino 功能。

## 创建 HAP 附件属性数据库

下一步是实现定义 HAP 附件属性数据库的代码，该数据库不是真正的数据库，而只是此 HomeSpan 设备实现的所有 HAP 附件对象、服务对象和特征对象的列表。

你可以通过实例化一个新的 `SpanAccessory` 对象来创建一个新的 HAP 附件，如下所示：

```C++
new SpanAccessory();
```

SpanAccessory 可以在没有任何参数的情况下实例化，并且你不需要将对象保存在变量中，因为 HomeSpan 会自动在 `HomeSpan` 对象中注册附件。

创建 HAP 附件后，你可以通过实例化 HomeSpan 服务和特征对象开始添加 HAP 服务和 HAP 特征。HomeSpan 支持的每个 HAP 服务都在 `Service` 命名空间中定义。HomeSpan 支持的每个 HAP 特征都在 `Characteristic` 命名空间中定义。有关完整列表，请参阅 [服务和特征](ServiceList.md)。

例如，要将 HAP 二氧化碳传感器服务添加到附件，只需实例化相应的 HomeSpan 服务对象，如下所示：

```C++
new Service::CarbonDioxideSensor();
```

HomeSpan 服务对象不带参数，你不需要将对象保存在变量中。HomeSpan 将自动注册此服务并将其附加到你定义的最后一个附件。

HAP 特征以类似的方式添加到服务中。例如要添加 HAP 检测到的二氧化碳特征，只需按如下方式实例化相应的 HomeSpan 特征对象：

```C++
new Characteristic::CarbonDioxideDetected();
```

HomeSpan 将自动注册此特征并将其附加到你定义的最后一个服务。但是与附件和服务对象不同，特征确实采用单个可选参数（上面示例行未显示），该参数在启动时初始化特征的值。如果你不指定参数，HomeSpan 将使用合理的默认值。

因此，通过简单地实例化一个或多个 SpanAccessory 对象来定义一个完整的 HAP 附件属性数据库，每个 SpanAccessory 对象具有一个或多个服务对象，而这些对象又包含一个或多个特征对象。例如一个 HomeSpan 设备支持一个带有非可调光吸顶灯的变速、可逆吊扇和一个带有可调光灯泡的台灯，可以按如下方式实现：

```C++
#include " HomeSpan.h"         // 包括 HomeSpan 库

void setup() {     
 
  Serial.begin(115200);       // 启动串口接口
  
   HomeSpan.begin();           // 初始化 HomeSpan 

  new SpanAccessory();        //带非调光吸顶灯的可逆变速吊扇
   new Service::Fan();       
    new Characteristic::Active();          
    new Characteristic::RotationSpeed(100);   // 将默认速度初始化为 100%
    new Characteristic::RotationDirection();
   new Service::LightBulb();
    new Characteristic::On();
    
  new SpanAccessory();        // 可调光台灯
   new Service::LightBulb();
    new Characteristic::On();
    new Characteristic::Brightness(50);     // 将默认亮度初始化为 50%
  
} // 结束设置

void loop(){

  HomeSpan.poll(); 

} //结束循环
```

如你所见，你不需要命名任何对象或指定任何 HAP 参数，例如格式类型、UUID 代码等。但是，实例化对象的**顺序**至关重要。特征自动与最后实例化的服务相关联，服务自动与最后实例化的附件相关联。

> :heavy_check_mark: HomeSpan 具有广泛的错误检查功能。在启动时 HomeSpan 将验证你实例化的 HAP 附件属性数据库的配置，以确保每个附件都具有所有必需的服务，并且每个服务都具有其所有必需的特征。如果 HomeSpan 发现附件缺少必需的服务，服务缺少必需的特征，或者既非必需也非可选的特征已添加到不支持该特征的服务中，HomeSpan 将报告这些错误并停止程序。

事实上，如果你尝试运行上面的草图，你会发现它无法验证。这是因为每个附件都缺少所需的服务和特征 - HAP 附件信息服务和识别特征。请参阅 [教程](Tutorials.md) 以获取包含所有必需 HAP 元素的各种完整且有效的示例，例如简单台灯的此草图：

```C++
/* HomeSpan Table Lamp 示例 */

#include " HomeSpan.h"         // 包含 HomeSpan 库

void setup() {     
 
  Serial.begin(115200);       // 开启串口
  
   HomeSpan.begin();           // 初始化  HomeSpan 

  new SpanAccessory();           // 台灯配件
  
    new Service::AccessoryInformation();            // HAP 要求每个附件都实现一个 AccessoryInformation 服务
      new Characteristic::Identify();               // HAP 要求附件信息服务包括识别特征
        
    new Service::LightBulb();                       // 创建灯泡服务
      new Characteristic::On();                     // 存储灯泡状态的特征：开或关
  
} // 结束 setup()

void loop(){

  HomeSpan.poll(); 

} // 结束 loop()
```

## 将 HomeSpan 与真实世界的电器连接

上面的台灯示例是一个功能齐全的 HomeSpan 草图。如果你将其上传到你的 ESP32 并将它与 HomeKit 配对，你会发现“家庭”应用中出现一个标有“我的台灯”的新图块。图块是可操作的。按一次，它显示灯打开。再按一次，指示灯熄灭。当然，现实世界中什么也没有发生 - HomeSpan 还没有被编程来打开一个实际的灯。

当你按下“家庭”应用中的图块时，HomeKit 会向 HomeSpan 发送请求以*更新*一个或多个特征。在上面的示例中，按下图块会导致 HomeKit 专门请求将灯泡服务的开关特征更新为 true (on) 或 false (off)。当 HomeSpan 收到更新一个或多个特征的请求时，而不是顺序调用代码来分别更新每个特征，HomeSpan 而是根据它们的服务对请求进行分组，并为该服务调用 `update()` 方法。这是因为许多服务支持物理相关的多个特征。例如，打开一盏灯并将其亮度设置为 50% 需要更新两个特征（开和亮度）。在这些按顺序处理的系统中，用户需要编写复杂的代码来保存中间状态，并确定当它收到打开灯的请求时要做什么，如果还不知道设置亮度的请求是否会紧随其后。

HomeSpan 使用的**以服务为中心**方法使得实现代码来处理更新变得更加容易。你无需编写代码来响应单个特征的更新，而是编写代码来响应整个服务的更新，并使用方法通知你请求更新哪些特征，以及它们的新对应值。

每个 HomeSpan 服务都实现了一个虚拟的 `update()` 方法，默认情况下什么都不做。要实现你自己的逻辑，你可以使用自己的代码重写 `update()`。最简单的方法是创建一个新服务，该服务派生自你要使用自己的 `update()` 方法自定义的服务。例如，我们可以从 LightBulb 派生一个名为 TableLamp 的新服务，如下所示：

```C++
struct TableLamp : Service::LightBulb {};
```

在这个新结构中，我们可以创建自己的构造函数，可能带有一个或多个参数。构造函数可以存储一些特定于设备的参数，例如将用于驱动打开和关闭台灯的继电器的 ESP32 引脚的编号。构造函数还应定义服务所需的所有特征。你还需要将至少一些特征对象保存为命名变量，以便稍后在 `update()` 方法中引用它们。所有 HomeSpan 特征的基类是 `SpanCharacteristic`，因此创建一个变量来存储 HomeSpan 特征需要你将其定义为 `SpanCharacteristic *`。综上所述，到目前为止，我们有：

```C++
struct TableLamp : Service::LightBulb{

  int lampPin;                               // 存储连接到转动表的假设 Lamp on/off 继电器的引脚号 
  SpanCharacteristic *lampPower;             // 存储对 On 特征的引用
  
  TableLamp(int lampPin) : Service::LightBulb(){       // 使用一个参数定义的 TableLamp 的 constructor() 方法。请注意，我们还调用了 LightBulb Service 的 constructor() 方法。

    lampPower=new Characteristic::On();      // 实例化开关特征并将其保存为 lampPower
    this->lampPin=lampPin;                   // 保存假设继电器的引脚号
    pinMode(lampPin,OUTPUT);                 // 使用标准 Arduino pinMode 函数将引脚配置为输出                    
    
  } // 结束构造
};
```

现在我们准备好实现我们的 `update()` 方法了。虽然 `update()` 没有参数，但它确实有一个布尔返回值，它让 HomeKit 知道它更新台灯的请求是成功（true）还是不成功（false）。在 `update()` 方法中，你将可以访问服务中的所有特征，在这种情况下，就是我们命名为 lampPower 的 On 特征。HomeSpan 特征实现了多种方法，这些方法提供了有关特征状态、是否有更新它们的请求以及更新值应该是什么的信息。由于我们的 TableLamp 服务只定义了一个特征，这意味着每当调用 `update()` 时，HomeKit 都必须请求更新 On 特征，我们不必检查它是否正在更新。但是，我们确实需要知道 HomeKit 请求的值。要检索此值，我们使用 `getNewVal()` 方法，如下所示：`lampPower->getNewVal()`。这将返回 HomeKit 请求的开关特征的新值。这将是真 (1) 或假 (0)，具体取决于 HomeKit 是否希望打开或关闭灯。因此，我们可以使用这个值来相应地更新 lampPin。我们完整的 TableLamp 服务现在看起来像这样：

```C++
struct TableLamp : Service::LightBulb{

  int lampPin;                               // 存储连接到打开/关闭台灯的假设继电器的引脚号
  SpanCharacteristic *lampPower;             // 存储对 On 特征的引用
  
  TableLamp(int lampPin) : Service::LightBulb(){       // 使用一个参数定义的 TableLamp 的 constructor() 方法。请注意，我们还调用了 LightBulb Service 的 constructor() 方法。

    lampPower=new Characteristic::On();      // 实例化开关特征并将其保存为 lampPower
    this->lampPin=lampPin;                   // 保存假设继电器的引脚号
    pinMode(lampPin,OUTPUT);                 // 使用标准 Arduino pinMode 函数将引脚配置为输出                       
    
  } // 结束构造
  
  boolean update(){                          // update()方法

    digitalWrite(lampPin,lampPower->getNewVal());      // 使用标准 Arduino digitalWrite 函数根据 HomeKit 请求的值将 ledPin 更改为高或低
   
    return(true);                            // 返回 true 让 HomeKit（和“家庭”应用Client）知道更新成功
  
  } // 结束更新
  
};
```

最后，我们将派生的 TableLamp 服务添加到原始草图中，并替换我们的通用灯泡服务和开关特征的实例化：

```C++
new Service::LightBulb();                       // 创建灯泡服务
  new Characteristic::On();                       // 存储灯泡状态的特征：开或关
```

TableLamp 服务的实例化如下：

```C++
new TableLamp(17);
```

其中 17 指定连接到用于打开和关闭台灯的假设继电器的 ESP32 引脚号。

下面完整介绍了我们完成的用于操作台灯的 HomeSpan 草图。请注意，它也可以在 Arduino IDE 中的 *文件→示例→HomeSpan→其他示例→TableLamp* 下使用。

```C++
/* HomeSpan Table Lamp 示例 */

#include " HomeSpan.h"         // 包含 HomeSpan 库

//// 定义 TableLamp 服务 ////

struct TableLamp : Service::LightBulb{

  int lampPin;                               // 存储连接到打开/关闭台灯的假设继电器的引脚号
  SpanCharacteristic *lampPower;             // 存储对 On 特征的引用
  
  TableLamp(int lampPin) : Service::LightBulb(){       // 使用一个参数定义的 TableLamp 的 constructor() 方法。请注意，我们还调用了 LightBulb Service 的 constructor() 方法。

    lampPower=new Characteristic::On();      // 实例化开关特征并将其保存为 lampPower
    this->lampPin=lampPin;                   // 保存假设继电器的引脚号
    pinMode(lampPin,OUTPUT);                 // 使用标准 Arduino pinMode 函数将引脚配置为输出                  
    
  } // 结束构造
  
  boolean update(){                          // update() method

    digitalWrite(lampPin,lampPower->getNewVal());      // 使用标准 Arduino digitalWrite 函数根据 HomeKit 请求的值将 ledPin 更改为高或低
   
    return(true);                            // 返回 true 让 HomeKit（和“家庭”应用Client）知道更新成功
  
  } // 结束更新
  
};

//// 初始化 HomeSpan 并创建 HAP 配件属性数据库 ////

void setup() {     
 
  Serial.begin(115200);       // 开启串口
  
   HomeSpan.begin();           // 初始化 HomeSpan 

  new SpanAccessory();           // Table Lamp Accessory
  
    new Service::AccessoryInformation();            // HAP要求每个 Accessory 实现 AccessoryInformation Service
      new Characteristic::Identify();               // HAP要求配件信息服务包含识别特征
       
   new TableLamp(17);                               // 实例化 TableLamp 服务（定义如下），并将 lampPin 设置为 17
  
} // 结束设置

////运行HomeSpan ////

void loop(){

  HomeSpan.poll(); 

} // 结束循环
```

这个完整的工作示例已准备好上传到你的 ESP32，可用于操作通过继电器连接到引脚 17 的虚拟台灯。或者你可以简单地将 LED 连接到引脚 17，看看它是如何工作的！ 恭喜，你已经创建了你的第一个 HomeSpan 草图。

## 多文件草图

将所有 TableLamp 函数封装到单个结构中表明 TableLamp 代码可以存储在单独的文件中。由于 TableLamp 表示 HAP 的通用灯泡服务的**设备特定**实现，因此建议的约定是将 DEV_ 作为前缀添加到所有此类结构中，并将它们放在具有类似 DEV_ 前缀的 `\*.h` 文件中。例如，我们将 `struct TableLamp` 更改为 `struct DEV_TableLamp` 并将其代码放在单独的文件名 `DEV_TableLamp.h` 中。要使用时，我们只需将 `#include "DEV_TableLamp.h"` 添加到我们的 \*.ino 草图文件中，通常在`#include "HomeSpan.h"`之后。

以这种方式，我们可以为现实世界的灯、吊扇、窗帘、车库门开启器，甚至只是控制独特设备的一系列开关创建一个设备特定结构的“库”。通过将每个结构（或一组相关结构）存储在其自己的 `DEV_\*.h` 文件中，并使用记录良好的构造函数，并仅包含特定 HomeSpan 草图所需的特定于设备的结构，我们提高了整体可读性、可移植性和可重用性。你会看到在 HomeSpan 的教程草图中使用了这个约定。

# 探索更多

尽管上面的示例很好地说明了基础的 HomeSpan 草图，但它仅涉及 HomeSpan 功能的基础。了解全部 HomeSpan 功能的最佳方式是探索 [教程](Tutorials.md)，可以在 GitHub 上在线查看，也可以在 Arduino IDE 中打开每个教程草图。后一种方法是首选，因为你还可以编译教程草图并将其上传到你的设备以查看它们的实际效果。

此外，你应该通读 [API 参考](Reference.md) 以获取有关每个 HomeSpan 对象、结构、方法和函数的完整详细信息，包括任何教程中未涵盖的一些较少使用的函数。

最后，别忘了访问 [命令行界面](CLI.md)。HomeSpan命令行界面是你可以找到实时诊断并能够监视设备状态的地方。最重要的是，HomeSpan命令行界面可用于使用家庭网络配置你设备的 WiFi 凭据和 HomeKit 设置代码，以便你可以将设备与“家庭”应用配对。

---

[↩️](../README.md#resources) 返回欢迎页面
