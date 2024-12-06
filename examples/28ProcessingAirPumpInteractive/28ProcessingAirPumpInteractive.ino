/**
  气泵和processing互动

  视频链接：
  https://www.bilibili.com/video/BV1yk28YUEFZ

  库文件：
  https://github.com/adafruit/Adafruit_MPR121
  https://github.com/adafruit/Adafruit_NeoPixel
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/


#include <Wire.h>                                                                                                           // I2C读写库
#include "Adafruit_MPR121.h"                                                                                                // MPR121触摸传感
#include <Adafruit_NeoPixel.h>                                                                                              // WS2812灯带

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

// MPR121 -------------------------------------------------------------------------------------------------------------------------
Adafruit_MPR121 cap = Adafruit_MPR121();                                                                                    // 实例化对象

#define debounceDelay                   100                                                                                 // 间隔时间毫秒
bool touchFlag[8] = {LOW};                                                                                                  // 触摸状态
bool lastTouchFlag[8] = {LOW};                                                                                              // 上一次触摸
unsigned long lastDebounceTime[8] = {0};                                                                                    // 触摸按下去的初始时间
// --------------------------------------------------------------------------------------------------------------------------------

// 集成化气泵 -----------------------------------------------------------------------------------------------------------------------
#define airPumpPin                      5                                                                                   // 控制气泵充气 引脚
#define airValvePin                     6                                                                                   // 控制气阀放气 引脚
#define airPumpON                       HIGH                                                                                // 气泵打开 开启充气
#define airPumpOFF                      !airPumpON                                                                          // 气泵关闭 停止充气
#define airValveON                      HIGH                                                                                // 气阀打开 开启放气
#define airValveOFF                     !airValveON                                                                         // 气阀关闭 停止放气
// --------------------------------------------------------------------------------------------------------------------------------

// WS2812 -------------------------------------------------------------------------------------------------------------------------
#define pixelsPin                       4                                                                                   // 控制引脚
#define pixelsQuantitys                 60                                                                                  // 灯珠数量
Adafruit_NeoPixel Pixels(pixelsQuantitys, pixelsPin, NEO_GRB + NEO_KHZ800);
uint8_t rgb[3][3] = {                                                                                                       // rgb灯
  {255,  0 , 0 },                                                                                                           // 红
  { 0 , 255, 0 }                                                                                                            // 绿
};
#define DefaultBrightness               100                                                                                 // 亮度值（0 ~ 255）
// --------------------------------------------------------------------------------------------------------------------------------

// 系统参数 -------------------------------------------------------------------------------------------------------------------------
int systemState = 0;                                                                                                        // 系统状态机

#define keepFlashing                    500                                                                                 // 闪烁保持时长
unsigned long flashingTime = 0;

bool airPumpState = false;                                                                                                  // 气泵运行状态
bool YNFlag = false;                                                                                                        // 充放状态

unsigned long recordRunTime = 0;                                                                                            // 记录运行时间
unsigned long tempRunTime = 0;                                                                                              // 设置对应时间
unsigned long airPumpRunTime[8] = {                                                                                         // 运行时间【可调 毫秒】
  1500, 1500, 1000, 2500, 2000, 2500, 3000, 6000
};
// --------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                                                                       // 设置波特率9600
  // 集成化气泵 -------------------
  pinMode(airPumpPin, OUTPUT);                                                                                              // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                                                             // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                                                                     // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveOFF);                                                                                   // 初始状态 气阀关闭 关闭放气
  // 触摸传感器 -------------------
  if (!cap.begin(0x5A)) {                                                                                                   // 连接地址
    Serial.println("MPR121 not found, check wiring?");
  }
  // WS2812灯 --------------------
  Pixels.begin();                                                                                                           // 初始化连接
  Pixels.setBrightness(DefaultBrightness);                                                                                  // 设置亮度
  Pixels.clear();                                                                                                           // 清除
  Pixels.show();                                                                                                            // 更新显示
  // 打印提示 ---------------------
  // Serial.println("Started successfully");                                                                                // 启动打印提示
}

void loop() {
  // 获取按钮状态 -------------------------------------
  getTouchFlag();                                                                                                           // 获取按钮状态

  // 根据不同状态设置运行 ------------------------------
  switch (systemState) {
    case 0: {                  // 默认状态 无情况

      } break;
    case 1: {                  // 运行模式一
        Serial.print("1");
        systemState = 9;
      } break;
    case 2: {                  // 运行模式二
        Serial.print("2");
        systemState = 9;
      } break;
    case 3: {                  // 运行模式三
        Serial.print("3");
        systemState = 9;
      } break;
    case 4: {                  // 运行模式四
        Serial.print("4");
        systemState = 9;
      } break;
    case 5: {                  // 运行模式五
        Serial.print("5");
        systemState = 11;
      } break;
    case 6: {                  // 运行模式六
        Serial.print("6");
        systemState = 11;
      } break;
    case 7: {                  // 运行模式七
        Serial.print("7");
        systemState = 11;
      } break;
    case 8: {                  // 运行模式八
        Serial.print("8");
        systemState = 11;
      } break;
    case 9: {                  // 闪烁一次，记录时间（绿色）
        flashingTime = millis();
        setRgb(1);
        systemState = 10;
      } break;
    case 10: {                 // 闪烁一次（绿色）
        if (millis() - flashingTime >= keepFlashing) {
          Pixels.clear();                                                                                                     // 清除
          Pixels.show();                                                                                                      // 更新显示
          systemState = 0;
        }
      } break;
    case 11: {                 // 闪烁一次,记录时间（红色）
        flashingTime = millis();
        setRgb(0);
        systemState = 12;
      } break;
    case 12: {                 // 闪烁一次 关闭（红色）
        if (millis() - flashingTime >= keepFlashing) {
          Pixels.clear();                                                                                                     // 清除
          Pixels.show();                                                                                                      // 更新显示
          systemState = 13;
          flashingTime = millis();
        }
      } break;
    case 13: {                 // 闪烁一次（红色）
        if (millis() - flashingTime >= keepFlashing) {
          setRgb(0);
          systemState = 10;
          flashingTime = millis();
        }
      } break;
    default: {                 // 其它
      } break;
  }
  // 控制运行气泵 -------------------------------------
  if (airPumpState == true) {
    if (systemState == true) {              // 放气
      if (millis() - recordRunTime >= tempRunTime) {
        digitalWrite(airPumpPin, airPumpOFF);                                                                                // 初始状态 关闭气泵 停止充气
        digitalWrite(airValvePin, airValveOFF);                                                                              // 初始状态 气阀关闭 关闭放气
        airPumpState = false;
        // Serial.println("-------------");
      }
    }
    else {                                  // 充气
      if (millis() - recordRunTime >= tempRunTime) {
        digitalWrite(airPumpPin, airPumpOFF);                                                                                // 初始状态 关闭气泵 停止充气
        digitalWrite(airValvePin, airValveOFF);                                                                              // 初始状态 气阀关闭 关闭放气
        airPumpState = false;
        // Serial.println("-------------");
      }
    }
  }

  // loop回括号
}

void getTouchFlag() {
  cap.touched();
  for (int i = 0; i < 8; i++) {
    bool reading = false;                                                                                                   // 起始状态为1（高电平）
    int tempVal = cap.filteredData(i);
    if (tempVal < 120) {
      reading = true;
    }

    if (reading != lastTouchFlag[i]) {                                                                                      // 当状态发生改变，给时间赋值
      lastDebounceTime[i] = millis();                                                                                       // 并记录时间
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {                                                                 // 判断时间
      if (reading != touchFlag[i]) {                                                                                        // 当状态发生改变
        touchFlag[i] = reading;                                                                                             // 赋值给touchFlag
        if (touchFlag[i] == HIGH) {
          // Serial.print(i + 1);
          // Serial.println("press");
          // 参数设置 -----------------------
          systemState = i + 1;                                                                                              // 设置状态机
          airPumpState = true;                                                                                              // 运行状态开启
          if (systemState <= 4) {                                                                                           // 如果小于值则为放气
            YNFlag = true;
            digitalWrite(airPumpPin, airPumpOFF);         // 气泵关闭 停止充气
            digitalWrite(airValvePin, airValveON);        // 气阀开启 开启放气
          } else {                                                                                                          // 如果大于值则为充气
            YNFlag = false;
            digitalWrite(airPumpPin, airPumpON);          // 气泵开启 开启充气
            digitalWrite(airValvePin, airValveOFF);       // 气阀关闭 停止放气
          }
          recordRunTime = millis();                                                                                         // 记录当前时间
          tempRunTime = airPumpRunTime[i];                                                                                  // 记录运行时长
          // Serial.println(tempRunTime);
        }
      }
    }
    lastTouchFlag[i] = reading;
  }
}

// 设置灯带颜色 -----------------------------------
void setRgb(int temp) {
  Pixels.clear();                                                                                                           // 清除灯带
  for (int i = 0; i < pixelsQuantitys; i++) {                                                                               // for循环设置
    Pixels.setPixelColor(i, Pixels.Color(rgb[temp][0], rgb[temp][1], rgb[temp][2]));                                        // 设置灯珠颜色
  }
  Pixels.show();                                                                                                            //
}
