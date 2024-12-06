
/**
   气泵效果
   通过压力传感器进行变化

   视频链接：
   https://www.bilibili.com/video/BV12m4y1H7sV

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// -------------------------------- 压力效果 ------------------------------
#define pressureEffect        2                                                    // 压力效果【1】：压一下，变大一下；压力效果【2】：压力越大，气球越大，
// -----------------------------------------------------------------------
// -------------------------------- 气泵 ----------------------------------
#define airPumpPin            5                                                    // 控制气泵充气 引脚
#define airValvePin           6                                                    // 控制气阀放气 引脚
#define airPumpON             HIGH                                                 // 气泵打开 开启充气
#define airPumpOFF            !airPumpON                                           // 气泵关闭 停止充气
#define airValveON            HIGH                                                 // 气阀打开 开启放气
#define airValveOFF           !airValveON                                          // 气阀关闭 停止放气
// -----------------------------------------------------------------------
// -------------------------------- 压力 ----------------------------------
#define pressurePin           A0                                                   // 控制引脚
int newPressureValue =        0;                                                   // 新的压力大小值
// -----------------------------------------------------------------------
// -------------------------------- 参数 ----------------------------------
#define getIntervalTime       5                                                    // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long getDataTime =   0;                                                   // 运行记录时间
#define setPressureLevel      250                                                  // 设置最大压力值（大于则开始充气）
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(airPumpPin, OUTPUT);                                                     // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                    // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                            // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                                           // 初始状态 气阀打开 开启放气
  //analogWrite(airPumpPin, 0);                                                    // 初始状态 关闭气泵 停止充气
  //analogWrite(airValvePin, 255);                                                 // 初始状态 气阀打开 开启放气
  // --------------- 压力传感器 --------------------
  // ---------------------------------------------
  Serial.println(F("Start successfully"));

}

void loop() {
  if (millis() - getDataTime >= getIntervalTime) {                                 // 判断时间是否满足
    getDataTime = millis();                                                        // 记录时间
    getPressureChange();                                                           // 获取吹气变化

    Serial.println(newPressureValue);
#if (pressureEffect == 1)
    if (newPressureValue >= setPressureLevel) {
      digitalWrite(airPumpPin, airPumpON);                                         // 开启充气
      digitalWrite(airValvePin, airValveOFF);                                      // 关闭放气
    } else {
      digitalWrite(airPumpPin, airPumpOFF);                                        // 关闭充气
    }
#elif (pressureEffect == 2)
    if (newPressureValue >= setPressureLevel) {
      digitalWrite(airPumpPin, airPumpON);                                         // 开启充气
      digitalWrite(airValvePin, airValveOFF);                                      // 关闭放气
    } else {
      digitalWrite(airPumpPin, airPumpOFF);                                        // 关闭充气
      digitalWrite(airValvePin, airValveON);                                       // 关闭放气
    }
#endif
  }

  // loop回括号
}

// 获取压力大小
void getPressureChange() {
  // 采集原始值
  newPressureValue = analogRead(pressurePin);                                       // 获取原始数据值
}
