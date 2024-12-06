/**
  超声波互动气泵装置

  视频链接：
  https://www.bilibili.com/video/BV1Bt421K7zc
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

// --------------------------------- 超声波 ----------------------------
int TrigPin[4] = {                                                    // Trig引脚
  7, 9, 11, A1
};
int EchoPin[4] = {                                                    // Echo引脚
  6, 8, 10, A0
};
float distance[4] = {                                                 // 距离数据
  0, 0, 0, 0
};
// --------------------------------------------------------------------
// --------------------------------- 气泵 ------------------------------
#define airPumpCont   2                                               // 气泵数量
int airPumpPin[airPumpCont] = {                                       // 控制气泵充气 引脚
  4, 5
};
int airValvePin[airPumpCont] = {                                      // 控制气阀放气 引脚
  2, 3
};
#define airPumpON     HIGH                                            // 气泵打开 开启充气
#define airPumpOFF    !airPumpON                                      // 气泵关闭 停止充气
#define airValveON    HIGH                                            // 气阀打开 开启放气
#define airValveOFF   !airValveON                                     // 气阀关闭 停止放气
// --------------------------------------------------------------------
// -------------------------------- 系统参数 ----------------------------
#define getIntervalTime         300                                   // 获取数据间隔时间（1秒 = 1000毫秒）
unsigned long runGetTime =      0;                                    // 运行时间

uint8_t systemRunState =        0x00;                                 // 系统运行状态
#define systemRunIntervalTime   (1 * 60 * 1000UL)                     // 系统运行时间 1分钟 * 60秒 * 1000毫秒
unsigned long systemRunTime =   0;                                    // 运行时间
bool runTimingState =           true;                                 // 记录时间状态

#define maxDistance             200                                   // 设置距离范围内（1米 = 100cm）
#define setDistance             5                                     // 设置距离偏差量(距离差值)

#define delayTime               (30 * 1000UL)                         // 设置等待延迟时间
// --------------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                 // 波特率9600
  // ----------- 超声波 ------------
  for (int i = 0; i < 4; i++) {                                       // for循环设置
    pinMode(TrigPin[i], OUTPUT);                                      // 设置引脚输出
    pinMode(EchoPin[i], INPUT);                                       // 设置引脚输入
  }
  // ----------- 气泵 ------------
  for (int i = 0; i < airPumpCont; i++) {
    pinMode(airPumpPin[i], OUTPUT);                                   // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                                  // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                          // 初始状态 关闭气泵 停止充气
    digitalWrite(airValvePin[i], airValveON);                         // 初始状态 气阀打开 开启放气
  }
  Serial.println(F("Start successfully"));                            // 打印提示
}

void loop() {
  if (millis() - runGetTime >= getIntervalTime) {                     // 间隔时间获取数据
    runGetTime = millis();                                            // 记录阶段时间
    getDistanceData();                                                // 获取4超声波距离数据
  }


  switch (systemRunState) {                                           // 状态机运行
    case 0x00: {                                                      // 判断数据状态
        for (int i = 0; i < 2; i++) {
          if (distance[i] != 0 && distance[i] < maxDistance && distance[i + 2] != 0 && distance[i + 2] < maxDistance) {
            if (abs(distance[i] - distance[i + 2]) <= setDistance) {  // 判断距离差值
              // 充气
              digitalWrite(airPumpPin[i], airPumpON);                 // 气泵开启 开启充气
              if (runTimingState == true) {                           // 进入数据状态
                systemRunState = 0x01;                                // 状态改变
                Serial.println(F("--------- 游戏开始 --------"));      // 打印提示
              }
            } else {                                                  // 保持
              digitalWrite(airPumpPin[i], airPumpOFF);                // 气泵关闭 停止充气
            }
            digitalWrite(airValvePin[i], airValveOFF);                // 气阀关闭 停止放气
          } else {
            digitalWrite(airPumpPin[i], airPumpOFF);                  // 气泵关闭 停止充气
          }
        }

        // 运行时间判断
        if (runTimingState == false) {                                // 如果游戏开启
          if (millis() - systemRunTime >= systemRunIntervalTime) {    // 判断时间
            systemRunTime = millis();                                 // 记录时间
            systemRunState = 0x02;                                    // 状态02
            Serial.println(F("--------- 游戏时间结束 --------"));       // 打印提示
            digitalWrite(airValvePin[0], airValveON);                 // 开启放气
            digitalWrite(airValvePin[1], airValveON);                 // 开启放气
          }
        }
      } break;
    case 0x01: {                                                      // 刷新开启记录
        runTimingState = false;
        systemRunTime = millis();                                     // 记录时间
        systemRunState = 0x00;                                        // 状态改变
      } break;
    case 0x02: {
        if (millis() - systemRunTime >= delayTime) {                  // 判断时间
          systemRunState = 0x00;                                      // 状态00
          runTimingState = true;
          Serial.println(F("--------- 游戏重置完成 --------"));         // 打印提示
        }
      } break;
    default: {
      } break;
  }


  // loop 回括号
}


void getDistanceData() {
  for (int i = 0; i < 4; i++) {                                       // for循环获取数据
    digitalWrite(TrigPin[i], LOW);                                    // 设置引脚低电平
    delayMicroseconds(2);                                             // 延迟2微秒
    digitalWrite(TrigPin[i], HIGH);                                   // 设置引脚高电平
    delayMicroseconds(10);                                            // 延迟10微秒
    digitalWrite(TrigPin[i], LOW);                                    // 设置引脚低电平
    distance[i] = pulseIn(EchoPin[i], HIGH) / 58.0;                   // 获取时间进行转换
    distance[i] = (int(distance[i] * 100.0)) / 100.0;                 // 转换获取距离
    if (distance[i] > 300) {                                          // 限定最大距离
      distance[i] = 300;                                              // 赋值
    }
    Serial.print(distance[i]);                                        // 打印提示
    Serial.print("cm,  ");                                            // 打印提示
  }
  Serial.println();
}
