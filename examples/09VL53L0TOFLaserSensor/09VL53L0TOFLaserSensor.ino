/**
   气泵效果09
   通过激光测距VL53L0模块进行变化

   视频链接：
   https://www.bilibili.com/video/BV1Wh4y1U76Z

   库文件：
   https://github.com/adafruit/Adafruit_VL53L0X
   
   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/
#include "Adafruit_VL53L0X.h"
// -------------------------------- 气泵 ----------------------------------
#define airPumpPin                  5                                            // 控制气泵充气 引脚
#define airValvePin                 6                                            // 控制气阀放气 引脚
#define airPumpON                   HIGH                                         // 气泵打开 开启充气
#define airPumpOFF                  !airPumpON                                   // 气泵关闭 停止充气
#define airValveON                  HIGH                                         // 气阀打开 开启放气
#define airValveOFF                 !airValveON                                  // 气阀关闭 停止放气
#define inflationTime               40                                           // 充气的最大时间（1秒 = 1000毫秒）  *可调*
unsigned long inflatableMarket =    0;                                           // 记录已充气的时长
unsigned long setInflationTime =    0;                                           // 设定充气的时长

// -----------------------------------------------------------------------
// -------------------------------- 距离 ----------------------------------
#define distanceIntervalValue       (20  * 10)                                   // 距离间隔差 cm

Adafruit_VL53L0X lox = Adafruit_VL53L0X();                                       // 实例化对象
VL53L0X_RangingMeasurementData_t measure;                                        // 建立数据接收结构体
// -----------------------------------------------------------------------
// -------------------------------- 参数 ----------------------------------
#define getIntervalTime             300                                          // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long getDataTime =         0;                                           // 运行记录时间
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(airPumpPin, OUTPUT);                                                   // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                  // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                          // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                                         // 初始状态 气阀打开 开启放气
  // ------------------------- 激光传感器初始化 -----------------------------
  if (!lox.begin(0x29)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1)
      ;
  }
  // -----------------------------------------------------------------------
  Serial.println(F("Start successfully"));
}

void loop() {
  if (millis() - getDataTime >= getIntervalTime) {                               // 判断时间是否满足
    getDataTime = millis();                                                      // 记录时间

    int tempVal = getDistanceData();
    Serial.print("Distance ");
    Serial.print(" : ");
    Serial.println(tempVal);

    if (tempVal < distanceIntervalValue) {
      digitalWrite(airPumpPin, airPumpON);                                       // 开始充气
      digitalWrite(airValvePin, airValveOFF);                                    // 停止放气
    } else {
      digitalWrite(airPumpPin, airPumpOFF);                                      // 初始状态 关闭气泵 停止充气
      digitalWrite(airValvePin, airValveON);                                     // 初始状态 气阀打开 开启放气
    }
  }
  // loop回括号
}

// 获取激光距离数据 返回int数据类型 距离值 // 注意：激光模块最大测试距离为  35mm~1000cm
int getDistanceData() {
  static int tempDistance = 0;                                                   // 临时存储变量
  lox.rangingTest(&measure, false);                                              //传入“true”以获得调试数据打印输出！
  if (measure.RangeStatus != 4) {                                                //相位故障的数据不正确
    tempDistance = measure.RangeMilliMeter;
  }
  else {
    tempDistance = 1000;
  }
  return tempDistance;                                                           // 输出反馈距离
}
