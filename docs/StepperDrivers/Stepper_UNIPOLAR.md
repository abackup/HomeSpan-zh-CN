# Stepper_UNIPOLAR

此类提供通用驱动器，可与任何中心抽头式单极步进电机配合使用。需要使用驱动器板，该驱动器板可将 ESP32 上 4 个引脚的低压/低电流数字信号转换为高压/高电流输出，适合直接连接到步进电机中每个线圈（*1* 和 *2*）的两个相（*A* 和 *B*）。

**Stepper_UNIPOLAR** 类包含以下构造函数：
* `Stepper_UNIPOLAR(int coil1A, int coil1B, int coil2A, int coil2B)`
  * 使用 ESP32 的 4 个数字引脚控制驱动器板，其中参数指定引脚编号
  * 驱动器电路应连接并配置为当上述任何 ESP32 引脚设置为高电平时，电流流过相应的线圈/相。同样，当引脚设置为低电平时，驱动器电路应停止电流流过相应的线圈/相。
  * 支持的模式如下：

    * FULL_STEP_ONE_PHASE
    * FULL_STEP_TWO_PHASE
    * HALF_STEP

❗注意：此类不支持短制动状态。调用 `brake()` 方法以及将 `move()` 和 `moveTo()` 方法中的 *endAction* 参数设置为 **StepperControl::BRAKE** 对电机驱动器没有影响。<br><br>

> [!WARNING]
> **请注意构造函数参数的顺序！** 前两个参数指定控制流过***线圈 1*** 的 *A* 和 *B* 相电流的 ESP32 引脚；后两个参数用于***线圈 2*** 的 *A* 和 *B* 相。

只要前两个参数用于其中一个线圈，后两个参数用于另一个线圈，哪个线圈定义为 *1* 或 *2*，哪一侧称为 *A* 或 *B* 都无关紧要。您会知道您是否错误指定了引脚的顺序，因为电机将来回振动，而不是顺时针或逆时针转动。

## 技术细节

此类将指定引脚设置为 HIGH 和 LOW 的模式取决于所选的*步进模式*，如下所示：

#### FULL_STEP_ONE_PHASE

* 4 步循环，其中每一步电流仅流过**一个**线圈的**一个**相

<table>
<tr><th></th><th colspan="2">阶段 A</th><th colspan="2">阶段 B</th></tr>
<tr><th></th><th>线圈 1</th><th>线圈 2</th><th>线圈 1</th><th>线圈 2</th></th></tr>
<tr><th>步骤 1</th><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td><td align="center">-</td></td></tr>
<tr><th>步骤 2</th><td align="center">-</td><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td></td></tr>
<tr><th>步骤 3</th><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td><td align="center">-</td></td></tr>
<tr><th>步骤 4</th><td align="center">-</td><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td></td></tr>
</table>
<br>

#### FULL_STEP_TWO_PHASE

* 4 步循环，每一步电流流过**每个**线圈的**一个**相

<table>
<tr><th></th><th colspan="2">阶段 A</th><th colspan="2">阶段 B</th></tr>
<tr><th></th><th>线圈 1</th><th>线圈 2</th><th>线圈 1</th><th>线圈 2</th></th></tr>
<tr><th>步骤 1</th><td align="center">HIGH</td><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td></td></tr>
<tr><th>步骤 2</th><td align="center">-</td><td align="center">HIGH</td><td align="center">HIGH</td><td align="center">-</td></td></tr>
<tr><th>步骤 3</th><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td><td align="center">HIGH</td></td></tr>
<tr><th>步骤 4</th><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td></td></tr>
</table>
<br>

#### HALF_STEP

* 通过将 FULL_STEP_ONE_PHASE 模式的 4 个步骤与 FULL_STEP_TWO_PHASE 模式的 4 个步骤交错形成的 8 步循环

<table>
<tr><th></th><th colspan="2">阶段 A</th><th colspan="2">阶段 B</th></tr>
<tr><th></th><th>线圈 1</th><th>线圈 2</th><th>线圈 1</th><th>线圈 2</th></th></tr>
<tr><th>步骤 1</th><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td><td align="center">-</td></td></tr>
<tr><th>步骤 2</th><td align="center">HIGH</td><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td></td></tr>
<tr><th>步骤 3</th><td align="center">-</td><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td></td></tr>
<tr><th>步骤 4</th><td align="center">-</td><td align="center">HIGH</td><td align="center">HIGH</td><td align="center">-</td></td></tr>
<tr><th>Step 5</th><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td><td align="center">-</td></td></tr>
<tr><th>Step 6</th><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td><td align="center">HIGH</td></td></tr>
<tr><th>Step 7</th><td align="center">-</td><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td></td></tr>
<tr><th>Step 8</th><td align="center">HIGH</td><td align="center">-</td><td align="center">-</td><td align="center">HIGH</td></td></tr> 
</table>

---

[↩️](../Stepper.md) 返回步进电机控制页面
