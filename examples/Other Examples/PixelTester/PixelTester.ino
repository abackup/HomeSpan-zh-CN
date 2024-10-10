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

// 此草图旨在帮助确定用于 NeoPixel、NeoPixel Strip 或任何包含一个或多个单线可寻址 RGB 型 LED 的设备的正确设置。

// 说明：运行草图并按照屏幕上的说明进行操作。

//////////////////////////////////////

#include "HomeSpan.h"

#define MAX_BRIGHTNESS  255     // lower this value (max=255) if pixels LEDs are too bright to look at when perfoming this test

int pin=-1;
int nPixels=0;

Pixel::Color colors[5]={
  Pixel::RGB(MAX_BRIGHTNESS,0,0,0,0),
  Pixel::RGB(0,MAX_BRIGHTNESS,0,0,0),
  Pixel::RGB(0,0,MAX_BRIGHTNESS,0,0),
  Pixel::RGB(0,0,0,MAX_BRIGHTNESS,0),
  Pixel::RGB(0,0,0,0,MAX_BRIGHTNESS)
};

Pixel *testPixel;

//////////////////////////////////////

char *getSerial(){
  static char buf[9];
  strcpy(buf,"");
  return(Utils::readSerial(buf,8));
}

//////////////////////////////////////

void setup() {
   
  Serial.begin(115200);
  delay(1000);
  
  Serial.printf("\n\n*************** PIXEL TESTER **********************\n\n");
  Serial.printf("This sketch helps you identity your Pixel Type\n\n");

  while(pin<0){
    Serial.printf("Enter PIN number to which NeoPixel is connected: ");
    sscanf(getSerial(),"%d",&pin);
    if(pin<0)
      Serial.printf("(invalid entry)\n");
    else
      Serial.printf("%d\n",pin);    
  }

  testPixel=new Pixel(pin,"01234");

  while(nPixels<=0){
    Serial.printf("Enter number of PIXELS in NeoPixel device: ");
    sscanf(getSerial(),"%d",&nPixels);
    if(nPixels<=0)
      Serial.printf("(invalid entry)\n");
    else
      Serial.printf("%d\n",nPixels);    
  }

  Serial.printf("\nFor each test below, specify COLORS shown using the following characters:\n\n");
  if(nPixels==1){
    Serial.printf("  'R' = Red\n");
    Serial.printf("  'G' = Green\n");
    Serial.printf("  'B' = Blue\n");
    Serial.printf("  'W' = White (or Warm-White)\n");
    Serial.printf("  'C' = Cool White\n");
    Serial.printf("  '-' = Pixel is NOT lit\n");
  }
  else{
    Serial.printf("  'R' = FIRST Pixel is Red\n");
    Serial.printf("  'G' = FIRST Pixel is Green\n");
    Serial.printf("  'B' = FIRST Pixel is Blue\n");
    Serial.printf("  'W' = FIRST Pixel is White (or Warm-White)\n");
    Serial.printf("  'C' = FIRST Pixel is Cool White\n");
    Serial.printf("  '-' = neither FIRST nor SECOND Pixel is lit\n");
    Serial.printf("  'X' = FIRST Pixel is not lit, but SECOND Pixel is lit (any color)\n");
  }
  Serial.printf("\nNote: entries are case-insensitive.\n\n");

  char pType[6]="";
  
  for(int i=0;i<5;i++){
    testPixel->set(colors[i]);
    while(strlen(pType)==i){
      Serial.printf("Test #%d - enter COLOR: ",i+1);
      if(nPixels==1)
        sscanf(getSerial(),"%1[RGBWCrgbwc-]",pType+i);
      else
        sscanf(getSerial(),"%1[RGBWCrgbwcxX-]",pType+i);
      if(strlen(pType)==i)
        Serial.printf("(invalid entry)\n");
      else{
        pType[i]=toupper(pType[i]);
        Serial.printf("'%s'\n",pType+i);
      }
    }
    if(pType[i]=='X')
      break;
  }

  while(strlen(pType)>3 && ((pType[strlen(pType)-1]=='-' && nPixels==1) || pType[strlen(pType)-1]=='X'))
      pType[strlen(pType)-1]='\0';
      
  Serial.printf("\nTest Concluded.  Best match for your Pixel Type is '%s'\n\n",pType);
  testPixel=new Pixel(pin,pType);
  testPixel->set(Pixel::RGB(0,0,0,0,0),nPixels);
  Serial.printf("Hit ENTER to verify with flashing test\n\n");
  getSerial();

}

//////////////////////////////////////

void loop(){
  
  char c[]="RGBWC";

  for(int i=0;i<5;i++){
    if(testPixel->hasColor(c[i])){
      Serial.printf("Color '%c'...",c[i]);
      
      for(int v=0;v<MAX_BRIGHTNESS;v++){
        testPixel->set(Pixel::RGB(i==0?v:0,i==1?v:0,i==2?v:0,i==3?v:0,i==4?v:0),nPixels);
        delay(4*255/MAX_BRIGHTNESS);
      }
        
      for(int v=MAX_BRIGHTNESS;v>=0;v--){
        testPixel->set(Pixel::RGB(i==0?v:0,i==1?v:0,i==2?v:0,i==3?v:0,i==4?v:0),nPixels);
        delay(4*255/MAX_BRIGHTNESS);
      }
    }
  }
  testPixel->set(Pixel::RGB(0,0,0,0,0),nPixels);
  Serial.printf("Done.\n");
  Serial.printf("Hit ENTER to repeat with flashing test, or type 'R' to restart program...\n");
  if(toupper(getSerial()[0])=='R')
    ESP.restart();
}

//////////////////////////////////////
