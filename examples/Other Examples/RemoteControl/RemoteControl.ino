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

#include "HomeSpan.h"             // 包含 HomeSpan 库

void setup() {     
 
  Serial.begin(115200);           // 启动串口
  Serial.flush();
  delay(1000);                    // 等待接口刷新

  Serial.print("\n\nHomeSpan RF Transmitter Example\n\n");

  RFControl rf(13);               // 创建 RFControl 实例，并将信号输出至 ESP32 的第 13 引
  rf.clear();                     // 清除脉冲序列内存缓冲区

  rf.add(5000,5000);              // 创建具有三个 5000 刻高/低脉冲的脉冲序列
  rf.add(5000,5000);
  rf.add(5000,10000);             // 最后低点周期的两倍持续时间

  Serial.print("Starting 4 cycles of three 500 ms on pulses...");
  
  rf.start(4,100);                // 开始传输 4 个周期的脉冲序列，1 个时钟周期 = 100 微秒

  Serial.print("Done!\n");

  delay(2000);

  rf.clear();

  for(int i=1000;i<10000;i+=1000)
    rf.add(i,10000-i);
  rf.add(10000,10000);
  
  Serial.print("Starting 3 cycles of 100-1000 ms pulses...");
  
  rf.start(3,100);                // 开始传输 3 个周期的脉冲序列，1 个时钟周期 = 100 微秒

  Serial.print("Done!\n");
  
  Serial.print("\nEnd Example");
  
} // setup() 结束

void loop(){

} // loop() 结束
