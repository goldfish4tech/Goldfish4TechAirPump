/**
   集成化气泵04_2——心率存在感应2
   有心率充气，没心率放气

   视频链接：
   https://www.bilibili.com/video/BV19u4m1M7Jr

   库文件：
   https://github.com/oxullo/Arduino-MAX30100/tree/master

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

// 心率模块 -------------------------------------------------------------------------------------------------
#define REPORTING_PERIOD_MS   1000
PulseOximeter pox;
uint32_t tsLastReport = 0;
// ---------------------------------------------------------------------------------------------------------

// 气泵 -----------------------------------------------------------------------------------------------------
#define airPumpPin            5                                                       // 控制气泵充气 引脚
#define airValvePin           6                                                       // 控制气阀放气 引脚
#define airPumpON             HIGH                                                    // 气泵打开 开启充气
#define airPumpOFF            !airPumpON                                              // 气泵关闭 停止充气
#define airValveON            HIGH                                                    // 气阀打开 开启放气
#define airValveOFF           !airValveON                                             // 气阀关闭 停止放气                                                      
// ---------------------------------------------------------------------------------------------------------

// 在检测到脉搏时触发的回调函数（在下面注册）
void onBeatDetected() {
  //Serial.println("心跳!");
}

void setup() {
  Serial.begin(9600);                                                                 // 用于串行监视器
  // 气泵设置 -------------------
  pinMode(airPumpPin, OUTPUT);                                                        // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                       // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                               // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveOFF);                                             // 初始状态 气阀关闭 关闭放气
  // --------------- 心率 ---------------
  if (!pox.begin()) {
    Serial.println("initialization failed");
    for (;;);
  }
  // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);                                    // IR LED的默认电流为50mA，可以通过取消以下行的注释进行更改。
  pox.setOnBeatDetectedCallback(onBeatDetected);                                      // 注册脉搏检测的回调函数
  Serial.println(F("Start successfully"));                                            // 启动提示
}

void loop() {
  // 确保尽快调用 update
  pox.update();

  // 异步将心率和氧化水平转储到串行端口
  // 对于两者，值为0表示 "无效"
  if (millis() - tsLastReport >= REPORTING_PERIOD_MS) {
    Serial.print("心率: ");
    float heartRateVal = pox.getHeartRate();
    Serial.print(heartRateVal);
    Serial.print("bpm / SpO2: ");
    Serial.print(pox.getSpO2());
    Serial.println("%");
    tsLastReport = millis();

    if (heartRateVal > 0) {
      digitalWrite(airPumpPin, airPumpON);                                            // 开启充气
      digitalWrite(airValvePin, airValveOFF);                                         // 关闭放气
    } else {
      digitalWrite(airPumpPin, airPumpOFF);                                           // 停止充气
      digitalWrite(airValvePin, airValveON);                                          // 开启放气
    }
  }

  // loop回括号
}
