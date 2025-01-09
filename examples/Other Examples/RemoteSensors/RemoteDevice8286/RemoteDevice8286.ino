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

#ifndef ARDUINO_ARCH_ESP8266
#error ERROR: THIS SKETCH IS DESIGNED FOR ESP8266 MICROCONTROLLERS!
#endif

// *** 此草图适用于 ESP8266，不适用于 ESP32 *** //

// 此草图类似于 HomeSpan 的 RemoteDevice.ino 示例（专为运行 HomeSpan 的 ESP32 设计），
// 其中我们使用 HomeSpan 的 SpanPoint 类模拟远程温度传感器。但是，由于 HomeSpan 和 SpanPoint 
// 都不是设计用于在 ESP8266 上运行的，因此我们将通过直接调用 ESP8266 支持的等效 ESP-NOW 命令来
// 实现 SpanPoint 的基本通信功能。此草图并不试图复制 SpanPoint 的所有功能，也不包括自动通道校
// 准或队列管理。

// 首先包含以下 ESP8266 库

#include <ESP8266WiFi.h>                 
#include <espnow.h>
#include <Crypto.h>       // 需要此库来实现 SpanPoint 用于生成 ESP-NOW 加密密钥的哈希码过程

float temp=-10.0;         // 这个全局变量代表我们的“模拟”温度（以摄氏度为单位）

// 下面我们对运行 HomeSpan 的主 ESP32 设备的 MAC 地址进行编码，该 ESP8266 设备将连接到该设备

// 重要提示：ESP32 设备有两个 MAC 地址。一个用于 ESP32 在站点 (STA) 模式下工作。它是 WiFi.macAddress() 
// 函数返回的地址。另一个用于 ESP32 在接入点 (AP) 模式下工作。此地址由 WiFi.softAPmacAddress() 函数返回。
// HomeSpan 通常以两种模式 (STA+AP) 操作 ESP32，因此两个 MAC 地址都处于活动状态。

// 在 ESP32 设备上，当每个设备通过其 STA MAC 地址向其他设备发送数据时，ESP-NOW 似乎工作正常。
// ESP8266 设备通过 ESP-NOW 向 ESP32 设备发送数据也是如此，但有一个关键例外：一旦 ESP32 连接到 WiFi 网络
// （必须通过 STA 模式才能运行 HomeSpan），出于某种原因，ESP8266 设备无法再使用其 STA MAC 地址通过 ESP-NOW 向 ESP32 发送数据。

// 解决方案是让 ESP8266 通过 ESP-NOW 将数据发送到 ESP32 的 AP MAC 地址。无论 ESP32 是否连接到中央 WiFi 网络，这似乎都有效。为了支持在 ESP32 上进行此类使用，SpanPoint 构造函数包含第五个可选参数“useAPaddress”。使用 HomeSpan 创建 ESP32 的 SpanPoint 链接时，如果 SpanPoint 连接的远程设备是 ESP8266，则将 useAPaddress 设置为 TRUE。如果远程设备是 ESP32，则将“useAPaddress”设置为 FALSE（或保留未指定，因为 FALSE 是默认值）。

// 当 HomeSpan 首次启动时（以及每当您在 CLI 中输入“i”时），串行监视器将显示您在 ESP32 草图中实例化的每个 SpanPoint 对象的详细信息。此输出包括 SpanPoint 将监听来自远程设备的传入数据的 MAC 地址。显示的与此远程设备（即此草图）对应的 SpanPoint 实例的 MAC 地址是您应在下面使用的 MAC 地址。
 
uint8_t main_mac[6]={0xAC,0x67,0xB2,0x77,0x42,0x21};        // 这是 HomeSpan 串行监视器中报告的在 ESP32 上运行 HomeSpan 的主设备的 **AP MAC 地址**

// 接下来我们创建一个简单的、标准的 ESP-NOW 回调函数来报告每次数据传输的状态

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.printf("Last Packet Send Status: %s\n",sendStatus==0?"Success":"Fail");
}

//////////////////////

void setup() {

  Serial.begin(115200);
  delay(1000); 
  Serial.printf("\nMAC Address: %s\n",WiFi.macAddress().c_str());         // 将此 MAC 地址作为运行 HomeSpan 的 ESP32 上匹配的 SpanPoint 对象的第一个参数输入
  
  WiFi.mode(WIFI_STA);            // 将模式设置为站点
  wifi_set_channel(3);            // 您还需要手动设置频道，以匹配 ESP32 连接到 WiFi 网络后使用的频道

  // 提示：作为替代方案，您可以向此草图添加代码以连接到 HomeSpan 使用的相同 WiFi 网络。虽然此草图不会使用该 WiFi 网络，但通过建立连接，ESP8266 会自动配置通道，现在将与 ESP32 匹配。
  // Next, initialize ESP-NOW
  
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // SpanPoint 使用 ESP-NOW 加密进行所有通信。此加密基于两个 16 字节密钥：本地主密钥 (LMK) 和主主密钥 (PMK)。为了生成这些密钥，SpanPoint 采用基于文本的密码（默认为单词“HomeSpan”），创建 32 字节（256 位）文本哈希值（使用 SHA256 方法），并使用前 16 个字节作为 LMK，后 16 个字节作为 PMK。这很容易复制，如下所示：
  
  uint8_t hash[32];                 // 创建空间来存储 32 字节哈希码
  char password[]="HomeSpan";       // 指定密码
  
  experimental::crypto::SHA256::hash(password,strlen(password),hash);     // 创建下面要使用的哈希码

  esp_now_register_send_cb(OnDataSent);                   // 注册我们上面定义的回调函数
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);         // 将此设备的角色设置为控制器（即，它将数据发送到 ESP32）

  esp_now_set_kok(hash+16,16);        //接下来我们设置 PMK。出于某种原因，这在 ESP8266 上称为 KOK。请注意，您必须在添加任何对等点之前设置 PMK
        
  esp_now_add_peer(main_mac, ESP_NOW_ROLE_COMBO, 0, hash, 16);    // 现在我们添加对等体，设置其角色，并指定 LMK

  // 提示：上面的第三个参数是 WiFi 通道。但是，这只是 ESP-NOW 存储的参考编号。ESP-NOW 实际上不会为您设置通道。我们已经在上面设置了 WiFi 通道。为了方便起见，ESP-NOW 允许您将通道设置为零，这意味着 ESP-NOW 应该期望通道是已经为 WiFi 控制器设置的任何通道。建议始终将其设置为零，以避免在您指定真实通道时出现任何不匹配的情况。
}

//////////////////////

void loop() {

  Serial.printf("Sending Temperature: %f\n",temp);  
  esp_now_send(main_mac, (uint8_t *)&temp, sizeof(temp));     // 将数据发送到主设备！

  temp+=0.5;       // 将“温度”增加 0.5 C
  if(temp>35.0)
    temp=-10.0;

  delay(5000);    // 等待 5 秒后再发送另一个更新
}
