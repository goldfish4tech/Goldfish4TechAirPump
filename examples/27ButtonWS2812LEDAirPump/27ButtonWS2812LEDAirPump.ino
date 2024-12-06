/**
  按钮控制气泵和灯环

  视频链接：
  https://www.bilibili.com/video/BV1SJ4m1M7Zq

  库文件：
  https://github.com/adafruit/Adafruit_NeoPixel
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

#include <Adafruit_NeoPixel.h>                                                                            // WS2812灯带库

// ------------------------------------------------------ 按钮 ----------------------------------------------------------
#define buttonNumber            2                                                                         // 按钮个数
int buttonPin[3] = {                                                                                      // 设置按键引脚
  2, 3
};
#define ButtonInterval          50                                                                        // 获取按钮状态间隔时间
unsigned long getButtonTime =   0;                                                                        // 记录阶段时间
bool buttonFlag[buttonNumber] = {                                                                         // 按钮状态
  false, false
};
// ---------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------ 气泵 ----------------------------------------------------------
#define airPumpCont             2                                                                         // 气泵数量
int airPumpPin[airPumpCont] = {                                                                           // 控制气泵充气 引脚
  4, 6
};
int airValvePin[airPumpCont] = {                                                                          // 控制气阀放气 引脚
  5, 7
};
unsigned long recordTime[airPumpCont] = {                                                                 // 记录阶段时间
  0, 0
};
#define airPumpON               HIGH                                                                      // 气泵打开 开启充气
#define airPumpOFF              !airPumpON                                                                // 气泵关闭 停止充气
#define airValveON              HIGH                                                                      // 气阀打开 开启放气
#define airValveOFF             !airValveON                                                               // 气阀关闭 停止放气
// ---------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------ 灯带 ----------------------------------------------------------
#define pixelsCount             2                                                                         // 灯带数量
int pixelsPin[pixelsCount] = {                                                                            // 灯带引脚
  8, 9
};
#define pixelsQuantitys         60                                                                        // 灯珠数量
Adafruit_NeoPixel PixelsLeft(pixelsQuantitys, pixelsPin[0], NEO_GRB + NEO_KHZ800);                        // 实例化灯带
Adafruit_NeoPixel PixelsRight(pixelsQuantitys, pixelsPin[1], NEO_GRB + NEO_KHZ800);                       // 实例化灯带
Adafruit_NeoPixel Pixels[pixelsCount] = {                                                                 // 灯带数组
  PixelsLeft, PixelsRight
};
#define Brightness              100                                                                       // 灯带亮度
uint8_t rgb[1][3] = {
  {255, 185, 105},                                                                                        // 暖白
};
// ---------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------ 参数 ----------------------------------------------------------
bool systemRunState[2] =  {false, false};                                                                 // 系统运行状态
bool lightStripState[2] = {true, true};                                                                   // 灯运行状态
unsigned long recordRunTime[2] = {0, 0};                                                                  // 记录运行时间
unsigned long openingTime[2] = {                                                                          // 灯带开启时间 【单位：毫秒】
  3000, 3000
};
unsigned long closeTime[2] = {                                                                            // 灯带关闭时间 【单位：毫秒】
  5000, 5000
};

bool recordRunStatus[2] = {1, 1};                                                                         // 记录运行状态
#define inflationTime           (50 * 1000UL)                                                             // 运行时 充气时间
#define deflateTime             (30 * 1000UL)                                                             // 放气时间
// ---------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                                                     // 设置波特率为9600
  // 按钮 -------------------
  for (int i = 0; i < buttonNumber; i++) {                                                                // for循环设置按钮
    pinMode(buttonPin[i], INPUT_PULLUP);                                                                  // 设置按键模式为上拉输入
  }
  // 气泵 -------------------
  for (int i = 0; i < airPumpCont; i++) {                                                                 // for循环设置
    pinMode(airPumpPin[i], OUTPUT);                                                                       // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                                                                      // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                                                              // 初始状态 关闭气泵 停止充气
    digitalWrite(airValvePin[i], airValveON);                                                             // 初始状态 气阀打开 开启放气
  }
  // 灯带 -------------------
  for (int i = 0; i < pixelsCount; i++) {                                                                 // for循环设置
    Pixels[i].begin();                                                                                    // 灯带连接
    Pixels[i].setBrightness(Brightness);                                                                  // 设置亮度
    Pixels[i].clear();                                                                                    // 清除
    Pixels[i].show();                                                                                     // 更新
  }
  setRgbOn(0);
  setRgbOn(1);
  // 启动运行提示 ------------
  Serial.println("Start successfully");                                                                   // 打印提示
}

void loop() {
  // ---------------------- 获取按钮按压状态 ----------------------------
  getButtonFlag();                                                                                        // 获取按钮状态

  // ---------------------- 设置运行状态 ----------------------------
  for (int i = 0; i < 2; i++) {                                                                           // for循环状态
    if (systemRunState[i] == false) {                                                                     // 判断运行状态
      // ---------------------- 灯切换等待人进入 ----------------------------
      if (lightStripState[i] == true) {                                                                   // 判断灯带切换
        // 判断时间是否超出
        if (millis() - recordRunTime[i] >= openingTime[i]) {                                              // 记录打开时间
          recordRunTime[i] = millis();
          lightStripState[i] = false;
          Serial.println("关闭");
          setRgbOff(i);                                                                                   // 关闭
        }
        // ------------------ 判断灯打开时，是否有人进入 ---------------------
        if (buttonFlag[i] == true) {                                                                      // 如果对应按钮有人进入
          systemRunState[i] = true;                                                                       // 启用运行状态
          setRgbOn(i);                                                                                    // 打开灯
          digitalWrite(airPumpPin[i], airPumpON);                                                         // 开始充气
          digitalWrite(airValvePin[i], airValveOFF);                                                      // 停止放气
          recordTime[i] = millis();                                                                       // 记录当前时间
          recordRunStatus[i] = true;                                                                      // 运行状态为true
        }
      } else {
        // 判断时间是否超出
        if (millis() - recordRunTime[i] >= closeTime[i]) {                                                // 记录关闭时间
          recordRunTime[i] = millis();
          lightStripState[i] = true;
          Serial.println("开启");
          setRgbOn(i);                                                                                    // 打开
        }
      }
    } else {
      // ----------------------- 踩上去运行状态 ----------------------------
      if (recordRunStatus[i] == true) {
        // ------------------- 判断人是否有走开 ----------------------
        if (buttonFlag[i] == false) {
          recordTime[i] = millis();
          digitalWrite(airPumpPin[i], airPumpOFF);                                                        // 关闭充气
          digitalWrite(airValvePin[i], airValveON);                                                       // 开始放气
          recordRunStatus[i] = false;                                                                     // 状态切换
          setRgbOff(i);                                                                                   // 关闭
        }
        // ------------------- 判断充气时间是否满足 ----------------------
        if (millis() - recordTime[i] >= inflationTime) {                                                  // 判断超出时间
          recordTime[i] = millis();
          digitalWrite(airPumpPin[i], airPumpOFF);                                                        // 关闭充气
        }
      } else {
        if (millis() - recordTime[i] >= deflateTime) {                                                    // 判断放气时间
          systemRunState[i] = false;                                                                      // 系统运行状态改变
        }
      }
    }
  }

  // loop回括号
}

// 获取按钮状态
void getButtonFlag() {
  if (millis() - getButtonTime >= ButtonInterval) {                                                       // 判断时间是否满足
    getButtonTime = millis();                                                                             // 记录当前时间
    for (int i = 0; i < buttonNumber; i++) {                                                              // for循环
      if (digitalRead(buttonPin[i]) == LOW) {                                                             // 判断获取按钮状态
        buttonFlag[i] = true;                                                                             // true为按下
      } else {
        buttonFlag[i] = false;                                                                            // false为松开
      }
    }
  }
}

// 设置灯带打开
void setRgbOn(int runSubscript) {
  Pixels[runSubscript].clear();                                                                           // 清除灯设置
  for (int i = 0; i < pixelsQuantitys; i++) {                                                             // for灯珠数量
    Pixels[runSubscript].setPixelColor(i, Pixels[runSubscript].Color(rgb[0][0], rgb[0][1], rgb[0][2]));   // 设置灯珠
  }
  Pixels[runSubscript].show();                                                                            // 更新设置灯珠
}

// 设置灯带关闭
void setRgbOff(int runSubscript) {
  Pixels[runSubscript].clear();                                                                           // 清除灯设置
  Pixels[runSubscript].show();                                                                            // 更新设置灯珠
}
