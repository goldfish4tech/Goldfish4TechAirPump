/**
   气泵效果02
   呼吸效果，由小变大，由大变小

   视频链接：
   

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// 气泵初始化 ---------------------------------------------------------
#define airPumpCont         2                                     // 气泵数量

int airPumpPin[airPumpCont]  = {2, 4};                            // 控制气泵充气 引脚
int airValvePin[airPumpCont] = {3, 5};                            // 控制气阀放气 引脚

#define airPumpON           HIGH                                  // 气泵打开 开启充气
#define airPumpOFF          !airPumpON                            // 气泵关闭 停止充气

#define airValveON          HIGH                                  // 气阀打开 开启放气
#define airValveOFF         !airValveON                           // 气阀关闭 停止放气

// 系统参数 -----------------------------------------------------------
uint8_t systemState[airPumpCont] = {0x00, 0x00};                  // 不同运行状态
unsigned long recordTime[airPumpCont] = {0, 0};                   // 记录阶段时间

unsigned long initializeDelay[airPumpCont] = {0, 2000};           // 初始化等待时间【可调 毫秒】

unsigned long inflationTime[airPumpCont] = {3000, 5000};          // 设置充气时间【可调 毫秒】

unsigned long deflationTime[airPumpCont] = {3000, 5000};          // 设置放气时间屏【可调 毫秒】

void setup() {
  Serial.begin(9600);                                             // 设置波特率9600
  // 初始化气泵 --------------
  for (int i = 0; i < airPumpCont; i++) {
    pinMode(airPumpPin[i], OUTPUT);                               // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                              // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                      // 初始状态 关闭气泵
    digitalWrite(airValvePin[i], airValveOFF);                    // 初始状态 气阀关闭

    recordTime[i] = millis();                                     // 更新记录当前时间
  }
  // 启动打印提示 ------------
  Serial.println("Start successfully");                           // 打印提示
}

void loop() {
  for (int i = 0; i < airPumpCont; i++) {
    switch (systemState[i]) {
      case 0x00: {                                                // 初始化等待时间
          if (millis() - recordTime[i] >= initializeDelay[i]) {   // 判断等待时间是否达到
            recordTime[i] = millis();                             // 记录阶段时间
            systemState[i] = 0x01;                                // 状态改变
            Serial.println(String(i + 1) + " 初始化结束");
            Serial.println(String(i + 1) + " 开始充气");
          }
        } break;
      case 0x01: {                                                // 充气
          digitalWrite(airPumpPin[i], airPumpON);                 // 打开气泵
          digitalWrite(airValvePin[i], airValveOFF);              // 关闭气阀
          if (millis() - recordTime[i] >= inflationTime[i]) {     // 判断等待时间是否达到
            recordTime[i] = millis();                             // 记录阶段时间
            systemState[i] = 0x02;                                // 状态改变
            Serial.println(String(i + 1) + " 开始放气");
          }
        } break;
      case 0x02: {                                                // 放气
          digitalWrite(airPumpPin[i], airPumpOFF);                // 关闭气泵
          digitalWrite(airValvePin[i], airValveON);               // 打开气阀
          if (millis() - recordTime[i] >= deflationTime[i]) {     // 判断等待时间是否达到
            recordTime[i] = millis();                             // 记录阶段时间
            systemState[i] = 0x01;                                // 状态改变
            Serial.println(String(i + 1) + " 开始充气");
          }
        } break;
      default: break;
    }
  }

  // loop回括号
}
