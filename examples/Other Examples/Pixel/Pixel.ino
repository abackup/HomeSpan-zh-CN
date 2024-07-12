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
 
// HomeSpan 可寻址 RGB LED 示例。演示了以下内容的使用：
//
// * HomeSpan 像素类，用于控制单线可寻址 RGB 和 RGBW LED，例如 WS2812 和 SK6812
// * HomeSpan 点类，用于控制双线可寻址 RGB LED，例如 APA102 和 SK9822
//
// 重要提示：您可能需要更改下面的引脚号以匹配您的特定 ESP32/S2/C3 板
//

#if defined(CONFIG_IDF_TARGET_ESP32)

  #define NEOPIXEL_RGB_PIN       26
  #define NEOPIXEL_RGBW_PIN      32
  #define DOTSTAR_DATA_PIN       33
  #define DOTSTAR_CLOCK_PIN      27
  #define DEVICE_SUFFIX          ""

#elif defined(CONFIG_IDF_TARGET_ESP32S2)

  #define NEOPIXEL_RGB_PIN       17
  #define NEOPIXEL_RGBW_PIN      38
  #define DOTSTAR_DATA_PIN       3
  #define DOTSTAR_CLOCK_PIN      7
  #define DEVICE_SUFFIX          "-S2"

#elif defined(CONFIG_IDF_TARGET_ESP32C3)

  #define NEOPIXEL_RGB_PIN       0
  #define NEOPIXEL_RGBW_PIN      3
  #define DOTSTAR_DATA_PIN       7
  #define DOTSTAR_CLOCK_PIN      2

  #define DEVICE_SUFFIX          "-C3"

#endif
 
#include "HomeSpan.h"

///////////////////////////////

struct NeoPixel_RGB : Service::LightBulb {      // 可寻址单线 RGB LED 灯串（例如 NeoPixel）
 
  Characteristic::On power{0,true};
  Characteristic::Hue H{0,true};
  Characteristic::Saturation S{0,true};
  Characteristic::Brightness V{100,true};
  Pixel *pixel;
  int nPixels;
  
  NeoPixel_RGB(uint8_t pin, int nPixels) : Service::LightBulb(){

    V.setRange(5,100,1);                      // 将亮度范围设置为最小 5% 到最大 100%，步长为 1%
    pixel=new Pixel(pin);                     // 在指定引脚上创建像素 LED
    this->nPixels=nPixels;                    // 保存此 LED 灯串中的像素数
    update();                                 // 手动调用 update() 来设置像素以恢复初始值
  }

  boolean update() override {

    int p=power.getNewVal();
    
    float h=H.getNewVal<float>();       // 范围 = [0,360]
    float s=S.getNewVal<float>();       // 范围 =  [0,100]
    float v=V.getNewVal<float>();       // 范围 =  [0,100]

    Pixel::Color color;

    pixel->set(color.HSV(h*p, s*p, v*p),nPixels);       // 将所有 nPixel 设置为相同的 HSV 颜色
          
    return(true);  
  }
};

///////////////////////////////

struct NeoPixel_RGBW : Service::LightBulb {      // 可寻址单线 RGBW LED 灯串（例如 NeoPixel）
 
  Characteristic::On power{0,true};
  Characteristic::Brightness V{100,true};
  Characteristic::ColorTemperature T{140,true};
  Pixel *pixel;
  int nPixels;
  
  NeoPixel_RGBW(uint8_t pin, int nPixels) : Service::LightBulb(){

    V.setRange(5,100,1);                      // 将亮度范围设置为最小 5% 到最大 100%，步长为 1%
    pixel=new Pixel(pin,true);                // 在指定引脚上创建像素 RGBW LED（第二个参数设置为 RGBW 的 true）
    this->nPixels=nPixels;                    // 保存此 LED 灯串中的像素数
    update();                                 // 手动调用 update() 来设置像素以恢复初始值
  }

  boolean update() override {

    int p=power.getNewVal();
    
    float v=V.getNewVal<float>();       // 范围 = [0,100]
    float t=T.getNewVal<float>();       // 范围 = [140,500]（140=最冷，500=最热）

    float hue=240-(t-140)/3;            // 在蓝色和绿色之间添加一抹色彩来模拟色温的变化

    // Pixel::Color color; // 如果使用静态 HSV 方法（如下），则无需先创建 Color 对象

    pixel->set(pixel->HSV(hue, 100, v*p, v*p),nPixels);      // 将所有 nPixels 设置为相同的 HSV 颜色（注意使用静态方法 pixel->HSV，而不是定义和设置 Pixel::Color）
          
    return(true);  
  }
};

///////////////////////////////

struct DotStar_RGB : Service::LightBulb {      // 可寻址双线 RGB LED 灯串（例如 DotStar）
 
  Characteristic::On power{0,true};
  Characteristic::Hue H{0,true};
  Characteristic::Saturation S{0,true};
  Characteristic::Brightness V{100,true};
  Dot *pixel;
  int nPixels;
  
  DotStar_RGB(uint8_t dataPin, uint8_t clockPin, int nPixels) : Service::LightBulb(){

    V.setRange(5,100,1);                      // 将亮度范围设置为最小 5% 到最大 100%，步长为 1%
    pixel=new Dot(dataPin,clockPin);          // 在指定引脚上创建点 LED
    this->nPixels=nPixels;                    // 保存此 LED 灯串中的像素数
    update();                                 // 手动调用 update() 来设置像素以恢复初始值
    update();                                 // 第二次调用第二个 update() - DotStar 似乎需要在启动时“刷新”
  }

  boolean update() override {

    int p=power.getNewVal();
    
    float h=H.getNewVal<float>();       // 范围 = [0,360]
    float s=S.getNewVal<float>();       // 范围 = [0,100]
    float v=V.getNewVal<float>();       // 范围 = [0,100]

    Dot::Color color[nPixels];          // 创建一个颜色数组

    float hueStep=360.0/nPixels;        // 色调从一个像素到下一个像素变化的步长

    for(int i=0;i<nPixels;i++)
      color[i].HSV(h+i*hueStep,s,100,v*p);   // 从指定的色调开始创建所有色调的光谱；使用限流参数（第 4 个参数）来控制整体亮度，而不是 PWM
      
    pixel->set(color,nPixels);          // 根据数组设置颜色
          
    return(true);  
  }
};

///////////////////////////////

void setup() {
  
  Serial.begin(115200);
 
  homeSpan.begin(Category::Lighting,"Pixel LEDS" DEVICE_SUFFIX);

  SPAN_ACCESSORY();                                             // 创建 Bridge（请注意，此草图使用了 v1.5.1 中引入的 SPAN_ACCESSORY() 宏——有关此便捷宏的详细信息，请参阅 HomeSpan API 参考）

  SPAN_ACCESSORY("Neo RGB");
    new NeoPixel_RGB(NEOPIXEL_RGB_PIN,8);                       // 创建具有全色彩控制的 8-LED NeoPixel RGB 灯串

  SPAN_ACCESSORY("Neo RGBW");
    new NeoPixel_RGBW(NEOPIXEL_RGBW_PIN,60);                    // 创建具有模拟色温控制的 60-LED NeoPixel RGBW 灯串 

  SPAN_ACCESSORY("Dot RGB");
    new DotStar_RGB(DOTSTAR_DATA_PIN,DOTSTAR_CLOCK_PIN,30);     // 创建 30-LED DotStar RGB 灯串，显示色谱，并使用 DotStars 的限流功能实现无闪烁调光

}

///////////////////////////////

void loop() {
  homeSpan.poll();
}

///////////////////////////////
