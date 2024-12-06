/**
   集成化气泵04_3——心率存在感应3
   有心率充气，没心率放气

   视频链接：
   https://www.bilibili.com/video/BV1UH4y1s7PV

   库文件链接：
   https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

#include <Wire.h>                                                                     // I2C通讯库
#include "MAX30105.h"                                                                 // 心率传感器库
#include "heartRate.h"                                                                // 数据解析库

// 心率模块 -------------------------------------------------------------------------------------------------
MAX30105 particleSensor;                                                              // 实例化
long lastBeat = 0;                                                                    // 最后记录心率的时间
float beatsPerMinute;                                                                 // 心率值
#define heartRateActivationTime  5000                                                 // 设定规定时间内如果心率过高则保持高心率状态
// ---------------------------------------------------------------------------------------------------------

// 气泵 -----------------------------------------------------------------------------------------------------
#define airPumpPin               5                                                    // 控制气泵充气 引脚
#define airValvePin              6                                                    // 控制气阀放气 引脚
#define airPumpON                HIGH                                                 // 气泵打开 开启充气
#define airPumpOFF               !airPumpON                                           // 气泵关闭 停止充气
#define airValveON               HIGH                                                 // 气阀打开 开启放气
#define airValveOFF              !airValveON                                          // 气阀关闭 停止放气                                                      
// ---------------------------------------------------------------------------------------------------------


void setup() {
  Serial.begin(9600);                                                                 // 用于串行监视器

  // 气泵设置 -------------------
  pinMode(airPumpPin, OUTPUT);                                                        // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                       // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                               // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveOFF);                                             // 初始状态 气阀关闭 关闭放气

  // --------------- 心率 ---------------
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {                                  // 默认使用I2C，400KHZ频率
    while (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
      Serial.println(F("MAX30105 was not found. Please check wiring/power. "));
    }
  }
  particleSensor.setup();                                                             // 使用默认设置配置传感器
  particleSensor.setPulseAmplitudeRed(0x0A);                                          // 将红色LED拉低，表示传感器正在运行
  // ---------------------------------------------
  Serial.println(F("Start successfully"));                                            // 启动提示
}
void loop() {
  // 获取心率 ------------------------------------
  getHeartRateData();

  // loop回括号
}

// 获取心率传感器数据
void getHeartRateData() {
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true) {                                                // 感应到心率
    long delta = millis() - lastBeat;                                                 // 间隔时间
    lastBeat = millis();                                                              // 记录阶段时间
    beatsPerMinute = 60 / (delta / 1000.0);                                           // 转换
  }
  if (irValue > 50000) {                                                              // 如果采集量在符合的标准内
    Serial.print(F("BPM: "));
    Serial.println(beatsPerMinute);

    digitalWrite(airPumpPin, airPumpON);                                              // 开启充气
    digitalWrite(airValvePin, airValveOFF);                                           // 关闭放气
  } else {
    beatsPerMinute = 0;
 
    digitalWrite(airPumpPin, airPumpOFF);                                             // 停止充气
    digitalWrite(airValvePin, airValveON);                                            // 开启放气
  }
}
