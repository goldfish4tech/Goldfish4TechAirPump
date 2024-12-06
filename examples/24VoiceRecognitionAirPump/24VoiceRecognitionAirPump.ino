/**
  语音控制气泵充放气

  视频链接：
  https://www.bilibili.com/video/BV1phs6e3Eoo
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

#include "SoftwareSerial.h"                                                                                         // 软串口库

// 语音模块 ------------------------------------------------------------------------------------------------------------------
#define voiceRx                 2                                                                                   // 定义针脚
#define voiceTx                 3
SoftwareSerial mySerial(voiceRx, voiceTx);                                                                          // 定义软串口

// 震动马达 ------------------------------------------------------------------------------------------------------------------
#define motorPinA               9                                                                                   // 电机
#define motorPinB               10                                                                                  // 电机

// 气泵 ----------------------------------------------------------------------------------------------------------------------
#define airPumpPin              5                                                                                   // 控制气泵充气 引脚
#define airValvePin             6                                                                                   // 控制气阀放气 引脚
#define airPumpON               HIGH                                                                                // 气泵打开 开启充气
#define airPumpOFF              !airPumpON                                                                          // 气泵关闭 停止充气
#define airValveON              HIGH                                                                                // 气阀打开 开启放气
#define airValveOFF             !airValveON                                                                         // 气阀关闭 停止放气

#define setRunVal               150                                                                                 // 设置运行充气参数【可调 0 ~ 255】

// 系统参数 ------------------------------------------------------------------------------------------------------------------
uint8_t systemState = 0x00;                                                                                         // 系统运行状态

unsigned long runTime = 0;                                                                                          // 运行时间

#define runKeepTime             (20 * 1000UL)                                                                       // 运行保活时间【可调 毫秒】
#define delayTime               (1 * 30 * 1000UL)                                                                   // 运行延迟时间

void setup() {
  Serial.begin(9600);                                                                                               // 打开串口，波特率9600
  // 音乐模块 --------------
  mySerial.begin(9600);                                                                                             // 智能语音控制模块
  // 震动马达 --------------
  pinMode(motorPinA, OUTPUT);                                                                                       // 针脚输出
  pinMode(motorPinB, OUTPUT);                                                                                       // 针脚输出
  digitalWrite(motorPinA, LOW);                                                                                     // 初始化关闭
  digitalWrite(motorPinB, LOW);                                                                                     // 初始化关闭
  // 气泵 -----------------
  pinMode(airPumpPin, OUTPUT);                                                                                      // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                                                     // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                                                             // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                                                                            // 初始状态 气阀打开 开启放气
  // 打印提示 --------------
  Serial.println("Started successfully");
}

void loop() {
  // 获取语音指令 ----------------------------------------------------
  if (mySerial.available() > 0) {                                                                                   // 检测软串口是否有数据
    byte val = mySerial.read();                                                                                     // 读出串数据
    Serial.print("Voice Command: ");
    Serial.println(val);                                                                                            // 从主串口打开出来

    if (systemState != 0x01) {
      systemState = 0x01;
      runTime = millis();

      // 震动开启 --------------------
      digitalWrite(motorPinA, HIGH);
      digitalWrite(motorPinB, HIGH);
      // 开始充气 --------------------
      //digitalWrite(airPumpPin, airPumpON);                   // 气泵开启 开启充气
      analogWrite(airPumpPin, setRunVal);
      digitalWrite(airValvePin, airValveOFF);                  // 气阀关闭 停止放气
    }
  }

  // 不同状态运行 ----------------------------------------------------
  switch (systemState) {
    case 0x00: {                    // 默认无响应

      } break;
    case 0x01: {                    // 充气二十秒
        if (millis() - runTime >= runKeepTime) {
          runTime = millis();
          systemState = 0x02;
          // 关闭震动 -----------------------
          digitalWrite(motorPinA, LOW);
          digitalWrite(motorPinB, LOW);
          // 停止充气 -----------------------
          digitalWrite(airPumpPin, airPumpOFF);               // 气泵关闭 停止充气
        }
      } break;
    case 0x02: {                    // 等待一分钟
        if (millis() - runTime >= delayTime) {
          runTime = millis();
          systemState = 0x00;

          // 开始放气 ------------------
          digitalWrite(airValvePin, airValveON);              // 气阀开启 开启放气
        }
      } break;
    default: break;
  }

  // loop回括号
}
