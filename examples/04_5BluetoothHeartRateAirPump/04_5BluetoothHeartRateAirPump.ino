/**
   esp32蓝牙控制心率气泵

   视频链接：
   https://www.bilibili.com/video/BV1152SYzE4m

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

#include <Wire.h>                                       // I2C读写库
#include "MAX30105.h"                                   // 心率传感器库
#include "heartRate.h"                                  // 心率
#include "SoftwareSerial.h"                             // 软串口

// ---------------------------- 心率模块 -------------------------------
MAX30105 particleSensor;                                // 实例化
long lastBeat = 0;                                      // 最后记录心率的时间
float beatsPerMinute;                                   // 心率值
#define maxHeartRate  100                               // 设定超出最大心率值
#define heartRateActivationTime  5000                   // 设定规定时间内如果心率过高则保持高心率状态
unsigned long heartRateTime = 0;                        // 记录阶段心率时间
bool heartRateFlga = false;                             // 心率快慢状态 false为稳定 true为高
// --------------------------------------------------------------------
// ---------------------------- 气阀气泵 -------------------------------
#define airPumpPin    5                                 // 控制气泵充气 引脚
#define airValvePin   6                                 // 控制气阀放气 引脚

#define airPumpON     HIGH                              // 气泵打开 开启充气
#define airPumpOFF    !airPumpON                        // 气泵关闭 停止充气
#define airValveON     HIGH                             // 气阀打开 开启放气
#define airValveOFF   !airValveON                       // 气阀关闭 停止放气
// --------------------------------------------------------------------
// ---------------------------- 蓝牙模块 -------------------------------
#define UnoRx          3                                // Arduino 数字5 连接 蓝牙TX
#define UnoTx          4                                // Arduino 数字4 连接 蓝牙RX
#define BTBaudRate     9600                             // 蓝牙连接波特率
SoftwareSerial BTSerial(UnoRx, UnoTx);                  // 定义软串口
#define BTSendIntervalTime 500                          // 蓝牙发送间隔时间（1秒 = 1000毫秒）
unsigned long BTSendTime = 0;                           // 蓝牙发送阶段时间
String sendState = "off";                               // 默认状态
// --------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  // --------------- 气泵 ---------------
  pinMode(airPumpPin, OUTPUT);                          // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                         // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                 // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                // 初始状态 气阀打开 开启放气
  // -----------------------------------
  // --------------- 心率 ---------------
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {    // 默认使用I2C，400KHZ频率
    while (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
      Serial.println(F("MAX30105 was not found. Please check wiring/power. "));
    }
  }
  particleSensor.setup();                               // 使用默认设置配置传感器
  particleSensor.setPulseAmplitudeRed(0x0A);            // 将红色LED拉低，表示传感器正在运行
  // -----------------------------------
  // ------------ 蓝牙模块 ----------------
  BTSerial.begin(BTBaudRate);                           // 设置蓝牙连接
  // --------------------------------
  Serial.println(F("Start successfully"));              // 启动提示
}

/**
  接受蓝牙指令
  begin ：开始充气
  stop  ：停止状态
  off   ：关闭
*/

void loop() {
  // 获取心率
  getHeartRateData();                                   // 采集数据

  // 接收蓝牙指令
  if (BTSerial.available() > 0) {                       // 检测软串口是否有数据
    String val = "";
    while (BTSerial.available() > 0) {                  // 循环获取数据
      val += char(BTSerial.read());                     // 读出串数据
      delay(2);
    }
    Serial.print("Receive Data: ");
    Serial.println(val);                                // 从主串口打开出来
    if (val[0] == 'b' || val[0] == 'B') {               // 判断是否为开始充气
      sendState = "begin";
      digitalWrite(airPumpPin, airPumpON);              // 气泵开启 开启充气
      digitalWrite(airValvePin, airValveOFF);           // 气阀关闭 停止放气

    } else if (val[0] == 's' || val[0] == 'S') {        // 判断是否为停止充气
      sendState = "stop";
      digitalWrite(airPumpPin, airPumpOFF);             // 气泵关闭 停止充气
      digitalWrite(airValvePin, airValveOFF);           // 气阀开启 开启放气

    } else if (val[0] == 'o' || val[0] == 'O') {        // 判断是否为关闭
      sendState = "off";
      digitalWrite(airPumpPin, airPumpOFF);             // 气泵关闭 停止充气
      digitalWrite(airValvePin, airValveON);            // 气阀开启 开启放气

    } else {                                            // 其它
    }
  }

  // 间隔发送数据给到手机端查看
  if (millis() - BTSendTime >= BTSendIntervalTime) {    // 判断间隔时间
    BTSendTime = millis();                              // 记录阶段时间
    BTSerial.print("BPM: ");                            // 发送数据
    BTSerial.println(beatsPerMinute);                   // 发送数据
    BTSerial.print("State: ");                          // 发送数据
    BTSerial.println(sendState);                        // 发送数据
    BTSerial.println();
  }

  // loop回括号
}

// 获取心率传感器数据
void getHeartRateData() {
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue) == true) {                  // 感应到心率
    long delta = millis() - lastBeat;                   // 间隔时间
    lastBeat = millis();                                // 记录阶段时间
    beatsPerMinute = 60 / (delta / 1000.0);             // 转换
  }
  if (irValue > 50000) {                                // 如果采集量在符合的标准内
    Serial.print(F("BPM="));
    Serial.println(beatsPerMinute);
  } else {
    beatsPerMinute = 0;
  }
}
