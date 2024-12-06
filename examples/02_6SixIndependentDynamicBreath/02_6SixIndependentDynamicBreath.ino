/**
   Arduino 控制6个气泵

   气泵效果02
   呼吸效果，由小变大，由大变小

   视频链接：
   https://www.bilibili.com/video/BV1uL411i7RL
   
   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com

*/


#define airPumpCont          6                                  // 气泵数量

int airPumpPin[airPumpCont] = {                                 // 控制气泵充气 引脚
  3, 5, 6, 9, 10, 11
};  
int airValvePin[airPumpCont] = {                                // 控制气阀放气 引脚
  2, 4, 7, 8, 12, 13
};
unsigned long recordTime[airPumpCont] = {                       // 记录阶段时间
  0, 0, 0, 0, 0, 0
};
bool runState[airPumpCont] = {                                  // 运行状态 true为充气  false为放气
  true, true, true, true, true, true
};

// --------------------- 设置参数 --------------------
#define initializeTime        5000                              // 初始化放气 加载时间（1秒 = 1000毫秒）  *可调*
unsigned long runRecordTime = 0;                                // 运行记录时间

long SetInflationTime[airPumpCont] = {                          // 设置充气时间（对应1 ， 2 ， 3 ， 4）(1秒 = 1000毫秒)
  500, 500, 500, 500, 500, 500
};
long SetDeflationTime[airPumpCont] = {                          // 设置放气时间（对应1 ， 2 ， 3 ， 4）
  240, 240, 240, 240, 240, 240
};
// --------------------------------------------------

#define airPumpON      HIGH                                     // 气泵打开 开启充气
#define airPumpOFF     !airPumpON                               // 气泵关闭 停止充气
#define airValveON     HIGH                                     // 气阀打开 开启放气
#define airValveOFF    !airValveON                              // 气阀关闭 停止放气

void setup() {
  Serial.begin(9600);                                           // 设置波特率9600
  for (int i = 0; i < airPumpCont; i++) {
    pinMode(airPumpPin[i], OUTPUT);                             // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                            // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                    // 初始状态 关闭气泵 停止充气
    digitalWrite(airValvePin[i], airValveON);                   // 初始状态 气阀打开 开启放气
  }

  // 初始化开始时，先放气
  Serial.println(F("初始化放气中~"));
  delay(initializeTime);

  // 加载空气
  for (int i = 0; i < airPumpCont; i++) {
    digitalWrite(airPumpPin[i], airPumpON);                     // 开启充气
    digitalWrite(airValvePin[i], airValveOFF);                  // 关闭放气
  }
  delay(initializeTime);

  Serial.println("Start successfully");
}

void loop() {
  for (int i = 0; i < airPumpCont; i++) {                       // 循环设置4个气泵
    if (runState[i] == true) {                                  // 如果为true 充气
      digitalWrite(airValvePin[i], airValveOFF);                // 气阀关闭 停止放气
      digitalWrite(airPumpPin[i], airPumpON);                   // 打开气泵 开启充气
      if (millis() - recordTime[i] >= SetInflationTime[i]) {    // 判断时间是否达到
        runState[i] = false;                                    // 状态改变
        recordTime[i] = millis();                               // 记录时间
      }
    } else {                                                    // 如果为false 放气
      digitalWrite(airPumpPin[i], airPumpOFF);                  // 关闭气泵 停止充气
      digitalWrite(airValvePin[i], airValveON);                 // 气阀打开 开启放气
      if (millis() - recordTime[i] >= SetDeflationTime[i]) {    // 判断时间是否达到
        runState[i] = true;                                     // 状态改变
        recordTime[i] = millis();                               // 记录时间
      }
    }
  }

  // loop回括号
}
