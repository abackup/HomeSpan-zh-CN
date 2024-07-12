<!-- 原文时间：2023.3.19，翻译时间：2024.5.6，校对时间：2024.7.12 -->

# HomeSpan 配件类别

每个 HomeSpan 设备都必须分配一个 HomeKit 配件类别。HomeSpan 将这些类别实现为 C++ 类，其名称与苹果公司在 [HAP-R2](../master/docs/HAP-R2.pdf) 的第 13 节中指定的拼写和大小写完全匹配，但没有任何空格。HomeSpan 配件类别在 HomeSpan 的 `Category` 命名空间中定义。例如，HomeSpan 将**车库门开启器**类别（HAP 类别 4）定义为 `Category::GarageDoorOpeners`，可以在初始化 HomeSpan 时使用，如下所示：

```C++
homeSpan.begin(Category::GarageDoorOpeners,"Acme Garage Door Lifts");
```

下表提供了所有 HomeSpan 类别的列表。

### 配件类别
|序号|类别|中文|
|:-:|--------------------------- | :------------------------------: |
| 1|AirConditioners | 空调 |
| 2|AirPurifiers | 空气净化器 |
| 3|Bridges |桥接|
| 4|Dehumidifiers |除湿机|
| 5|Doors |门|
| 6|Fans |风扇|
| 7|Faucets |水龙头|
| 8|Lighting |灯|
| 9|Locks| 锁|
| 10|GarageDoorOpeners |车库门开启器|
| 11|Heaters |加热器|
| 12|Humidifiers |加湿器|
| 13|IPCameras |网络摄像机|
| 14|Other |其他|
| 15|Outlets |插座|
| 16|ProgrammableSwitches |可编程开关|
| 17|SecuritySystems |安全系统|
| 18|Sensors |传感器|
| 19|ShowerSystems| 淋浴系统|
| 20|Sprinklers |洒水器|
| 21|Switches |开关|
| 22|Thermostats |恒温器|
| 23|VideoDoorbells| 可视门铃|
| 24|Windows |窗户|
| 25|WindowCoverings |窗帘|
| 26|Television |电视|

请注意，HomeKit 主要使用设备的配件类别来确定在设备配对时在“家庭”应用中显示的图块。除了这个纯粹的装饰功能外，分配给设备的类别不会以任何方式限制可以在该设备上实现的服务或特性。

---

[↩️](../README.md#resources) 返回欢迎页面
