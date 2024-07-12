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

/////////////////////// 彩灯测试仪 //////////////////////////

// 该草图旨在帮助识别 NeoPixel、NeoPixel Strip 或任何包含一个或多个单线可寻址 RGB 或 RGBW LED 的设备（“像素设备”）的正确设置。

// 编译之前，将 PIXEL_PIN 设置为连接到像素设备的 ESP32 引脚，并将 NPIXELS 设置为像素设备中的像素数量。请注意，对于某些灯带，
// 单个芯片控制多个 LED，在这种情况下，应将 NPIXELS 设置为控制芯片的数量，而不是 LED 的数量。

// 首先，下面 testPixel 对象的 Pixel 构造函数的第二个参数应保持设置为 PixelType::RGBW

// 运行时，草图将重复循环颜色，方法是先将设备中的所有像素设置为红色，然后是绿色，然后是蓝色，最后是白色。短暂暂停后，循环重复。

// 对于每种颜色，亮度都会从 0 增加到 MAX_BRIGHTNESS，然后再回到 0。如果您想限制像素的亮度，可以将 MAX_BRIGHTNESS 更改为低于 255 的值。

// 对于具有多个像素的像素设备，诊断如下：
//
// * 如果所有 4 种颜色按照预期的顺序重复闪烁，则表示 PixelType::RGBW 的基本设置是正确的！
//
// * 如果不是将每个像素设置为相同的颜色，而是条带中的像素分别以不同的颜色点亮（或根本没有颜色），则意味着您拥有的是 RGB LED，
// 而不是 RGBW LED。将构造函数的第二个参数更改为 PixelType::RGB 并重新运行草图。
//
// * 如果所有像素都设置为相同的颜色，但顺序不是红色、绿色、蓝色，则更改构造函数的第二个参数，以使 PixelType 颜色的顺序与像素设
// 备上显示的颜色顺序相匹配。例如，如果您的 RGBW 像素设备闪烁绿色、红色、蓝色，然后是白色，请使用 PixelType::GRBW。

// 对于仅有一个像素的像素设备，诊断如下：

// * 如果所有 4 种颜色按照预期的顺序重复闪烁，则表示 PixelType::RGBW 的基本设置是正确的！
//
// * 如果像素设置为白色时根本不亮，则意味着您有一个 RGB LED，而不是 RGBW LED。将构造函数的第二个参数更改为 PixelType::RGB 并重新运行草图。
//
// * 如果所有像素都设置为相同的颜色，但顺序不是红色、绿色、蓝色，则更改构造函数的第二个参数，以使 PixelType 颜色的顺序
// 与 Pixel Device 上显示的颜色顺序相匹配。例如，如果您的 RGB Pixel Device 闪烁绿色、红色，然后闪烁蓝色，请使用 PixelType::GRB。

//////////////////////////////////////

#include "HomeSpan.h"

//////////////////////////////////////

#define MAX_BRIGHTNESS  255           // RGBW闪烁时最大亮度[0-255]

#define PIXEL_PIN 26                  // 将其设置为您正在使用的任何密码 - 注意密码不能是“仅输入”
#define NPIXELS   8                   // 设置为条带中的像素数

Pixel testPixel(PIXEL_PIN, PixelType::RGBW);      // 改变第二个参数直到设备以正确的颜色运行

//////////////////////////////////////

void setup() {
 
  Serial.begin(115200);
  delay(1000);

  Serial.printf("\n\nPixel Test on pin %d with %d pixels\n\n",PIXEL_PIN,NPIXELS);
}

//////////////////////////////////////

void flashColor(boolean r, boolean g, boolean b, boolean w){
  
  for(int i=0;i<MAX_BRIGHTNESS;i++){
    testPixel.set(Pixel::RGB(i*r,i*g,i*b,i*w),NPIXELS);
    delay(4);
  }
    
  for(int i=MAX_BRIGHTNESS;i>=0;i--){
    testPixel.set(Pixel::RGB(i*r,i*g,i*b,i*w),NPIXELS);
    delay(4);
  }
}

//////////////////////////////////////

void loop(){

  Serial.printf("Red...");
  flashColor(1,0,0,0);

  Serial.printf("Green...");
  flashColor(0,1,0,0);

  Serial.printf("Blue...");
  flashColor(0,0,1,0);

  if(testPixel.isRGBW()){
    Serial.printf("White...");
    flashColor(0,0,0,1);
  }

  Serial.printf("Pausing.\n");
  delay(1000);
}

//////////////////////////////////////
