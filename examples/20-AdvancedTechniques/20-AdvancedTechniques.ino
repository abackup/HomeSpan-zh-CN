/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2022 Gregg E. Berman
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

//////////////////////////////////////////////////////////////////
//                                                              //
//               HomeSpan：ESP32 的 HomeKit 实现                //
//       ------------------------------------------------       //
//                                                              //
//      示例 20：通过实现 Bridge 演示各种高级 HomeSpan 功能，     //
//                  其中可以*动态*添加和删除一个或多个            //
//                  灯泡配件，而无需重新启动设备                  //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "HomeSpan.h"

  // 在示例 20 中，我们将实现一个最多支持 10 个灯泡配件的桥接设备。但是，我们不会预先指定灯的数量，而是允许用户通过 CLI 动态添加和删除灯配件。
  // 更改会反映在 “家庭”应用中，无需重新启动设备！请注意，此示例使用了各种高级 HomeSpan 函数，以及 ESP32-IDF 和 C++ 的一些详细功能，
  // 这些功能在前面的任何示例中都没有使用过。

  // 我们将使用一个包含 10 个元素的 C++ 数组，其中包含表示每个实现的灯泡配件的灯“ID”的整数。ID 为零表示该元素中没有定义灯。
  
#include <array>               // 包含 C++ 标准库数组容器

std::array<int,10> lights;      // 将“lights”声明为一个由 10 个整数组成的数组

using std::fill;                // 将 std 库函数 fill、remove 和 find 放入全局命名空间，以便我们可以在下面使用它们，而无需在前面加上“std::”
using std::find;

  // 我们将使用非易失性存储 (NVS) 来存储灯光数组，以便设备在重启时可以恢复当前配置
  
nvs_handle savedData;           // 将 savdData 声明为与 NVS 一起使用的句柄（有关如何使用 NVS 存储的详细信息，请参阅 ESP32-IDF）


//////////////////////////////////////

void setup() {

  Serial.begin(115200);

  fill(lights.begin(),lights.end(),0);                   // 将灯光数组的 10 个元素全部初始化为零（未定义灯光配件）
  
  size_t len;  
  nvs_open("SAVED_DATA",NVS_READWRITE,&savedData);       // 在 NVS 中打开一个名为 SAVED_DATA 的新命名空间
  if(!nvs_get_blob(savedData,"LIGHTS",NULL,&len))        // 如果找到 LIGHTS 数据
    nvs_get_blob(savedData,"LIGHTS",&lights,&len);       // 检索数据

  homeSpan.setLogLevel(1);

  homeSpan.begin(Category::Lighting,"HomeSpan Lights");

 // 我们首先创建桥梁附件


  new SpanAccessory(1);                                  // 为清楚起见，我们在此指定 AID=1（如果留空，则默认为 1）
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Model("HomeSpan Dynamic Bridge");     // 定义模型是可选的

  //现在我们根据灯光数组中记录的内容创建灯光配件
//我们将使用 C++ 迭代器循环遍历所有元素，直到到达数组末尾，或者找到值为零的元素

  for(auto it=lights.begin(); it!=lights.end() && *it!=0; it++)       // 循环遍历所有元素（到达末尾或遇到值为零的元素时停止）
    addLight(*it);                                                   // 调用 addLight（下面进一步定义），并使用等于该元素中存储的整数作为参数

  // 接下来，我们创建四个用户定义的 CLI 命令，以便我们可以从 CLI 添加和删除灯光配件。
// 每个命令的功能在下面进一步定义。

  new SpanUserCommand('a',"<num> - add a new light accessory with id=<num>",addAccessory);
  new SpanUserCommand('d',"<num> - delete a light accessory with id=<num>",deleteAccessory);
  new SpanUserCommand('D'," - delete ALL light accessories",deleteAllAccessories);  
  new SpanUserCommand('u',"- update accessories database",updateAccessories);

  // 最后，我们调用 autoPoll 开始轮询背景。请注意，这完全是可选的，仅用于说明如何使用 autoPoll - 您也可以通过将其包含在 Arduino loop() 函数中来调用通常的 homeSpan.poll() 函数

  homeSpan.autoPoll();
  
} // setup() 结束

// 通常，Arduino loop() 函数会在此处的某个位置定义。但由于我们在 setup() 函数中使用了 autoPoll，因此我们根本不必在此草图中定义 loop() 函数！
// 为什么我们没有收到错误？因为 HomeSpan 包含一个默认的 loop() 函数，这可以防止编译器抱怨 loop() 未定义。

///////////////////////////

// 此函数创建一个新的灯光配件，其“ID”为 n。
// 它最初在上面的 setup() 中被调用，以根据灯光数组中存储的内容创建灯光配件。它还会在 CLI 中输入“a”（见下文）时被调用，这会在设备运行时动态添加新的灯光配件。

void addLight(int n){

  char name[32];
  sprintf(name,"Light-%d",n);                    // 使用指定的“ID”创建设备的名称
  char sNum[32];
  sprintf(sNum,"%0.10d",n);                      // 根据 ID 创建序列号 - 如果我们使用 “家庭”应用将 Light 重命名为其他名称，这将很有用

  Serial.printf("Adding Accessory: %s\n",name);
  
  new SpanAccessory(n+1);                      // 重要提示：添加 1，因为桥接附件已使用第一个 AID=1 的附件
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name(name);
      new Characteristic::SerialNumber(sNum);
    new Service::LightBulb();
      new Characteristic::On(0,true);  
}

///////////////////////////

// 此函数是在 CLI 中输入“@a <num>”后调用的。
// 它通过调用上面的 addLight(num) 来添加一个 ID=num 的新灯光配件。

void addAccessory(const char *buf){
 
  int n=atoi(buf+1);                                // 读取指定<num>的值

  if(n<1){                                          // 确保 <num> 大于 0
    Serial.printf("Invalid Accessory number!\n");
    return;
  }

  if(find(lights.begin(),lights.end(),n)!=lights.end()){             // 在现有的灯光数组中搜索此 ID - 如果找到，则报告错误并返回
    Serial.printf("Accessory Light-%d already implemented!\n",n);
    return;
  }
  
  auto it=find(lights.begin(),lights.end(),0);                        // 找到光数组中的下一个“空闲”元素（第一个值为零的元素）
  
  if(it==lights.end()){                                                       // 如果没有元素为零，则数组已满，无法添加新的 Lights
    Serial.printf("Can't add any more lights - max is %d!\n",lights.size());
    return;
  }

  *it=n;                                                        // 保存灯光编号
  nvs_set_blob(savedData,"LIGHTS",&lights,sizeof(lights));      // 更新 NVS 中的数据
  nvs_commit(savedData); 
  addLight(n);                                                  // 通过调用上面的函数添加灯光配件！
}

///////////////////////////

// 此函数删除现有的灯光附件，并在 CLI 中输入“@d <num>”时调用。

void deleteAccessory(const char *buf){

  int n=atoi(buf+1);                                  // 与上面相同，我们读取指定的 <num> 并检查它是否有效（即大于 0）

  if(n<1){
    Serial.printf("Invalid Accessory number!\n");
    return;
  }

  // 下面我们使用 homeSpan 方法 deleteAccessory(aid) 完全删除 AID=n+1 的 Accessory。
  // 我们添加 1，因为第一个 Light Accessory 的 AID 为 2，因为桥接附件的 AID 为 1。
  // 如果找到具有匹配 AID 的附件，则 deleteAccessory() 方法返回 true，否则返回 false。
  // 删除 Accessory 时，HomeSpan 将为与该 Accessory 关联的每个 Service、Characteristic、loop() 方法、button() 方法和 SpanButton 打印一条删除消息。
  // 这些是 1 级日志消息，因此您需要将草图中的日志级别设置为 1 或 2 才能接收输出。

  if(homeSpan.deleteAccessory(n+1)){                            // 如果 deleteAccessory() 为真，则表示找到匹配项
    Serial.printf("Deleting Accessory: Light-%d\n",n);
  
    fill(remove(lights.begin(),lights.end(),n),lights.end(),0);     // 从灯光数组中删除条目并用零填充任何未定义的元素
    nvs_set_blob(savedData,"LIGHTS",&lights,sizeof(lights));        // 更新 NVS 中的数据
    nvs_commit(savedData);
    
  } else {   
    Serial.printf("No such Accessory: Light-%d\n",n);
  }
}

///////////////////////////

void deleteAllAccessories(const char *buf){

// 此函数在 CLI 中输入“@D”时被调用。
// 它会删除所有灯光配件

  if(lights[0]==0){                                                   // 首先通过检查 lights[0] 中的非零 ID 来检查至少有一个灯光附件
    Serial.printf("There are no Light Accessories to delete!\n");
    return;
  }

  for(auto it=lights.begin(); it!=lights.end() && *it!=0; it++)       // 使用迭代器循环遍历灯光数组中的所有非零元素......
    homeSpan.deleteAccessory(*it+1);                                  // ...并删除匹配的灯光附件（不要忘记在灯光 ID 上加 1 以形成 AID）

  fill(lights.begin(),lights.end(),0);                          // 将灯光数组中的所有元素清零，因为所有灯光配件都已被删除
  nvs_set_blob(savedData,"LIGHTS",&lights,sizeof(lights));      // 更新 NVS 中的数据
  nvs_commit(savedData);

  Serial.printf("All Light Accessories deleted!\n");
}

///////////////////////////

// 最后，我们还有最重要的 updateAccessories 函数。这是在 CLI 中输入“@u”时调用的。虽然上述函数可用于动态添加和删除灯具配件，但已连接到设备的控制器（如“家庭”应用）
// 尚不知道已将其他灯具配件添加到（或从）整体配件数据库中删除。为了让它们知道，HomeSpan 需要增加 HAP 配置编号并通过 MDNS 重新广播，以便所有连接的控制器都知道它们需要从设备请求刷新。

// 当您在 CLI 中输入“@u”时，您应该会看到设备和任何连接的控制器之间有大量活动，因为它们请求刷新。请耐心等待 - 更改可能需要一分钟才能正确反映在您的 iPhone 或 Mac 上的 “家庭”应用中。

void updateAccessories(const char *buf){

  // 注意，如果数据库确实发生了变化（例如添加了一个或多个新的灯具配件），则 updateDatabase() 方法返回 true；如果没有任何变化，则返回 false
  
  if(homeSpan.updateDatabase())
    Serial.printf("Accessories Database updated.  New configuration number broadcasted...\n");
  else
    Serial.printf("Nothing to update - no changes were made!\n");
}

///////////////////////////
