/**
  需要esp32
  蓝牙控制气泵充气、震动模块震动

  视频链接：
  https://www.bilibili.com/video/BV1iGeweGEa9

  库文件：
  https://github.com/hen1227/bluetooth-serial
  https://github.com/queuetue/Q2-HX711-Arduino-Library

  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

#include "BluetoothSerial.h"                                                                        // 蓝牙库
#include <Q2HX711.h>                                                                                // 读取ADC数据库

// 蓝牙 --------------------------------------------------------------------------------------------------------
BluetoothSerial SerialBT;                                                                           // 实例化蓝牙对象
String BTName = "ESP32 BT";                                                                         // 设置蓝牙名字【可调】

// HX710 -------------------------------------------------------------------------------------------------------
#define OUT_A                   14                                                                  // 设置out引脚
#define SCK_A                   27                                                                  // 设置sck引脚
Q2HX711 HX710_A(OUT_A, SCK_A);                                                                      // 实例化对象

#define OUT_B                   26                                                                  // 设置out引脚
#define SCK_B                   25                                                                  // 设置sck引脚
Q2HX711 HX710_B(OUT_B, SCK_B);                                                                      // 实例化对象

// 集成气泵 -----------------------------------------------------------------------------------------------------
#define airPumpCont             2                                                                   // 气泵数量
int airPumpPin[airPumpCont] = {                                                                     // 控制气泵充气 引脚
  16, 18
};
int airValvePin[airPumpCont] = {                                                                    // 控制气阀放气 引脚
  17, 19
};
#define airValveON              HIGH                                                                // 气阀打开 开启放气
#define airValveOFF             !airValveON                                                         // 气阀关闭 停止放气

// 震动模块 -----------------------------------------------------------------------------------------------------
#define motorCount              2                                                                   // 震动模块数量
int motorPin[motorCount] = {                                                                        // 控制引脚
  33, 32
};
#define pwmFreq                 5000                                                                // 定义PWM频率
#define pwmResolution           8                                                                   // 定义PWM分辨率（8位）

// 系统参数 -----------------------------------------------------------------------------------------------------
#define sendInterval            (1 * 1000UL)                                                        // 发送数据间隔时间【可调 毫秒】
unsigned long sendTime = 0;                                                                         // 记录阶段发送时间

#define minAirPump              120                                                                 // 最小气泵充气量【可调 0 ~ 255】
#define maxAirPump              255                                                                 // 最大气泵充气量【可调 0 ~ 255】

#define minMotor                110                                                                 // 最小震动参数【可调 0 ~ 255】
#define maxMotor                255                                                                 // 最小震动参数【可调 0 ~ 255】

void setup() {
  Serial.begin(115200);                                                                             // 设置波特率115200
  // 气阀设置 -----------------
  for (int i = 0; i < airPumpCont; i++) {                                                           // for循环设置
    pinMode(airValvePin[i], OUTPUT);                                                                // 设置引脚输出
    digitalWrite(airValvePin[i], airValveOFF);                                                      // 初始状态 气阀关闭 停止放气
  }
  // 震动模块 -----------------
  ledcSetup(0, pwmFreq, pwmResolution);                                                             // 配置指定引脚为PWM输出
  ledcAttachPin(motorPin[0], 0);                                                                    // 将指定引脚附加到PWM通道
  ledcWrite(0, 0);
  ledcSetup(1, pwmFreq, pwmResolution);                                                             // 配置指定引脚为PWM输出
  ledcAttachPin(motorPin[1], 1);                                                                    // 将指定引脚附加到PWM通道
  ledcWrite(1, 0);
  // 气泵设置 -----------------
  ledcSetup(2, pwmFreq, pwmResolution);                                                             // 配置指定引脚为PWM输出
  ledcAttachPin(airPumpPin[0], 2);                                                                  // 将指定引脚附加到PWM通道
  ledcWrite(2, 0);
  ledcSetup(3, pwmFreq, pwmResolution);                                                             // 配置指定引脚为PWM输出
  ledcAttachPin(airPumpPin[1], 3);                                                                  // 将指定引脚附加到PWM通道
  ledcWrite(3, 0);
  // 蓝牙设置 -----------------
  SerialBT.begin(BTName);                                                                           // 蓝牙初始化
  // 打印提示 -----------------
  Serial.println("Started successfully");                                                           // 打印提示
}

/**
  控制蓝牙指令集
  off  （关闭全部设备【停止充气、停止放气、停止震动】）

  控制充气（气泵  Air pump）
  P,ID,100  （ID 为0则是控制全部  1则控制第一组  2则控制第二组    充气速度 0 ~ 100）

  控制放气（气阀  Air valve）
  V,ID,1  （ID 为0则是控制全部    1则控制第一组  2则控制第二组    开启放气 1 关闭放气 0）

  控制震动马达（马达  Motor）
  M,ID,100  （ID 为0则是控制全部  1则控制第一组  2则控制第二组    震动强弱 0 ~ 100）
*/


void loop() {
  // 接收蓝牙数据 -------------------------------
  if (SerialBT.available() > 0) {                                                                   // 如果蓝牙串口有数据
    String BTVal = "";                                                                              // 创建临时变量存储
    while (SerialBT.available() > 0) {                                                              // 判断
      BTVal += char(SerialBT.read());                                                               // 读取数据累加
      delay(2);                                                                                     // 延迟2毫秒
    }
    Serial.print("Receiver Data: ");                                                                // 打印提示
    Serial.println(BTVal);                                                                          // 打印提示
    // 数据指令对应操作 ------------
    BTVal.toLowerCase();                                                                            // 改为小写
    if (BTVal == "off" || BTVal.indexOf("off") >= 0) {      // 全部关闭
      // 关闭充气
      ledcWrite(2, 0);                                                                              // 设置通道为0
      ledcWrite(3, 0);                                                                              // 设置通道为0
      // 关闭放气
      for (int i = 0; i < airPumpCont; i++) {                                                       // for循环设置
        digitalWrite(airValvePin[i], airValveOFF);                                                  // 气阀关闭 停止放气
      }
      // 关闭震动
      ledcWrite(0, 0);                                                                              // 设置通道为0
      ledcWrite(1, 0);                                                                              // 设置通道为0
      // 打印提示
      Serial.println("OFF  ok\r\n");                                                                // 打印提示
    }
    else if (BTVal[0] == 'p') {                             // 控制充气
      if (BTVal[2] == '0') {              // 同时设置
        int runData = BTVal.substring(4, BTVal.length()).toInt();                                   // 获取参数数据
        if (runData > 100) {              // 限制
          runData = 100;
        }
        if (runData <= 0) {               // 关闭
          ledcWrite(2, 0);                                                                          // 设置通道为0
          ledcWrite(3, 0);                                                                          // 设置通道为0
        } else {                          // 设置
          runData = map(runData, 1, 100, minAirPump, maxAirPump);                                   // 得到设置参数
          ledcWrite(2, runData);                                                                    // 设置通道参数
          ledcWrite(3, runData);                                                                    // 设置通道参数
        }
      } else if (BTVal[2] == '1') {       // 设置第一个
        int runData = BTVal.substring(4, BTVal.length()).toInt();                                   // 获取参数数据
        if (runData > 100) {              // 限制
          runData = 100;
        }
        if (runData <= 0) {               // 关闭
          ledcWrite(2, 0);                                                                          // 设置通道为0
        } else {                          // 设置
          runData = map(runData, 1, 100, minAirPump, maxAirPump);                                   // 得到设置参数
          ledcWrite(2, runData);                                                                    // 设置通道参数
        }
      } else if (BTVal[2] == '2') {       // 设置第二个
        int runData = BTVal.substring(4, BTVal.length()).toInt();                                   // 获取参数数据
        if (runData > 100) {              // 限制
          runData = 100;
        }
        if (runData <= 0) {               // 关闭
          ledcWrite(3, 0);                                                                          // 设置通道为0
        } else {                          // 设置
          runData = map(runData, 1, 100, minAirPump, maxAirPump);                                   // 得到设置参数
          ledcWrite(3, runData);                                                                    // 设置通道参数
        }
      } else {
        Serial.println("ID exceeds!\r\n");
      }
    }
    else if (BTVal[0] == 'v') {                             // 控制放气
      if (BTVal[2] == '0') {              // 同时设置
        if (BTVal[4] == '1') {            // 打开
          digitalWrite(airValvePin[0], airValveON);         // 气阀打开
          digitalWrite(airValvePin[1], airValveON);         // 气阀打开
        } else {                          // 关闭
          digitalWrite(airValvePin[0], airValveOFF);        // 气阀关闭
          digitalWrite(airValvePin[1], airValveOFF);        // 气阀关闭
        }
      } else if (BTVal[2] == '1') {       // 设置第一个
        if (BTVal[4] == '1') {            // 打开
          digitalWrite(airValvePin[0], airValveON);         // 气阀打开
        } else {                          // 关闭
          digitalWrite(airValvePin[0], airValveOFF);        // 气阀关闭
        }
      } else if (BTVal[2] == '2') {       // 设置第二个
        if (BTVal[4] == '1') {            // 打开
          digitalWrite(airValvePin[1], airValveON);         // 气阀打开
        } else {                          // 关闭
          digitalWrite(airValvePin[1], airValveOFF);        // 气阀关闭
        }
      } else {
        Serial.println("ID exceeds!\r\n");
      }
    }
    else if (BTVal[0] == 'm') {                             // 控制震动
      if (BTVal[2] == '0') {              // 同时设置
        int runData = BTVal.substring(4, BTVal.length()).toInt();                                   // 获取参数数据
        if (runData > 100) {              // 限制
          runData = 100;
        }
        if (runData <= 0) {               // 关闭
          ledcWrite(0, 0);                                                                          // 设置通道为0
          ledcWrite(1, 0);                                                                          // 设置通道为0
        } else {                          // 设置
          runData = map(runData, 1, 100, minMotor, maxMotor);                                       // 得到设置参数
          ledcWrite(0, runData);                                                                    // 设置通道参数
          ledcWrite(1, runData);                                                                    // 设置通道参数
        }
      } else if (BTVal[2] == '1') {       // 设置第一个
        int runData = BTVal.substring(4, BTVal.length()).toInt();                                   // 获取参数数据
        if (runData > 100) {              // 限制
          runData = 100;
        }
        if (runData <= 0) {               // 关闭
          ledcWrite(0, 0);                                                                          // 设置通道为0
        } else {                          // 设置
          runData = map(runData, 1, 100, minMotor, maxMotor);                                       // 得到设置参数
          ledcWrite(0, runData);                                                                    // 设置通道参数
        }
      } else if (BTVal[2] == '2') {       // 设置第二个
        int runData = BTVal.substring(4, BTVal.length()).toInt();                                   // 获取参数数据
        if (runData > 100) {              // 限制
          runData = 100;
        }
        if (runData <= 0) {               // 关闭
          ledcWrite(1, 0);                                                                          // 设置通道为0
        } else {                          // 设置
          runData = map(runData, 1, 100, minMotor, maxMotor);                                       // 得到设置参数
          ledcWrite(1, runData);                                                                    // 设置通道参数
        }
      } else {
        Serial.println("ID exceeds!\r\n");
      }
    }
    else {
      Serial.println("ERROR\r\n");                                                                  // 打印提示
    }
  }

  // 间隔蓝牙发送 -------------------------------
  if (millis() - sendTime >= sendInterval) {
    sendTime = millis();
    // 采集气压模拟量
    int AirPressureData_A = HX710_A.read() / 1000;                                                  // 读取数据
    int AirPressureData_B = HX710_B.read() / 1000;                                                  // 读取数据
    // 数据发送
    SerialBT.println("1," + String(AirPressureData_A) + ",2," + String(AirPressureData_B));         // 发送数据
  }

  // loop回括号
}
