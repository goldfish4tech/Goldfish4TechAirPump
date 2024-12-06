
/**
   气泵效果
   通过压力传感器进行变化

   视频链接：
   https://www.bilibili.com/video/BV1Cj421R7fn

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/
// -------------------------------- 压力效果 ------------------------------
#define pressureEffect         2                                                         // 压力效果【1】：有压力充气，无压力维持当前大小；压力效果【2】：有压力充气，无压力放气
// -----------------------------------------------------------------------
// -------------------------------- 气泵 ----------------------------------
#define airPumpCont            4                                                         // 气泵数量
int airPumpPin[airPumpCont] = {                                                          // 控制气泵充气 引脚
  2, 4, 6, 8
};
int airValvePin[airPumpCont] = {                                                         // 控制气阀放气 引脚
  3, 5, 7, 9                                                                             //A0 = 14, A1 = 15, A2 = 16, A3 = 17
};
#define airPumpON HIGH                                                                   // 气泵打开 开启充气
#define airPumpOFF !airPumpON                                                            // 气泵关闭 停止充气
#define airValveON HIGH                                                                  // 气阀打开 开启放气
#define airValveOFF !airValveON                                                          // 气阀关闭 停止放气
// -----------------------------------------------------------------------
// -------------------------------- 压力 ----------------------------------
#define pressurePin             A0                                                       // 控制引脚
int newPressureValue =          0;                                                       // 新的压力大小值
// -----------------------------------------------------------------------
// -------------------------------- 参数 ----------------------------------
#define getIntervalTime         5                                                        // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long getDataTime =     0;                                                       // 运行记录时间
#define setPressureLevel        250                                                      // 设置最大压力值（大于则开始充气）
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < airPumpCont; i++) {
    pinMode(airPumpPin[i], OUTPUT);                                                      // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                                                     // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                                             // 初始状态 关闭气泵 停止充气
    digitalWrite(airValvePin[i], airValveON);                                            // 初始状态 气阀打开 开启放气
  }
  Serial.println(F("Start successfully"));
}

void loop() {
  if (millis() - getDataTime >= getIntervalTime) {                                       // 判断时间是否满足
    getDataTime = millis();                                                              // 记录时间
    getPressureChange();                                                                 // 获取吹气变化

    Serial.println(newPressureValue);
#if (pressureEffect == 1)
    if (newPressureValue >= setPressureLevel) {
      for (int i = 0; i < airPumpCont; i++) {                                            // 循环设置4个气泵
        digitalWrite(airPumpPin[i], airPumpON);                                          // 开启充气
        digitalWrite(airValvePin[i], airValveOFF);                                       // 关闭放气
      }
    } else {
      for (int i = 0; i < airPumpCont; i++) {                                            // 循环设置4个气泵
        digitalWrite(airPumpPin[i], airPumpOFF);                                         // 关闭充气
      }
    }
#elif (pressureEffect == 2)
    if (newPressureValue >= setPressureLevel) {
      for (int i = 0; i < airPumpCont; i++) {                                            // 循环设置4个气泵
        digitalWrite(airPumpPin[i], airPumpON);                                          // 开启充气
        digitalWrite(airValvePin[i], airValveOFF);                                       // 关闭放气
      }
    } else {
      for (int i = 0; i < airPumpCont; i++) {                                            // 循环设置4个气泵
        digitalWrite(airPumpPin[i], airPumpOFF);                                         // 关闭充气
        digitalWrite(airValvePin[i], airValveON);                                        // 关闭放气
      }
    }
#endif
  }

}
// loop回括号

// 获取压力大小
void getPressureChange() {
  // 采集原始值
  newPressureValue = analogRead(pressurePin);                                            // 获取原始数据值
}
