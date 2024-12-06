/**
  2个mos模块4个气泵

  视频链接：
  https://www.bilibili.com/video/BV1rr421g7Xv
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

// 气泵控制 -------------------------------------------------------------------------------------------
int airPumpPin[2] =  {2, 4};                                                                    // 控制气泵充气 引脚
int airValvePin[2] = {3, 5};                                                                    // 控制气阀放气 引脚

#define airPumpON                     HIGH                                                      // 气泵打开 开启充气
#define airPumpOFF                    !airPumpON                                                // 气泵关闭 停止充气

#define airValveON                    HIGH                                                      // 气阀打开 开启放气
#define airValveOFF                   !airValveON                                               // 气阀关闭 停止放气                  

//呼吸
#define inflateTime                   5000                                                      // 充气时间【可调 毫秒】
#define exhaustTime                   3600                                                      // 抽气时间【可调 毫秒】

#define defaultTime                   1000                                                      // 初始化放气时间
unsigned long systemRunTime = 0;                                                                // 系统运行时间

bool runState[2] = {true, false};                                                               // 运行状态 true为充 false为抽
unsigned long runTime[2] = {0, 0};                                                              // 记录运行时间

void setup() {
  Serial.begin(9600);                                                                           // 设置波特率9600
  for (int i = 0; i < 2; i++) {                                                                 // for循环
    pinMode(airPumpPin[i], OUTPUT);                                                             // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                                                            // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);         //
    digitalWrite(airValvePin[i], airValveOFF);       //
  }
  delay(defaultTime);
  systemRunTime = millis();
  for (int i = 0; i < 2; i++) {                                                                 // for循环
    digitalWrite(airPumpPin[i], airValveOFF);                                                   // 气泵开启 开启充气
    digitalWrite(airValvePin[i], airValveON);                                                   // 气阀关闭 停止放气
  }
  while (millis() - systemRunTime <= defaultTime) {                                             // 等待时间
  }
  Serial.println("Started successfully");
}

void loop() {
  for (int i = 0; i < 2; i++) {                                                                 // for循环
    if (runState[i] == true) {                                                                  // 状态为true充气
      digitalWrite(airPumpPin[i], airPumpON);                                                   // 气泵开启 开启充气
      digitalWrite(airValvePin[i], airValveOFF);                                                // 气阀关闭 停止放气

      if (millis() - runTime[i] >= inflateTime) {                                               // 判断时间是否达到
        runTime[i] = millis();
        runState[i] = false;
      }
    } else {                                                                                    // 状态为false抽气
      digitalWrite(airPumpPin[i], airPumpOFF);                                                  // 气泵关闭 停止充气
      digitalWrite(airValvePin[i], airValveON);                                                 // 气阀开启 开启放气

      if (millis() - runTime[i] >= exhaustTime) {                                               // 判断时间是否达到
        runTime[i] = millis();
        runState[i] = true;
      }
    }
  }

  // loop回括号
}
