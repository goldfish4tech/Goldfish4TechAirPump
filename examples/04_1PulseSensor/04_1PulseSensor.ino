/**
   集成化气泵04_1——心率存在感应1
   有心率充气，没心率放气

   视频链接：
   https://www.bilibili.com/video/BV1xw4m1R7TN

   库文件：
   https://github.com/WorldFamousElectronics/PulseSensorPlayground

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

#define USE_ARDUINO_INTERRUPTS    true                                                // 设置低级别中断，以获得最准确的BPM数学计算。
#include <PulseSensorPlayground.h>                                                    // 包含 PulseSensorPlayground 库。 

// 心率传感器 -----------------------------------------------------------------------------------------------
#define PulseWire             A0                                                      // PulseSensor 紫色线连接到 ANALOG PIN 0
int Threshold = 550;                                                                  // 确定要 "计为一拍" 的信号和要忽略的信号。
PulseSensorPlayground pulseSensor;                                                    // 创建名为 "pulseSensor" 的 PulseSensorPlayground 对象的实例
// ---------------------------------------------------------------------------------------------------------

// 气泵 -----------------------------------------------------------------------------------------------------
#define airPumpPin            5                                                       // 控制气泵充气 引脚
#define airValvePin           6                                                       // 控制气阀放气 引脚
#define airPumpON             HIGH                                                    // 气泵打开 开启充气
#define airPumpOFF            !airPumpON                                              // 气泵关闭 停止充气
#define airValveON            HIGH                                                    // 气阀打开 开启放气
#define airValveOFF           !airValveON                                             // 气阀关闭 停止放气                                                      
// ---------------------------------------------------------------------------------------------------------

// 参数设置 -------------------------------------------------------------------------------------------------
int heartRateData = 0;                                                                // 心率数据

#define judgmentInterval      3000                                                    // 判断间隔时间【毫秒 设置如果没有心率变化的超时时间】
unsigned long judgmentTime = 0;                                                       // 判断时间

// ---------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                                 // 用于串行监视器

  // 气泵设置 -------------------
  pinMode(airPumpPin, OUTPUT);                                                        // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                       // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                               // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveOFF);                                             // 初始状态 气阀关闭 关闭放气

  // 心率设置 -------------------
  pulseSensor.analogInput(PulseWire);                                                 // 配置 PulseSensor 对象，通过将我们的变量分配给它来完成。
  pulseSensor.setThreshold(Threshold);
  if (pulseSensor.begin()) {                                                          // 再次检查 "pulseSensor" 对象是否已创建并 "开始" 观察信号。
    Serial.println("Started successfully");                                           // 这在Arduino上电时打印一次，或在Arduino复位时打印一次。
  }
}

void loop() {
  // 获取心率数据 -------------------------------------------
  heartRateData = pulseSensor.getBeatsPerMinute();                                    // 调用我们的 pulseSensor 对象上的函数，将BPM作为 "int" 返回。
  if (pulseSensor.sawStartOfBeat()) {                                                 // 不断测试是否 "发生了一次心跳"。
    Serial.print("BPM: ");                                                            // 打印短语 "BPM: "
    Serial.println(heartRateData);                                                    // 打印 myBPM 中的值。

    // 有心率的情况下更新时间，设置气泵开启运行
    judgmentTime = millis();

    digitalWrite(airPumpPin, airPumpON);                                              // 开启充气
    digitalWrite(airValvePin, airValveOFF);                                           // 关闭放气
  }

  // 如果更新时间超出，设置气泵关闭 -----------------------------
  if (millis() - judgmentTime >= judgmentInterval) {                                  // 判断时间是否超出
    judgmentTime = millis();                                                          // 记录当前时间

    // 设置开启放气
    digitalWrite(airPumpPin, airPumpOFF);                                             // 停止充气
    digitalWrite(airValvePin, airValveON);                                            // 开启放气
  }

  // loop回括号
}
