/**
   气泵效果02_2 四气泵（单个轮替动态呼吸）
   呼吸效果，由小变大，由大变小

   视频链接：
   https://www.bilibili.com/video/BV14z421Q7Ku

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/


#define airPumpCont       4                                                           // 气泵数量

int airPumpPin[airPumpCont] = {                                                       // 控制气泵充气 引脚
  2, 4, 6, 9
};
int airValvePin[airPumpCont] = {                                                      // 控制气阀放气 引脚
  3, 5, 7, 8
};

#define airPumpON         HIGH                                                        // 气泵打开 开启充气
#define airPumpOFF        !airPumpON                                                  // 气泵关闭 停止充气
#define airValveON        HIGH                                                        // 气阀打开 开启放气
#define airValveOFF       !airValveON                                                 // 气阀关闭 停止放气

int setRunData[airPumpCont] = {                                                       // 设置充气运行参数
  250, 200, 150, 100
};
uint8_t runState[airPumpCont] = {                                                     // 运行状态 0为停止 1为充气 2为放气
  0x00, 0x00, 0x00, 0x00
};
unsigned long RecordTime[airPumpCont] = {                                             // 记录阶段时间
  0, 0, 0, 0
};
unsigned long StopTime[airPumpCont] = {                                               // 设置停止时间【可调 毫秒】
  (2 * 1000UL), (3 * 1000UL), (4 * 1000UL), (5 * 1000UL)
};
unsigned long InflationTime[airPumpCont] = {                                          // 设置充气时间【可调 毫秒】
  (15 * 1000UL), (15 * 1000UL), (15 * 1000UL), (15 * 1000UL)
};
unsigned long DeflationTime[airPumpCont] = {                                          // 设置放气时间【可调 毫秒】
  (20 * 1000UL), (20 * 1000UL), (20 * 1000UL), (20 * 1000UL)
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
    switch (runState[i]) {
      case 0x00: {                                                                    // 停止
          // 停止 ------------------------------------------
          digitalWrite(airPumpPin[i], airPumpOFF);                                    // 气泵关闭 停止充气
          digitalWrite(airValvePin[i], airValveOFF);                                  // 气阀关闭 停止放气
          // 等待时间 ---------------------------------------
          if (millis() - RecordTime[i] >= StopTime[i]) {
            RecordTime[i] = millis();
            runState[i] = 0x01;
          }
        } break;
      case 0x01: {                                                                    // 充气
          // 充气 ------------------------------------------
          analogWrite(airPumpPin[i], setRunData[i]);                                  // 气泵开启 开启充气
          digitalWrite(airValvePin[i], airValveOFF);                                  // 气阀关闭 停止放气
          // 等待时间 ---------------------------------------
          if (millis() - RecordTime[i] >= InflationTime[i]) {
            RecordTime[i] = millis();
            runState[i] = 0x02;
          }
        } break;
      case 0x02: {                                                                    // 放气
          // 放气 ------------------------------------------
          digitalWrite(airPumpPin[i], airPumpOFF);                                    // 气泵关闭 停止充气
          digitalWrite(airValvePin[i], airValveON);                                   // 气阀开启 开启放气
          // 等待时间 ---------------------------------------
          if (millis() - RecordTime[i] >= DeflationTime[i]) {
            RecordTime[i] = millis();
            runState[i] = 0x00;
          }
        } break;
      default: break;
    }
  }

  // loop回括号
}
