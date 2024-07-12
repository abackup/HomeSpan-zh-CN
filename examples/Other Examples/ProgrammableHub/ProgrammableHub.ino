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

//////////////////////////////////////////////////////////////////
//                                                              //
//       HomeSpan：ESP32 的 HomeKit 实现                         //
//       ------------------------------------------------       //
//                                                              //
//      演示如何与 HomeSpan 一起实现 Web 服务器，以创建可编程       //
//      集线器，最多可配置 12 个灯。允许动态更改附件，              //
//      而无需重新启动                                           //
//                                                              //
//////////////////////////////////////////////////////////////////

#include "HomeSpan.h"
#include <WebServer.h>                    // 包含 WebServer 库

WebServer webServer(80);                  // 在端口 80 上创建 WebServer
 
#define MAX_LIGHTS        12
#define MAX_NAME_LENGTH   32
#define HUB_NAME          "lighthub"

enum colorType_t : uint8_t {
  NO_COLOR,
  TEMPERATURE_ONLY,
  FULL_RGB
};

uint32_t aidStore=2;                     // 跟踪唯一的 AID 号码 - 从 AID=2 开始

struct lightData_t {
  char name[MAX_NAME_LENGTH+1]="";
  uint32_t aid=0;
  boolean isDimmable:1;
  colorType_t colorType:2;
} lightData[MAX_LIGHTS];

nvs_handle savedData;

//////////////////////////////////////

void setup() {

  Serial.begin(115200);

  size_t len;  
  nvs_open("SAVED_DATA",NVS_READWRITE,&savedData);       // 在 NVS 中打开一个名为 SAVED_DATA 的新命名空间
  
  if(!nvs_get_blob(savedData,"LIGHTDATA",NULL,&len))        // 如果找到 LIGHTDATA 数据
    nvs_get_blob(savedData,"LIGHTDATA",&lightData,&len);       // 检索数据

  nvs_get_u32(savedData,"AID",&aidStore);                   // 获取 AID（如果存在）

  homeSpan.setLogLevel(1);

  homeSpan.setHostNameSuffix("");         // 使用空字符串作为后缀（而不是 HomeSpan 设备 ID）
  homeSpan.setPortNum(1201);              // 更改 HomeSpan 的端口号，以便我们可以将端口 80 用于 Web 服务器
  homeSpan.setWifiCallback(setupWeb);     // 需要在 WiFi 建立后启动 Web 服务器

  homeSpan.begin(Category::Lighting,"HomeSpan Light Hub",HUB_NAME);

  new SpanAccessory(1);                                   // 为清楚起见，我们在此指定 AID=1（如果留空，则默认为 1）
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Model("HomeSpan Programmable Hub");
      new Characteristic::AccessoryFlags();

  for(int i=0;i<MAX_LIGHTS;i++){                         // 根据保存的数据创建灯光配件
    if(lightData[i].aid)
      addLight(i);
  }
    
  new SpanUserCommand('a',"<name> - add non-dimmable light accessory using name=<name>",[](const char *c){addLight(c+1,false,NO_COLOR);});
  new SpanUserCommand('A',"<name> - add dimmable light accessory using name=<name>",[](const char *c){addLight(c+1,true,NO_COLOR);});
  new SpanUserCommand('t',"<name> - add non-dimmable light accessory with color-temperature control using name=<name>",[](const char *c){addLight(c+1,false,TEMPERATURE_ONLY);});
  new SpanUserCommand('T',"<name> - add dimmable light accessory with color-temperature control using name=<name>",[](const char *c){addLight(c+1,true,TEMPERATURE_ONLY);});
  new SpanUserCommand('r',"<name> - add non-dimmable light accessory with full RGB color control using name=<name>",[](const char *c){addLight(c+1,false,FULL_RGB);});
  new SpanUserCommand('R',"<name> - add dimmable light accessory with full RGB color control using name=<name>",[](const char *c){addLight(c+1,true,FULL_RGB);});

  new SpanUserCommand('l'," - list all light accessories",listAccessories);
  new SpanUserCommand('d',"<index> - delete a light accessory with index=<index>",[](const char *buf){deleteAccessory(atoi(buf+1));});
  new SpanUserCommand('D'," - delete ALL light accessories",deleteAllAccessories);  
  new SpanUserCommand('u',"- update accessories database",updateAccessories);
 
} // setup() 结束

///////////////////////////

void loop(){
  homeSpan.poll();
  webServer.handleClient();           // 处理传入的 Web 服务器流量
}

///////////////////////////

void addLight(int index){
  
  Serial.printf("Adding Light Accessory:  Name='%s'  Dimmable=%s  Color=%s\n",
    lightData[index].name,lightData[index].isDimmable?"YES":"NO",lightData[index].colorType==NO_COLOR?"NONE":(lightData[index].colorType==TEMPERATURE_ONLY?"TEMPERATURE_ONLY":"FULL_RGB"));

  new SpanAccessory(lightData[index].aid);
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name(lightData[index].name);
      char sNum[32];
      sprintf(sNum,"Light-%02d",index);
      new Characteristic::SerialNumber(sNum);
      
    new Service::LightBulb();
      new Characteristic::On(0,true);
      if(lightData[index].isDimmable)
        new Characteristic::Brightness(100,true);
      if(lightData[index].colorType==TEMPERATURE_ONLY)
        new Characteristic::ColorTemperature(200,true);        
      if(lightData[index].colorType==FULL_RGB){
        new Characteristic::Hue(0,true);
        new Characteristic::Saturation(0,true);
      }
  
}

///////////////////////////

int addLight(const char *name, boolean isDimmable, colorType_t colorType){

  int index=0;
  for(index=0;index<MAX_LIGHTS && lightData[index].aid;index++);
  
  if(index==MAX_LIGHTS){
    Serial.printf("Can't add Light Accessory - maximum number of %d are already defined.\n",MAX_LIGHTS);
    return(-1);
  }
 
  int n=strncpy_trim(lightData[index].name,name,sizeof(lightData[index].name));

  if(n==1){
    Serial.printf("Can't add Light Accessory without a name specified.\n");
    return(-1);
  }

  if(n>sizeof(lightData[index].name))
    Serial.printf("Warning - name trimmed to max length of %d characters.\n",MAX_NAME_LENGTH);
  
  lightData[index].isDimmable=isDimmable;
  lightData[index].colorType=colorType;
  lightData[index].aid=aidStore++;

  nvs_set_blob(savedData,"LIGHTDATA",&lightData,sizeof(lightData));      // 更新 NVS 中的数据
  nvs_set_u32(savedData,"AID",aidStore);
  nvs_commit(savedData); 

  addLight(index);
  return(index);
}

///////////////////////////

size_t strncpy_trim(char *dest, const char *src, size_t dSize){

  while(*src==' ')                            // 跳过任何前导空格
    src++;

  size_t sLen=strlen(src);                    // 跳过前导空格后的 src 字符串长度
  while(sLen>0 && src[sLen-1]==' ')           // 缩短长度以删除尾随空格
    sLen--;

  size_t sSize=sLen+1;                        // 为空终止符添加空间
    
  if(dest!=NULL)
    *stpncpy(dest,src,(dSize<sSize?dSize:sSize)-1)='\0';
    
  return(sSize);                              // 返回整个修剪字符串所需的总大小，包括空终止符
}

///////////////////////////

void deleteAccessory(int index){

  if(index<0 || index>=MAX_LIGHTS){
    Serial.printf("Invalid Light Accessory index - must be between 0 and %d.\n",MAX_LIGHTS-1);
    return;
  }

  if(homeSpan.deleteAccessory(lightData[index].aid)){                            // 如果 deleteAccessory() 为真，则表示找到匹配项
    Serial.printf("Deleting Light Accessory:  Name='%s'\n",lightData[index].name);

    lightData[index].aid=0;
    nvs_set_blob(savedData,"LIGHTDATA",&lightData,sizeof(lightData));     // 更新 NVS 中的数据
    nvs_commit(savedData);
    
  } else {   
    Serial.printf("Nothing to delete - there is no Light Accessory at index=%d.\n",index);
  }
}

///////////////////////////

void deleteAllAccessories(const char *buf){

  for(int i=0;i<MAX_LIGHTS;i++){
    homeSpan.deleteAccessory(lightData[i].aid);
    lightData[i].aid=0;
  }
  
  nvs_set_blob(savedData,"LIGHTDATA",&lightData,sizeof(lightData));      // 更新 NVS 中的数据
  nvs_commit(savedData);

  Serial.printf("All Light Accessories deleted!\n");
}

///////////////////////////

void updateAccessories(const char *buf){
  
  if(homeSpan.updateDatabase())
    Serial.printf("Accessories Database updated.  New configuration number broadcasted...\n");
  else
    Serial.printf("Nothing to update - no changes were made!\n");
}

///////////////////////////

void listAccessories(const char *buf){

  Serial.printf("\nIndex  Dimmable  Color  Name\n");
  Serial.printf("-----  --------  -----  ");
  
  for(int i=0;i<MAX_NAME_LENGTH;i++)
    Serial.printf("-");
  Serial.printf("\n");
  for(int i=0;i<MAX_LIGHTS;i++){
    if(lightData[i].aid)
      Serial.printf("%5d  %8s  %5s  %-s\n",i,lightData[i].isDimmable?"YES":"NO",lightData[i].colorType==NO_COLOR?"NONE":(lightData[i].colorType==TEMPERATURE_ONLY?"TEMP":"RGB"),lightData[i].name);
  }
  Serial.printf("\n");
  
}

///////////////////////////

void setupWeb(){
  
  Serial.printf("Starting Light Server Hub at %s.local\n\n",HUB_NAME);
  webServer.begin();

  webServer.on("/", []() {
    
    String response = "<html><head><title>HomeSpan Programmable Light Hub</title>";
    response += "<style>table, th, td {border: 1px solid black; border-collapse: collapse;} th, td { padding: 5px; text-align: center; } </style></head>\n";
    response += "<body><h2>HomeSpan Lights</h2>";
    response += "<form action='/addLight' method='get'>";
    response += "<table><tr><th style='text-align:left;'>Accessory</th><th>Dim?</th><th>Color Control</th><th>Action</th></tr>";

    int openSlots=MAX_LIGHTS;
  
    for(int i=0;i<MAX_LIGHTS;i++){
      if(lightData[i].aid){
        response += "<tr><td style='text-align:left;'>" + String(lightData[i].name) + "</td>";
        response += "<td><input type='checkbox' disabled " + String(lightData[i].isDimmable?"checked>":">") + "</td>";
        response += "<td><input type='radio' disabled " + String(lightData[i].colorType==NO_COLOR?"checked>":">") + " NONE ";
        response += "<input type='radio' disabled " + String(lightData[i].colorType==TEMPERATURE_ONLY?"checked>":">") + " TEMP ONLY ";
        response += "<input type='radio' disabled " + String(lightData[i].colorType==FULL_RGB?"checked>":">") + " FULL COLOR </td>";
        response += "<td><button type='button' onclick=\"document.location='/deleteLight?index=" + String(i) + "'\">Delete Light</button></td>";
        response += "</tr>";
        openSlots--;
      }
    }

    response += "<tr><td style='text-align:left;'><input type='text' name='name' required placeholder='Type accessory name here...' size='"
             + String(MAX_NAME_LENGTH) + "' maxlength='" + String(MAX_NAME_LENGTH) + "'></td>";
    response += "<td><input type='checkbox' name='isDimmable'></td>";
    response += "<td><input type='radio' checked name='colorType' for='no_color' value='" + String(NO_COLOR) + "'><label for='no_color'> NONE </label>";
    response += "<input type='radio' name='colorType' for='temp_only' value='" + String(TEMPERATURE_ONLY) + "'><label for='temp_only'> TEMP ONLY </label>";
    response += "<input type='radio' name='colorType' for='full_rgb' value='" + String(FULL_RGB) + "'><label for='full_rgb'> FULL COLOR </label></td>";
    response += "<td><input type='submit' value='Add Light'" + String(openSlots?"":" disabled") + "></td>";
    response += "</tr>";      

    response += "</table>";
    response += "</form>";

    if(!openSlots)
      response += "<p>Can't add any more Light Accessories.  Max="+ String(MAX_LIGHTS) + "</p>";

    response += "<p>Press here to delete ALL Light Accessories: <button type='button' onclick=\"document.location='/deleteAll'\">Delete All Lights</button></p>";
    response += "<p>Press here to update the “家庭”应用when finished making changes: <button type='button' onclick=\"document.location='/update'\">Upddate HomeKit</button></p>";
    
    response += "</body></html>";
    webServer.send(200, "text/html", response);

  });

  webServer.on("/deleteLight", []() {

    int index=atoi(webServer.arg(0).c_str());

    String response = "<html><head><title>HomeSpan Programmable Light Hub</title><meta http-equiv='refresh' content = '3; url=/'/></head>";
    response += "<body>Deleting Light Accessory '" +  String(lightData[index].name) + "'...</body></html>";
    
    deleteAccessory(index);

    webServer.send(200, "text/html", response);

  });

  webServer.on("/deleteAll", []() {

    String response = "<html><head><title>HomeSpan Programmable Light Hub</title><meta http-equiv='refresh' content = '3; url=/'/></head>";
    response += "<body>Deleting All Light Accessories...</body></html>";    

    webServer.send(200, "text/html", response);
    deleteAllAccessories("");
    
  });  

  webServer.on("/update", []() {

    String response = "<html><head><title>HomeSpan Programmable Light Hub</title><meta http-equiv='refresh' content = '3; url=/'/></head><body>";
    
    if(homeSpan.updateDatabase())
      response += "Accessories Database updated.  New configuration number broadcasted...";
    else
      response += "Nothing to update - no changes were made...";

    response += "...</body></html>";      
    
    webServer.send(200, "text/html", response);

  });  

  webServer.on("/addLight", []() {

    colorType_t colorType=NO_COLOR;
    boolean isDimmable=false;
    int iName=-1;
     
    for(int i=0;i<webServer.args();i++){
      if(!webServer.argName(i).compareTo(String("colorType")))
        colorType=(colorType_t)webServer.arg(i).toInt();
      else if(!webServer.argName(i).compareTo(String("isDimmable")))
        isDimmable=true;
      else if(!webServer.argName(i).compareTo(String("name")))
        iName=i;
    }

    String response = "<html><head><title>HomeSpan Programmable Light Hub</title><meta http-equiv='refresh' content = '3; url=/'/></head><body>";

    if(iName!=-1){
      int index=addLight(webServer.arg(iName).c_str(),isDimmable,colorType);
      response += "Adding Light Accessory '" +  String(lightData[index].name) + "'";
    } else
      response += "Error - bad URL request";

    response += "...</body></html>";
    
    webServer.send(200, "text/html", response);

  });


}

///////////////////////////
