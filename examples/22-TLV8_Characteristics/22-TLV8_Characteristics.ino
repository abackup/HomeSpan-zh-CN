/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2020-2024 Gregg E. Berman
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

////////////////////////////////////////////////////////////////
//                                                            //
//                HomeSpan：ESP32 的 HomeKit 实现             //
//      ------------------------------------------------      //
//                                                            //
//      示例 22：通过实现 DisplayOrder 演示如何使用 TLV8 库，  //
//               DisplayOrder 是与 TV 服务一起使用的           //
//              可选 TLV8 特性，用于设置在 Home 应用           //
//              中显示的 TV Input 的顺序以供选择               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "HomeSpan.h"

// 注意：请参阅“其他示例 -> 电视”草图，了解如何实现电视服务的完整详细信息。此草图的重点仅是演示如何使用 TLV8 库创建 TLV8 数据以用于 DisplayOrder 特性。

// 首先我们定义一个简单的电视输入源服务

struct TVInput : Service::InputSource {

  SpanCharacteristic *inputID;
  SpanCharacteristic *inputName;
  
  TVInput(uint32_t id, const char *name) : Service::InputSource() {

     inputID = new Characteristic::Identifier(id);
     inputName = new Characteristic::ConfiguredName(name);
     new Characteristic::IsConfigured(Characteristic::IsConfigured::CONFIGURED);
     new Characteristic::CurrentVisibilityState(Characteristic::CurrentVisibilityState::VISIBLE);
  }
};

// 接下来我们定义一个简单的电视服务

struct HomeSpanTV : Service::Television {

  SpanCharacteristic *active = new Characteristic::Active(0);
  SpanCharacteristic *activeID = new Characteristic::ActiveIdentifier(10);
  
  SpanCharacteristic *displayOrder;       //  创建一个指针，用于新的 TLV8 DisplayOrder 特征，一旦我们构建 TLV8 记录，它将在下面实例化

  HomeSpanTV() : Service::Television() {

    // 在实例化 displayOrder 之前，我们需要构建一个包含 DisplayOrder Characteristic 所需信息的 TLV8 对象。DisplayOrder Characteristic 的 TLV8 规范（Apple 未记录）如下：

    // 标签     名称             格式       描述
    // ----    -------------    ------     --------------------------------------------
    // 0x01    inputSourceID    uint32     首先要显示的输入源的 ID
    // 0x00    separator        none       用于分隔 inputSourceID 的空元素
    // 0x01    inputSourceID    uint32     第二个要显示的输入源的 ID
    // 0x00    separator        none       用于分隔 inputSourceID 的空元素
    // 0x01    inputSourceID    uint32     第三个要显示的输入源的 ID
    // 0x00    separator        none       用于分隔 inputSourceID 的空元素
    // etc...

    // 首先，实例化一个新的 TLV8 对象
           
    TLV8 orderTLV;              // 创建一个空的 TLV8 对象

    // 接下来，根据上述规范用标签和值填充它。最简单（但不一定是最优雅）的方法是简单地添加每个标签/值，如下所示：

    orderTLV.add(1,10);        // TAG=1，VALUE=要显示的第一个输入源的 ID
    orderTLV.add(0);           // TAG=0  (没有值)
    orderTLV.add(1,20);        // TAG=1，VALUE=要显示的第二个输入源的 ID
    orderTLV.add(0);           // TAG=0  (没有值)
    orderTLV.add(1,50);        // TAG=1，VALUE=要显示的第三个输入源的 ID
    orderTLV.add(0);           // TAG=0  (没有值)
    orderTLV.add(1,30);        // TAG=1，VALUE=要显示的第四个输入源的ID
    orderTLV.add(0);           // TAG=0  (没有值)
    orderTLV.add(1,40);        // TAG=1，VALUE=要显示的第五个输入源的ID

    // 根据上述结构，我们预计“家庭”应用将根据其 ID 显示我们的输入源，顺序如下：10、20、50、30、40。
   // 这些 ID 当然必须与您在 setup() 中此草图末尾创建输入源时选择的 ID 相匹配

    // 现在我们可以使用上面创建的 TLV8 对象作为其初始值来实例化 displayOrder

    displayOrder = new Characteristic::DisplayOrder(orderTLV);           // 将 DisplayOrder 的“值”设置为我们刚刚创建的 orderTLV 对象

    // 就这样——您已经创建了您的第一个 TLV8 特征！
  }

  // 下面我们定义常用的 update() 循环。这部分代码与 TLV 无关
  
  boolean update() override {

    if(active->updated()){
      LOG0("Set TV Power to: %s\n",active->getNewVal()?"ON":"OFF");
    }

    if(activeID->updated()){
      LOG0("Set Input Source to ID=%d\n",activeID->getNewVal());        
    }      

  return(true);
  }
};

///////////////////////////////

void setup() {
  
  Serial.begin(115200);

  homeSpan.setLogLevel(2);
 
  homeSpan.begin(Category::Television,"HomeSpan Television");

  SPAN_ACCESSORY();      

  (new HomeSpanTV())                             // 定义电视服务并链接到输入源！
    ->addLink(new TVInput(10,"Xfinity"))
    ->addLink(new TVInput(20,"BlueRay Disc"))
    ->addLink(new TVInput(30,"Amazon Prime"))
    ->addLink(new TVInput(40,"Netflix"))
    ->addLink(new TVInput(50,"Hulu"))
    ;
      
}

//////////////////////////////////////

void loop(){ 
  homeSpan.poll();
}

//////////////////////////////////////
