<!--   原文时间：2024.2.18，翻译时间：2024.5.6，校对时间：2024.7.12   -->

# 消息记录

HomeSpan 包括各种具有不同详细级别的消息日志，以及用于创建你自己的日志消息和网络日志的内置方法。

## HomeSpan 日志消息

HomeSpan 日志消息通常以三种可能的详细级别直接输出到 Arduino 串口监视器：

|日志级别|输出|
|:---------:|------|
|0级|HomeSpan 配置数据和一些基本状态信息|
|1级|0 级中的所有内容以及其他更详细的状态消息|
|2级|1 级的所有内容以及发送到 HomeSpan 设备和从 HomeSpan 设备发送的所有 HAP 通信包|

你可以使用方法 `homeSpan.setLogLevel(uint8_t level)` 在草图中设置 *Log Level*，如 [API 参考](Reference.md) 中所述。始终输出 0 级消息； 仅当 *Log Level* 设置为 1 或更高时才输出级别 1 消息； 和级别 2 消息仅在 *Log Level* 设置为 2 时输出。*Log Level* 也可以随时通过串口监视器动态更改，方法是键入 "L0" 、"L1" 或 "L2",如 [命令行界面](CLI.md) 中所述。

你还可以通过在 HomeSpan命令行界面中键入 "L-1" 或在草图中调用 `homeSpan.setLogLevel(-1)` 将日志级别设置为-1，完全抑制 HomeSpan 生成的所有日志消息（以及所有用户定义的日志消息-参见下文）。在 ESP 32 控制单独的串口外设的情况下，禁止将所有日志消息输出到串口监视器可能很有用。在这种情况下，你可能希望在设备上实现一个物理开关，自动将日志级别设置为0或-1，这样你就不必在每次要启用/禁用 HomeSpan 日志消息时重新编译草图。

请注意，“日志级别”设置对草图中使用的任何 `Serial.print()` 或 `Serial.printf()` 语句输出的消息没有影响。要确保你可以通过日志级别设置控制此类消息，请使用下面的` LOG() `宏。另请注意，日志级别设置对 ESP32 操作系统本身生成的任何 ESP32 诊断消息没有影响。这些消息根据 Arduino IDE 的工具菜单下编译时指定的内核编译级别进行控制。

## 用户定义的日志消息

你可以使用 HomeSpan 的 **LOG0()**、**LOG1()** 和 **LOG2()** 宏将自己的日志消息添加到任何草图。使用这些宏创建的消息将根据上述 *Log Level* 设置输出到 Arduino 串口监视器。每个 **LOGn()** 宏（其中 n=\[0,2\]）根据指定的参数数量有两种形式：

* `LOGn(val)` - 当只指定一个参数时，HomeSpan 使用标准的 Arduino `Serial.print(val)` 方法输出 *val*，这意味着 *val* 几乎可以是任何 timvariable 类型。缺点是你无法控制格式。例如，`int n=255; LOG1(n);` 向 Arduino 串口监视器输出数字 "255"，前提是 *Log Level* 设置为 1 或更大。

* `LOGn(const char *fmt, ...)` - 当指定多个参数时，HomeSpan 使用 ESP32 `Serial.printf(fmt, ...)` 方法输出消息，该方法允许你格式化消息使用标准 C++ *printf* 约定具有可变数量的参数。例如，`int n=255; LOG2("The value is 0x%X",n);` 向 Arduino 串口监视器输出消息 "The value is 0xFF"，前提是 *Log Level* 设置为 2。

有关演示这些宏的教程草图，请参阅 [示例 9 - 消息日志](Tutorials.md#示例-9---消息日志)。
 
## 网络日志

除了将消息记录到 Arduino 串口监视器之外，HomeSpan 还可以选择在你选择的任何页面地址提供网络日志页面。由于网络日志作为 HomeSpan 的 HAP 服务器的一部分托管，因此其基地址和端口将与你的设备相同。例如，如果你的设备名称是 *http<nolink>://homespan-4e8eb8504e59.local*（假设端口为 80）并且你选择 "myLog" 作为网络日志页面地址，它将托管在 *http<nolink> ://homespan-4e8eb8504e59.local/myLog*。

HomeSpan 的网络日志功能还嵌入了调用 NTP 时间服务器来设置设备时钟的能力。此可选功能允许 HomeSpan 创建基于时钟的时间戳（例如 *Sat Apr 16 19:48:41 2022*）。

HomeSpan网络日志页面本身包含两个部分：
 
  * 页面顶部提供 HomeSpan 生成的状态信息，例如设备名称、自上次重启以来的总正常运行时间以及各种软件组件的版本号
 
  * 页面底部发布你使用 **WEBLOG()** 宏创建的消息。此宏仅以 *printf* 样式的形式 `WEBLOG(const char *fmt, ...)` 出现，类似于上述 LOG() 宏的第二个版本。
 
使用 WEBLOG() 生成的消息*也*以与 LOG1() 消息相同的优先级回显到 Arduino 串口监视器，这意味着如果 *Log Level* 设置为 1 或更高，它们将输出到串口监视器。网络日志页面按时间倒序显示消息，并补充了以下附加项目：
* *Entry Number* - HomeSpan 为每条消息编号，从 1 开始表示重启后的第一条消息
* *Up Time* - 相对消息时间，格式为 DDD:HH:MM:SS，从重启后的 000:00:00:00 开始
* *Log Time* - 标准 UNIX 格式的绝对消息时间，前提是网络日志已通过 NTP 时间服务器启用（见下文）
* *Client* - 创建 WEBLOG() 消息时连接到 HomeSpan 的客户端的 IP 地址。仅适用于在服务中的 `update()` 方法中产生的消息。客户端在所有其他实例中设置为 "0.0.0.0"
* *Message* - 格式化消息的文本。例如，`int ledNumber=5; WEBLOG("Request to turn LED %d OFF\n",ledNumber);` 会产生消息 "Request to turn LED 5 OFF"

要启用网络日志记录（默认情况下关闭），请调用方法 `homeSpan.enableWebLog()`，如在草图顶部附近的 [API 参考](Reference.md) 中更全面地描述。此方法允许你设置：

* 要存储的 WEBLOG() 消息的总数——一旦达到你设置的限制，旧消息将被丢弃以支持新消息
* NTP 时间服务器的 URL - 这是可选的，仅当你想在启动时设置设备的时钟时才需要
* 设备的时区 - 仅在指定 NTP 时间服务器时才需要
* 网络日志页面的 URL - 如果未指定，HomeSpan 将在名为“状态”的页面上提供网络日志
 
补充说明：
 
  * 可以在你的草图中包含 WEBLOG() 消息，即使网络日志*未*启用。在这种情况下，HomeSpan 不会提供网络日志页面，但如果 *Log Level* 设置为 1 或更高，WEBLOG() 消息仍将输出到 Arduino 串口监视器
  * 消息**不**存储在 NVS 中，因此在重新启动之间**不**保存
 
请参阅 [示例 19 - WebLog](Tutorials.md#示例-19---网页日志) 以获得演示使用 `homeSpan.enableWebLog()` 和 WEBLOG() 宏的教程草图。

## 自定义样式表（CSS）

HomeSpan 的网络日志通常由浅蓝色背景上的黑色文本组成。但是，你可以设置自定义样式表（CSS） 以通过调用`homeSpan.setWebLogCSS(const char *css)` 来更改格式，其中 css 是使用包含一个或多个自定义样式元素的  [HTML 类](https://www.w3schools.com/html/html_classes.asp) 构造的。HomeSpan 为网络日志的不同部分实现以下三个类名：
 
 * *bod1* - 此类指定网络日志页面主体的样式元素，包括背景颜色和顶部的标题文本（其本身的格式为 \<h2\>）
 * *tab1* - 此类指定网络日志页顶部状态表的样式元素
 * *tab2* - 此类指定网络日志页面的 botom 中的日志条目表的样式元素
 
例如，以下 CSS 将网络日志页面的背景颜色更改为浅黄色，将标题文本的颜色更改为蓝色，将顶部表格中单元格的颜色更改为浅绿色，将 botom 表中的单元格颜色更改为浅蓝色。它还将第二个表的标题行 (\<th\>) 中的文本颜色更改为红色，将第二个表中数据行 (\<th\>) 的颜色更改为深蓝色，并将数据行中的文本对齐方式更改为在每个表单元格中居中：
 
 ```C++
 homeSpan.setWebLogCSS(".bod1 {background-color:lightyellow;}"
                       ".bod1 h2 {color:blue;}"
                       ".tab1 {background-color:lightgreen;}"
                       ".tab2 {background-color:lightblue;} .tab2 th {color:red;} .tab2 td {color:darkblue; text-align:center;}"
                       );
 ```
 
请注意，每当日志级别设置为 1 或更高时，HomeSpan 都会将网络日志 HTML 的全部内容（包括你在上面指定的任何 CSS）输出到串口监视器。在创建自己的 CSS 时，查看此输出会很有帮助。

## 添加用户定义的数据和/或自定义 HTML

Homespan 提供了一个用于生成网络日志的文本钩子，你可以扩展该钩子以将自己的数据添加到初始表中，以及更普遍地添加任何自定义 HTML。

若要访问此文本，请使用 `homeSpan.setWebLogCallback(void (*func)(String &htmlText))` 设置网络日志回调：

  * *func* 是 void 类型的函数，它采用 String 类型的单个参数，并且
  * homeSpan 将 *htmlText* 设置为一个字符串引用，其中包含网络日志已生成的所有 HTML 文本，以生成初始表。

要将自己的数据添加到表中，只需根据需要添加任意数量的 `<tr>` 和 `<td>`  HTML 标记来扩展字符串 *htmlText*。如果你希望结束表格并添加任何其他 HTML，只需在 *htmlText* 中包含标记 `</table>`，然后添加任何其他自定义 HTML。例如，可以使用以下函数扩展初始网络日志表以显示可用 DRAM、结束表并提供指向 HomeSpan 存储库的热链接：

```C++
void extraData(String &r){
  r+="<tr><td>Free DRAM:</td><td>" + String(esp_get_free_internal_heap_size()) + " bytes</td></tr>\n"; 
  r+="</table><p><a href=\"https://github.com/HomeSpan/HomeSpan\">Click Here to Access HomeSpan Repo</a></p>";
}
```

若要在网络日志中嵌入此自定义 HTML 文本，请调用草图的`homeSpan.setWebLogCallback(extraData)`。

## 从草图中访问网络日志 HTML 文本

除了让 HomeSpan 提供 HTML网络日志页面以响应 HTTP 请求之外，用户还可以从其草图中直接访问网络日志页面的 HTML 文本，以便进行自定义操作和处理。由于网络日志页面的 HTML 可能非常大，因此 HomeSpan 仅在请求页面时生成网络日志页面的 HTML，并以 1024 字节的顺序块流式传输 HTML 以响应网络日志 HTTP 请求。因此，HomeSpan 不可能简单地为用户提供指向完整网络日志的 HTML 文本的 `char *` 指针。相反，HomeSpan 为用户提供了以下 homeSpan 方法来触发网络日志页的生成，并在需要时访问生成的 HTML 文本：

`getWebLog(void (*f)(const char *htmlText, void *data), void *userData)`

 * *f()* - 一个用户定义的函数，返回 `void` 并接受两个参数：
   * *htmlText* - 指向 HomeSpan 提供的 HTML 文本块（最大 1024 字节）的以空 结尾的 `const char *` 指针
   * *data* - 指向任何用户提供的数据 *userData* 的`void *`指针
 * *userData* - 指向任何可选用户提供的数据的 `void *` 指针，该数据作为其第二个参数 data 传递给 *f()*

当从草图调用上述方法时，HomeSpan 将重复调用用户定义的函数 *f()*，并为网络日志页面提供连续的 HTML 文本块作为第一个参数 *htmlText*。一旦所有 HTML 块都发送到函数 *f()*，HomeSpan 将最后一次调用 *f()*，并将 *htmlText* 设置为空，以指示不再发送 HTML 块。

此函数的主要目的是让用户提供自己的 HTML网络日志页服务方法，例如通过安全的 HTTPS 通道。请注意，此通道可以是 HomeSpan 通过 HTTP 请求正常提供网络日志页面的补充，也可以代替 HomeSpan 的正常服务，具体取决于 `homeSpan.enableWebLog()` 方法中使用的 URL 参数是否设置为空（禁用 HomeSpan 提供网络日志页面以响应 HTTP 请求）。

以下 psuedo-code 代码片段显示了如何使用 `getWebLog()` ：

```C++
...
homeSpan.enableWebLog(50,"pool.ntp.org","UTC",NULL);      // 这会启用 50 个条目的网络日志并设置时钟，但会阻止 HomeSpan 响应网络日志页面的任何 HTTP 请求
...
IF WEBLOG NEEDED THEN{
  homeSpan.getWebLog(myWebLogHandler,NULL);               // 这会触发 HomeSpan 生成网络日志页面的 HTML 文本并将数据流式传输到 myWebLogHandler，无需任何额外的用户数据
}
...
void myWebLogHandler(const char *htmlText, void *args){   // 这是用户定义的网络日志处理程序（请注意，本示例中未使用可选的 *arg 参数）
  if(htmlText!=NULL){
    DO SOMETHING WITH htmlText (e.g. transmit it to the user via an HTTPS connection)
  }
  else
    PERFORM ANY CLEAN-UP PROCESSING (e.g. close the HTTPS connection)
  }
}
```
 
---

[↩️](../README.md#resources) 返回欢迎页面
