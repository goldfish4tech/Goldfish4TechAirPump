/**
   集成化气泵04_3——心率存在感应3
   有心率充气，没心率放气

   视频链接：
   https://www.bilibili.com/video/BV14ZsfevEbZ

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

//----------------------------------心率模块-------------------------------
MAX30105 particleSensor;                                                                          // 实例化
long lastBeat = 0;                                                                                // 最后记录心率的时间
float beatsPerMinute;                                                                             // 心率值
#define heartRateActivationTime  5000                                                             // 设定规定时间内如果心率过高则保持高心率状态
// -----------------------------------------------------------------------
// 气泵控制 ---------------------------------------------------------------
int airPumpPin[2]  = {5};                                                                         // 控制气泵充气 引脚
int airValvePin[2] = {6};                                                                         // 控制气阀放气 引脚

#define airPumpON                     HIGH                                                        // 气泵打开 开启充气
#define airPumpOFF                    !airPumpON                                                  // 气泵关闭 停止充气

#define airValveON                    HIGH                                                        // 气阀打开 开启放气
#define airValveOFF                   !airValveON                                                 // 气阀关闭 停止放气                  

//呼吸
#define startinflat                   10*1000UL
#define inflateTime                   5*1000UL                                                    // 充气时间【可调 毫秒】
#define exhaustTime                   5*1000UL                                                    // 抽气时间【可调 毫秒】

#define defaultTime                   15*1000UL                                                   // 初始化放气时间
unsigned long systemRunTime =         0;                                                          // 系统运行时间

bool runState[2] = {true, false};                                                                 // 运行状态 true为充 false为抽
unsigned long runTime[2] = {0, 0};                                                                // 记录运行时间

void setup() {
  Serial.begin(9600);                                                                             // 设置波特率9600
  // for (int i = 0; i < 2; i++) {                                                                // for循环
  //   pinMode(airPumpPin[i], OUTPUT);                                                            // 设置引脚输出
  //   pinMode(airValvePin[i], OUTPUT);                                                           // 设置引脚输出
  //   digitalWrite(airPumpPin[i], airPumpOFF);         
  //   digitalWrite(airValvePin[i], airValveOFF);       
  // }
  // delay(defaultTime);
  systemRunTime = millis();
  for (int i = 0; i < 2; i++) {                                                                   // for循环
    digitalWrite(airPumpPin[i], airValveOFF);                                                     // 气泵开启 开启充气
    digitalWrite(airValvePin[i], airValveON);                                                     // 气阀关闭 停止放气
    Serial.println("初始化抽气");
  }
  while (millis() - systemRunTime <= defaultTime) {                                               // 等待时间
  }
  systemRunTime = millis();
  for (int i = 0; i < 2; i++) {                                                                   // for循环
    digitalWrite(airPumpPin[i], airValveON);                                                      // 气泵开启 开启充气
    digitalWrite(airValvePin[i], airValveOFF);                                                    // 气阀关闭 停止放气
    Serial.println("初始化充气");
  }
  while (millis() - systemRunTime <= startinflat) {                                               // 等待时间
  }
  // --------------- 心率 ---------------
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {                                              // 默认使用I2C，400KHZ频率
    while (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
      Serial.println(F("MAX30105 was not found. Please check wiring/power. "));
    }
  }
  particleSensor.setup();                                                                         // 使用默认设置配置传感器
  particleSensor.setPulseAmplitudeRed(0x0A);                                                      // 将红色LED拉低，表示传感器正在运行
  // ---------------------------------------------
  Serial.println("Started successfully");
}

void loop() {
  // 获取心率 ------------------------------------
  getHeartRateData();

  // loop回括号
}

void getHeartRateData() {
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true) {                                                            // 感应到心率
    long delta = millis() - lastBeat;                                                             // 间隔时间
    lastBeat = millis();                                                                          // 记录阶段时间
    beatsPerMinute = 60 / (delta / 1000.0);                                                       // 转换
  }
  if (irValue > 50000) {                                                                          // 如果采集量在符合的标准内
    Serial.print(F("BPM: "));
    Serial.println(beatsPerMinute);

    for (int i = 0; i < 2; i++) {                                                                 // for循环
      if (runState[i] == true) {                                                                  // 状态为true充气
        digitalWrite(airPumpPin[i], airPumpON);                                                   // 气泵开启 开启充气
        digitalWrite(airValvePin[i], airValveOFF);                                                // 气阀关闭 停止放气
        Serial.println("开始充气");


        if (millis() - runTime[i] >= inflateTime) {                                               // 判断时间是否达到
          runTime[i] = millis();
          runState[i] = false;
        }
      } else {                                                                                    // 状态为false抽气
        digitalWrite(airPumpPin[i], airPumpOFF);                                                  // 气泵关闭 停止充气
        digitalWrite(airValvePin[i], airValveON);                                                 // 气阀开启 开启放气
        Serial.println("开始放气");

        if (millis() - runTime[i] >= exhaustTime) {                                               // 判断时间是否达到
          runTime[i] = millis();
          runState[i] = true;
        }
      }
    }
  } else {
    beatsPerMinute = 0;

    for (int i = 0; i < 2; i++) {
      digitalWrite(airPumpPin[i], airPumpOFF);                                                    // 停止充气
      digitalWrite(airValvePin[i], airValveOFF);                                                  // 开启放气
      Serial.println("检测不到心率");
    }
  }
}
