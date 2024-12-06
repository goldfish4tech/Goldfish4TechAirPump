/**
  6个气泵充放气实现台阶效果

  视频链接：
  https://www.bilibili.com/video/BV1fm421J7LH
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

#define airPumpCont   6                                                               // 气泵数量

int airPumpPin[airPumpCont] = {                                                       // 控制气泵充气 引脚
  3, 5, 6, 9, 10, 11
};
int airValvePin[airPumpCont] = {                                                      // 控制气阀放气 引脚
  2, 4, 7, 8, 12, 13
};

#define airPumpON         HIGH                                                        // 气泵打开 开启充气
#define airPumpOFF        !airPumpON                                                  // 气泵关闭 停止充气
#define airValveON        HIGH                                                        // 气阀打开 开启放气
#define airValveOFF       !airValveON                                                 // 气阀关闭 停止放气

int setRunData[airPumpCont] = {                                                       // 设置充气运行参数
  250, 220, 190, 160, 130, 100
};
bool runState[airPumpCont] = {                                                        // 运行状态 true为充气 false为放气
  false, false, false, false, false, false
};
unsigned long RecordTime[airPumpCont] = {                                             // 记录阶段时间
  0, 0, 0, 0, 0, 0
};
unsigned long InflationTime[airPumpCont] = {                                          // 设置充气时间
  (30 * 1000UL), (30 * 1000UL), (30 * 1000UL),
  (30 * 1000UL), (30 * 1000UL), (30 * 1000UL)
};
unsigned long DeflationTime[airPumpCont] = {                                          // 设置放气时间
  (35 * 1000UL), (35 * 1000UL), (35 * 1000UL),
  (35 * 1000UL), (35 * 1000UL), (35 * 1000UL)
};

void setup() {
  Serial.begin(9600);                                                                 // 设置波特率9600
  for (int i = 0; i < airPumpCont; i++) {                                             // for循环设置
    pinMode(airPumpPin[i], OUTPUT);                                                   // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                                                  // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                                          // 初始状态 关闭气泵 停止充气
    digitalWrite(airValvePin[i], airValveON);                                         // 初始状态 气阀打开 开启放气
  }
  Serial.println(F("Started successfully"));
}

void loop() {
  for (int i = 0; i < airPumpCont; i++) {                                             // for循环设置
    if (runState[i] == true) {                                                        // 充气
      if (millis() - RecordTime[i] >= InflationTime[i]) {
        RecordTime[i] = millis();                                                     // 记录阶段时间
        runState[i] = false;                                                          // 状态变化
        Serial.println("开始放气: " + String(i + 1));

        // 设置放气
        digitalWrite(airPumpPin[i], airPumpOFF);                                      // 气泵关闭 停止充气
        digitalWrite(airValvePin[i], airValveON);                                     // 气阀开启 开启放气
      }
    } else {                                                                          // 放气
      if (millis() - RecordTime[i] >= DeflationTime[i]) {
        RecordTime[i] = millis();                                                     // 记录阶段时间
        runState[i] = true;                                                           // 状态变化
        Serial.println("开始充气: " + String(i + 1));

        // 设置充气
        analogWrite(airPumpPin[i], setRunData[i]);                                    // 气泵开启 开启充气
        digitalWrite(airValvePin[i], airValveOFF);                                    // 气阀关闭 停止放气
      }
    }
  }

  // loop回括号
}
