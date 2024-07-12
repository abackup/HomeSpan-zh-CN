<!--  原文时间：2023.6.27，翻译时间：2024.7.6，校对时间：2024.7.12  -->

# TLV8 特性

大多数 HomeKit 特性存储单个数值或简单字符串。但是，HomeKit 支持另外两种存储格式——简单的字节列表（**DATA** 格式）和结构化的标签和值列表（**TLV8** 格式）。任何 Apple 定义的特性都不使用 DATA 格式，但它包含在 HomeSpan 中，用于为非 Apple 应用程序创建自定义特性。

相比之下，HomeKit 在初始配对过程中以及在 HomeKit 和 HomeSpan 设备之间建立新的安全（已验证）连接时广泛使用 TLV8 格式。还有各种 Apple 定义的特性使用 TLV8 格式来存储和传输多组值，每组值都表示为任意长度的字节数组。

## TLV8 格式概述

TLV8 格式本身非常简单。TLV8 对象包含一个或多个 TLV8 *记录*，其中记录中的第一个字节表示标识标签（从 0 到 255），第二个字节表示值的长度，其余的长度字节表示值本身，它始终采用*字节数组*的形式（即 0 个或更多 *uint8_t* 元素的数组）。关于 TLV8 格式的值得注意的点如下：

* 由于长度仅存储为单个字节，因此需要超过 255 个字节的值必须表示为具有相同标记的连续 TLV8 记录
* TLV8 对象可以包含具有相同标记的多个记录（实际上很常见），但它们必须*由具有不同标记的记录分隔*，否则读取数据的解析器会将具有相同标记的连续记录中的值连接成单个记录（如上所述）
* 允许表示零长度值的记录，并且仅由两个字节组成：一个标记和一个零（表示零长度值）。具有零长度值的标签通常用于分隔具有相同标签的多个记录
* 如果该值的字节数组应该表示单个无符号整数，则应按小端格式排列（即最低有效字节在前），并根据需要用尾随零填充，以使该值的总长度为 1、2、4 或 8 个字节（表示 uint8_t、uint16_t、uint32_t 和 uint64_t 值）
* 如果该值的字节数组应该表示一个字符串，则它不应包含终止空值，因为长度会告诉您字符串中有多少个字符
* 组成一个值的字节本身可以表示一个单独的完整 TLV8 对象。可以递归嵌套在“父” TLV8 对象中的“子” TLV 数量没有限制
* 读取 TLV8 记录的解析器应默默忽略任何不期望的 TAG。忽略解析器所需的 TAG 可能是一个错误，但包含它无法识别的 TAG 则不是错误
* **不可能**明确地确定 TLV8 记录中的 VALUE 字节数组是否应该表示无符号整数、字符串、任意字节序列、“子” TLV 对象或其他完全不同的东西。任何给定 TLV8 记录的唯一识别信息是其 TAG 编号，范围为 0-255。没有将 TAG 类型映射到 VALUE 类型的通用架构或 TLV8 协议。相反，TAG 编号是任意的，用户必须查阅每个特性的文档，以了解预期的 TAG 编号以及它们的值应该代表该特定特性的什么
* 由于 HomeKit 数据传输通常以 JSON 表示，而 JSON 是一种纯文本格式，因此 HomeKit 要求在将 JSON 传输到控制器和从控制器传输到附件时，首先以 base-64 编码 TLV8 记录。

幸运的是，HomeSpan 包含一个专用的 TLV8 库（见下文），可以自动处理上述许多问题，使您能够读取、创建和处理 TLV8 数据，而不必担心解析超过 255 字节的 TLV8 记录、将数值转换为小端，或将记录编码/解码为 base-64。

## *TLV8()*

使用上述构造函数创建 HomeSpan 的 TLV8 **类**实例会构建一个空的 TLV8 对象，您可以在其中添加和处理 TLV8 记录。TLV8 对象被实例化为从 `std::list<tlv8_t>` 派生的标准 C++ 链接列表容器，其中 *tlv8_t* 是一个用于存储单个 TLV8 记录的不透明结构。[^opaque]

此外，如下所示，许多 TLV8 方法都使用链接列表*常量*迭代器。这些迭代器由 typedef *TLV8_itc* 表示。[^iterators]

[^opaque]:*tlv8_t* 结构是不透明的，因为通常您不必创建或直接与结构或其数据交互。请注意，除了上述特定于 TLV8 的方法之外，您还可以根据需要将任何 `std::list` 方法与 TLV8 对象一起使用。

[^iterators]:您不需要具备 C++ 容器和迭代器的专业知识即可使用 TLV8 库，但对容器和迭代器的基本了解将使该库更易于学习和使用。

T将通用 TLV8 记录添加到 TLV8 对象的方法如下：

* `TLV8_itc add(uint8_t tag, size_t len, const uint8_t *val)`

  * 其中 *tag* 是要添加的记录的 TAG 标识符，*val* 是指向包含 *len* 个元素的字节数组的指针
  * 示例：`TLV8 myTLV; uint8_t v[]={0x01, 0x05, 0xE3, 0x4C}; tlv.add(1, sizeof(v), v);`
  * 将 *val* 设置为 NULL 会为 TLV8 对象内的 TLV8 记录保留 *len* 个字节的空间，但不会复制任何数据
  * 此方法将 TLV8 常量迭代器返回到生成的 TLV8 记录，以便您可以在以后需要时引用该记录

除了上述适用于任何类型数据的通用方法之外，以下方法可以更轻松地添加具有特定、常用数据类型的 TLV8 记录：

* `TLV8_itc add(uint8_t tag, uintXX_t val)`
  * 添加包含单个无符号数值 *val*（即 uint8_t、uint16_t、uint32_t 或 uint64_t）的 TLV8 记录

* `TLV8_itc add(uint8_t tag, const char *val)`
  * 添加包含以空字符结尾的字符串的所有非空字节的 TLV8 记录， *val*

* `TLV8_itc add(uint8_t tag, TLV8 &subTLV)`
  * 添加包含整个 TLV8 对象 *subTLV* 的所有字节的 TLV8 记录

* `TLV8_itc add(uint8_t tag)`
  * 添加仅包含 TAG 标识符的零长度 TLV8 记录

请注意，如果您 *添加* 具有相同 TAG 标识符的连续记录，TLV8 库将连接它们的数据并合并为单个记录。例如，`myTLV.add(1,13); myTLV.add(1,300)` 将合并以生成单个 3 字节记录，其中包含数据 0x0D2C01，其中第一个字节代表数字 13，后两个字节代表数字 300。这可能是您想要的结果，但可能不是您想要的结果。

相反，要创建两个具有相同标记值的不同记录，只需在两者之间插入一个具有不同标记标识符的零长度记录作为“分隔符”，如下所示：`myTLV.add(1,13); myTLV.add(255); myTLV.add(1,300);` 这里我们使用 255 的标记标识符来表示分隔符，但这种选择是任意的，除非该标记恰好被特征用于其他用途（0 或 255 的标记标识符通常用作分隔符）。

在包含特定标记标识符的 TLV8 对象中查找 TLV8 记录的方法如下：

* `TLV8_itc find(uint8_t tag)`

  * 其中 *tag* 是您要查找的 TAG 标识符

  * 返回 TLV8 常量迭代器，指向匹配的*第一个*记录；如果没有匹配的记录，则返回 *end()*

要将搜索范围限制为有限的一组记录，请添加可选的起始和结束迭代器 *it1* 和 *it2*：

* `TLV8_itc find(uint8_t tag [, TLV8_itc it1 [, TLV8_itc it2]])`

  * 返回 TLV8 常量迭代器，指向从 *it1* 到 *it2* 的常量迭代器范围内与指定 *tag* 匹配的*第一个*记录

  * 搜索范围包括 *it1* 但不包括 *it2*

  * 如果没有匹配的记录，则返回 *it2*

  * 如果未指定 *it2*，则默认为 *end()*；如果未指定 *it1*，则默认为 *begin()*
  * 注意 `myTLV.find(tag)` 相当于 `myTLV.find(tag, myTLV.begin(), myTLV.end())`

使用 C++ `auto` 关键字通常是保存从 `find()` 和 `add()` 方法返回的 TVL8_itc 迭代器的最佳方式。例如，`auto myIT = myTLV.find(6)` 将 *myIT* 设置为指向 *myTLV* 中第一个 TAG 标识符为 6 的 TLV8 记录的常量迭代器。

查找存储在特定 TLV8 记录中的数据值的长度的方法如下：

* `int len(TLV8_itc it)`
  * 其中 *it* 是指向特定 TLV8 记录的常量迭代器
  * 返回存储在关联记录中的数据值的长度，对于零长度记录，该长度可能为零
  * 如果 *it* 指向 TLV8 对象的 *end()*，则返回 -1

`len()` 方法的典型用途是同时检查 TLV8 对象是否包含特定 TAG 标识符，以及 TAG 的长度是否与预期值匹配。例如，如果某个特性要求 TAG 标识符为 6 的 TLV8 记录包含 32 字节的注册号，则可以执行以下检查：

```C++
auto myIT = myTLV.find(6);
if(myTLV.len(myIT)!=32)
Serial.printf("Error: TAG 6 is either missing or of impor length\n");
else
Serial.printf("TAG 6 containing 32 bytes of data has been found\n");
```

将 TLV8 对象中的所有记录打印到串行监视器的方法如下：

* `void print()`

  * 将所有 TLV8 记录（每行一个）打印到串行监视器
  * 输出格式为：TAG(LENGTH) VALUE [NUMERIC]，其中
    * TAG = TAG 标识符（0-255）
    * LENGTH = VALUE 字节数组的长度（可能为零）
    * VALUE = VALUE 字节数组中所有字节的十六进制顺序列表（仅当 LENGTH>0 时显示）
    * NUMERIC = VALUE 中字节的无符号整数解释，假设为小端顺序
      * 仅当 LENGTH<=8 时才显示此十进制值
      * 如果 LENGTH=0，则显示单词“null”

要将打印范围限制为一组有限的记录，添加可选的起始和结束常量迭代器 *it1* 和 *it2*：

* `void print(TLV8_itc it1 [, TLV8_itc it2])`

  * 打印常量迭代器 *it1* 和 *it2* 之间的所有 TLV8 记录
  * 打印范围包括 *it1* 但不包括 *it2*
  * 如果未指定 *it2*，则仅打印 *it1* 指向的记录
  * 注意 `myTLV.print()` 等同于 `myTLV.print(myTLV.begin(), myTLV.end())`

`print()` 生成的输出可能包含一些非常长的行，特别是当某些 TLV8 记录的 VALUE 代表其他完整的 TLV8 对象（称为子 TLV 或“嵌套”TLV）时。要递归打印所有子 TLV 对象，请使用以下方法：

* `void printAll()`

  * 递归打印所有 TLV8 记录（每行一个）到串行监视器
  * 检查每个 TLV8 记录并尝试解析，就好像记录代表子 TLV 对象一样
    * 如果解析成功，则打印记录，然后在子 TLV 上调用 `printAll()`
    * 如果不成功，则打印记录并结束此递归分支
  * 每行的格式与 `print()` 的格式相同，只是 TAG 显示分支中所有 TAG 的完整路径
  * 请注意，如果您的 TLV8 对象包含多层嵌套子 TLV，则输出可能非常庞大
  * 警告：解释输出时需要小心[^subTLVs]

[^subTLVs]:`printAll()` 方法假定任何与子 TLV 格式一致的值都必须是子 TLV，即使它只是一个简单的数值。例如，`add(10,65536)` 生成一个 TAG 标识符为 10 且 4 字节 VALUE 为 0x00000100 的记录。`printAll()` 方法将与 NUMERIC=65536 一起显示此记录，但它随后还会将此 VALUE 解释（并因此显示）为子 TLV，其中包含一个零长度记录（TAG 标识符=0）和另一个零长度记录（TAG 标识符=1），因为 VALUE 可以成功解析为这样。

TLV8 对象管理其所有内部内存需求，并在超出范围或以其他方式被删除时释放所有资源和内存。但是，如果您需要“擦除” TLV8 对象的所有内容但仍保留该对象以便重新填充新数据，请使用以下方法：

* `void wipe()`
  * 擦除所有 TLV8 记录并释放所有相关内存
  * 留下一个空的 TLV8 对象以供重复使用
 
## *TLV8_itc()*  
  
*TLV8_itc* 类型的对象是指向 TLV8 对象中特定 *tlv8_t* 记录（或 *end()*）的常量迭代器。TLV8 迭代器用于访问、读取和写入任何给定 TLV8 记录中的数据元素，因此是 TLV8 库的重要组成部分。但是，如果您正确使用 TLV8 库，则很少（如果有的话）需要直接使用其构造函数实例化 *TLV8_itc*。相反，只需使用上面提到的 C++ `auto` 关键字即可。

可以使用以下方法取消引用 TLV8_itc 迭代器来处理单个 TLV8 记录中的数据：

* `uint8_t getTag()`

  * 返回 TLV8 记录的 TAG 标识符 (0-255)
  * 示例：`uint8_t tag = myIT->getTag()` 或等效地，`uint8_t tag = (*myIT).getTag()`

* `size_t getLen()`

  * 返回 TLV8 记录的 VALUE 字节数组的长度
  * 示例：`size_t len = myIT->getLen()` 或等效地，`size_t len = (*myIT).getLen()`

* `uint8_t *get()`

  * 返回指向 TLV8 记录的 VALUE 字节数组的第一个元素的 `uint8_t *`
  * 对于零长度的 TLV8 记录，返回值为 NULL
  *例如：`uint8_t *v = myIT->get();` 或者，等效地，`uint8_t *v = (*myIT).get();`
  * `(uint8_t *)` 转换运算符已重载，因此您也可以通过简单地取消引用迭代器来获取相同的 `uint8_t *` 指针
    * 例如：`auto myIT = myTLV.find(6); uint8_t *v = *myIT;`
    * 注意，这仅在编译器可以根据代码上下文确定是否需要自动转换为 `uint8_t *` 指针时才有效

* `uint8_t get()[i]`
  * 返回 VALUE 字节数组的第 *i 个元素
  * 示例：`uint8_t n = myIT->get()[i]` 或者，等效地，`uint8_t n = (*myIT).get()[i]`
  * 下标运算符也已重载，因此您只需取消引用迭代器即可获得第 *i 个元素
    * 示例：`uint8_t n = (*myIT)[i]`
  * 注意，没有范围检查，因此请确保 *i* 不会尝试引用 VALUE 字节数组末尾以外的元素

* `T getVal<class T>()`
  * 此模板函数返回一个类型为 *T* 的数值，前提是 VALUE 字节数组存储的是小端格式的无符号整数
  * *T* 可以是 *uint8_t*、*uint16_t*、*uint32_t* 或 *uint64_t*（如果未指定 *T*，则默认为 *uint32_t*）
  * 示例：`auto myIT = myTLV.add(50,60000); uint16_t n = myIT->getVal<uint16_t>();`
  * 只要 sizeof(*T*) >= 字节数组的长度，此方法就会返回正确的数值。例如：
    * 使用包含 2 个字节的 VALUE 字节数组设置 *T=uint64_t* 会返回 *正确* 的数值
    * 使用包含 4 个字节的 VALUE 字节数组设置 *T=uint16_t* 会返回 *不正确* 的数值
  * 此函数对所有零长度 TLV8 记录返回零

### 使用上述方法的详细示例

以下代码：

```C++
TLV8 myTLV;   // 实例化一个空的 TLV8 对象

myTLV.add(1,8700);                       // 添加 TAG=1 且 VALUE=8700 的 TLV8 记录
auto it_A = myTLV.add(2,180);           // 添加TAG=2、VALUE=180的TLV8记录，并保存返回的迭代器

uint8_t v[32];                           // 创建一个 32 字节数组 v，并用一些数据填充它
for(int i=0;i<32;i++)
  v[i]=i;
  
myTLV.add(200,32,v);                     // 添加 TAG=200 的 TLV8 记录，并将数组 v 的所有 32 个字节复制到其 VALUE 中

myTLV.add(50,60000);                     // 添加 TAG=50 和 VALUE=60000 的 TLV8 记录
myTLV.add(255);                          // 添加一个长度为零且 TAG=255 的 TLV8 记录作为分隔符
myTLV.add(50,120000);                    // 添加 TAG=50 且 VALUE=120000 的 TLV8 记录
myTLV.add(255);                           // 添加一个长度为零且 TAG=255 的 TLV8 记录作为分隔符
myTLV.add(50,180000);                    // add a TLV8 record with TAG=50 and VALUE=180000
myTLV.add(255);                           // 添加一个长度为零且 TAG=255 的 TLV8 记录作为分隔符
auto it_B = myTLV.add(50,240000);        // 添加TAG=50、VALUE=240000的TLV8记录，并保存返回的迭代器

auto it_C = myTLV.find(50);                  // 找到指向第一个 TAG=50 的 TLV8 记录的迭代器；
auto it_D = myTLV.find(50,std::next(it_C));   // 找到在 it_C 之后出现的第一个 TAG=50 的 TLV8 记录的迭代器；

auto it_E = myTLV.find(200);             // 找到第一个 TAG=200 的 TLV8 记录的迭代器；
Serial.printf("results of myTLV.print():\n\n");

myTLV.print();                          // 将 myTLV 的内容打印到串行监视器
Serial.printf("\n");

// 打印 it_A 的内容：

Serial.printf("it_A: TAG=%d, LENGTH=%d, Value=%d\n", it_A->getTag(), it_A->getLen(), it_A->getVal());

// 使用替代语法打印 it_B 的内容：

Serial.printf("it_B: TAG=%d, LENGTH=%d, Value=%d\n", (*it_B).getTag(), (*it_B).getLen(), (*it_B).getVal());

// 根据上面的 find() 打印 it_C 和 it_D 的内容：

Serial.printf("it_C TAG=%d, LENGTH=%d, Value=%d\n", (*it_C).getTag(), (*it_C).getLen(), (*it_C).getVal());
Serial.printf("it_D TAG=%d, LENGTH=%d, Value=%d\n", (*it_D).getTag(), (*it_D).getLen(), (*it_D).getVal());

// 您也可以直接使用 find() 的结果，而无需保存为单独的迭代器，尽管这在计算上效率低下：

if(myTLV.find(1)!=myTLV.end())     // 检查匹配
  Serial.printf("Found: TAG=%d, LENGTH=%d, Value=%d\n", myTLV.find(1)->getTag(), myTLV.find(1)->getLen(), myTLV.find(1)->getVal());

// 对 it_E 中的所有字节求和：

int sum=0;
for(int i=0; i < it_E->getLen(); i++)
  sum+= (*it_E)[i];

Serial.printf("it_E TAG=%d, LENGTH=%d, Sum of all bytes = %d\n", (*it_E).getTag(), (*it_E).getLen(), sum);

// 创建一个“空白”TLV8 记录，其 TAG=90 且空间为 16 字节：

auto it_F = myTLV.add(90,16,NULL);

// 将 it_E 的前 16 个字节复制到 it_F 中并打印记录：

memcpy(*it_F,*it_E,16);
myTLV.print(it_F);
```

产生以下输出：

```C++
results of myTLV.print():

1(2) FC21 [8700]
2(1) B4 [180]
200(32) 000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F
50(2) 60EA [60000]
255(0)  [null]
50(4) C0D40100 [120000]
255(0)  [null]
50(4) 20BF0200 [180000]
255(0)  [null]
50(4) 80A90300 [240000]

it_A: TAG=2, LENGTH=1, Value=180
it_B: TAG=50, LENGTH=4, Value=240000
it_C TAG=50, LENGTH=2, Value=60000
it_D TAG=50, LENGTH=4, Value=120000
Found: TAG=1, LENGTH=2, Value=8700
it_E TAG=200, LENGTH=32, Sum of all bytes = 496
90(16) 000102030405060708090A0B0C0D0E0F
```

## 读取和写入 TLV8 特性

如 [API 参考](Reference.md) 中完整记录的那样，以下 *SpanCharacteristic* 方法用于读取和写入 TLV8 对象到 TLV8 特性：

* `getTLV(TLV8 &tlv)`
* `getNewTLV(TLV8 &tlv)`
* `setTLV(TLV8 &tlv)`

这些方法类似于用于基于数值的特性的 `getVal()`、`getNewVal()` 和 `setVal()` 方法。

请注意，使用上述方法*不*需要您创建一个单独的字节数组来将记录拆分为 255 个字节的块，也不需要您将任何内容编码或解码为 base-64。相反，您可以直接将特征读取和写入 TLV8 对象。[^getString]

有关 TLV8 特征在实践中的详细示例，请参阅教程 [示例 22 - TLV8_特征](../examples/22-TLV8_Characteristics%20/22-TLV8_Characteristics.ino)，其中演示了如何使用 **DisplayOrder** TLV8 特征来设置电视服务输入源在“家庭”应用中的显示顺序。

[^getString]:由于 TLV8 特征存储为 base-64 编码字符串，因此如果您愿意，您可以始终使用 `getString()` 读取 base-64 文本，或使用 `getData()` 将字符串解码为代表整个 TLV8 对象的完整字节数组。此外，如果您确实不想使用 HomeSpan 的 TLV8 库来生成 TLV8 对象，而是更喜欢使用自己的方法来创建符合 TLV8 的字节数组，您可以这样做，然后使用 `setData()` 将您生成的字节数组保存到 TLV8 特性中，它将为您执行 base-64 编码。或者，如果您还想执行自己的 base-64 编码（为什么？），您可以这样做，然后只需使用 `setString()` 将生成的编码文本保存到 TLV8 特性中。

### 写入响应请求

对于大多数特性，当“家庭”应用向 HomeSpan 发送更新值的请求时，它会指示 HomeSpan 执行某种操作，例如“将灯泡的亮度更改为 30%”或“将门的目标状态更改为打开”。“家庭”应用期望收到的唯一响应此类请求的反馈基本上是 "OK" 或 "NOT OKAY"消息，这是每个服务的 `update()` 方法中布尔返回值的目的。

但是，有时“家庭”应用会向 HomeSpan 发送信息请求，而不是执行任务的直接指令。在这种情况下，“家庭”应用不会只发回 OK/NOT-OKAY 消息，而是期望配件设备使用“家庭”应用发送的新值而不是新值来更新特性的值。然后，它期望在更新结束时将此信息传回“家庭”应用。

此过程称为“写入响应请求”，它是拥有 TLV8 特性的主要目的，因为 TLV8 对象非常适合存储结构化信息。

虽然该过程很复杂，但 HomeSpan 处理了所有协议细节。您只需专注于读取 TLV8 特性并使用所需的 TLV8 响应对其进行更新，如下所示：

* 首先，从适用服务的 `update()` 循环中，检查“家庭”应用是否已请求更新 TLV8&nbsp;特性；
* 如果是，则创建一个新的 TLV8 对象并使用 `getNewTLV()` 将更新的特性的内容加载到该 TLV8 对象中；
* 然后，使用上面描述的 TLV8 库方法读取 TLV8 对象中的标签和值，以确定“家庭”应用正在传达哪些数据以及它希望返回哪些信息（基于特性的规范）；
* 接下来，创建 *第二个* TLV8 对象并使用上面的 TLV8 库方法创建响应信息请求所需的适当标签和值（同样，基于特性的规范）；
* 最后，使用 `setVal()` 用第二个 TLV8 对象更新 TLV8 特性

HomeSpan 将在 `update()` 循环结束时自动将您放置在 TLV8 特性中的新 TLV8 数据发送回“家庭”应用的响应中。

---

[↩️](../README.md#resources) 返回欢迎页面
