
/**
   气泵效果
   通过称重传感器进行变化

   视频链接：
   https://www.bilibili.com/video/BV1tN411B7hZ

   库文件：
   https://github.com/olkal/HX711_ADC

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/
#include <HX711_ADC.h>
// -------------------------------- 气泵 ----------------------------------
#define airPumpPin                          5                                                                 // 控制气泵充气 引脚
#define airValvePin                         6                                                                 // 控制气阀放气 引脚
#define airPumpON                           HIGH                                                              // 气泵打开 开启充气
#define airPumpOFF                          !airPumpON                                                        // 气泵关闭 停止充气
#define airValveON                          HIGH                                                              // 气阀打开 开启放气
#define airValveOFF                         !airValveON                                                       // 气阀关闭 停止放气
#define inflationTime                       40                                                                // 充气的最大时间（1秒 = 1000毫秒）  *可调*
unsigned long inflatableMarket =            0;                                                                // 记录已充气的时长
unsigned long setInflationTime =            0;                                                                // 设定充气的时长

// -----------------------------------------------------------------------
// -------------------------------- 称重 ----------------------------------
#define HX711Interval                       500                                                               // 称重模块检测间隔时间 【单位：毫秒】
#define weightMin                           150                                                               // 称重重量间隔小值大于150则为有手机
#define weightMax                           500                                                               // 称重重量间隔大值并且小于500为正常状态
#define hx711dout                           3                                                                 // mcu > HX711 dout pin
#define hx711sck                            4                                                                 // mcu > HX711 sck pin
bool newDataReady =                         0;                                                                // 定义是否有数据变化的布尔值
float weight;                                                                                                 // 存储重量
HX711_ADC LoadCell(hx711dout, hx711sck);                                                                      // HX711构造函数：
unsigned long HX711Time =                   0;                                                                // 运行间隔时间


#define weightMinVal                        0                                                                 // 最小的称重数据
#define weightMaxVal                        800                                                               // 最大的称重数据
#define stableThreshold                     5                                                                 // 数据稳定的阈值
#define stableDuration                      2000                                                              // 数据稳定的持续时间（1秒 = 1000毫秒） *可调*
unsigned long startMillis =                 0;                                                                // 时间
int signalMax =                             0;                                                                // 存储最大值
int signalMin =                             4096;                                                             // 存储最小值
long runCount[2] =                          { 0, 0 };                                                         // 运行次数
long triangleValue =                        0;                                                                // 累加数
#define runMax                              3                                                                 // 判断是否开启持续次数
#define runMin                              5                                                                 // 判断是否关闭持续次数
#define runMin                              5                                                                 // 判断是否关闭持续次数
#define tolerance                           2                                                                 // newWeightValue 和 oldWeightValue 的相差大小
bool pumpFlag =                             false;                                                            //false:放气 ;true:充气
float newWeightValue =                      0;                                                                // 新的称重大小值
float oldWeightValue =                      0;                                                                // 上一次的称重大小值
// -------------------------------- 参数 ----------------------------------
#define getIntervalTime                     5                                                                 // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long getDataTime =                 0;                                                                // 运行记录时间
#define setPressureLevel                    1                                                                 // 设置最大称重值（大于则开始充气）
// -----------------------------------------------------------------------

void setup() {
  delay(2000);
  Serial.begin(9600);
  pinMode(airPumpPin, OUTPUT);                                                                                // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                                               // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                                                       // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                                                                      // 初始状态 气阀打开 开启放气
  //analogWrite(airPumpPin, 0);                                                                               // 初始状态 关闭气泵 停止充气
  //analogWrite(airValvePin, 255);                                                                            // 初始状态 气阀打开 开启放气
  // --------------- 称重传感器 --------------------
  Serial.println("系统启动中...");
  LoadCell.setCalFactor(401);                                                                                 // 设置校准系数（浮点）
  LoadCell.begin();
  Serial.println("初始化称重模块");
  LoadCell.tareNoDelay();
  Serial.println("初始化完成, 系统启动完成.");
  // ---------------------------------------------
  Serial.println(F("Start successfully"));
}

void loop() {

  if (LoadCell.update()) {                                                                                    // 检查新数据/启动下一个转换：
    newDataReady = true;                                                                                      // 如果有新的数据，则设定标志位 newDataReady 为 true
  }
  if (newDataReady) {                                                                                         // 如果 newDataReady 的标志位为 true，则进行以下操作
    if (millis() - HX711Time > HX711Interval) {                                                               // 如果当前时刻减去 HX711Time 大于 HX711Interval，则执行以下操作
      HX711Time = millis();                                                                                   // 将 HX711Time 设定为当前时刻
      weight = LoadCell.getData();                                                                            // 将称重数据保存到变量 weight 中
      if (weight < 0.5) {                                                                                     // 如果重量小于 0.5g，则将重量值设定为 0
        weight = 0;
      }
      Serial.print("重量：");
      Serial.print(weight);
      Serial.print("g------------");
      Serial.print("设定时间(ms): ");
      Serial.print(setInflationTime);
      Serial.print("---------当前时间 ");
      Serial.println(inflatableMarket);
    }
  }

  // 如果数据稳定，则输出"数据稳定"
  if (checkStability(weight)) {
    newWeightValue = weight;
    if (abs(oldWeightValue - newWeightValue) > tolerance) {  //新旧变化超过容差
      setInflationTime = map(newWeightValue, weightMinVal, weightMaxVal, 0, inflationTime * 10) * 100;
      if (oldWeightValue < newWeightValue) {
        pumpFlag = true;                                                                                      // 气泵状态为充气状态
        digitalWrite(airPumpPin, airPumpON);                                                                  // 开启充气
        digitalWrite(airValvePin, airValveOFF);                                                               // 关闭放气
      } else {
        pumpFlag = false;                                                                                     // 气泵状态为放气状态
        digitalWrite(airPumpPin, airPumpOFF);                                                                 // 关闭充气
        digitalWrite(airValvePin, airValveON);                                                                // 开启放气
      }
      oldWeightValue = newWeightValue;                                                                        // 更新oldWeightValue值
    } else if (newWeightValue <= weightMinVal) {
      pumpFlag = false;                                                                                       // 气泵状态为放气状态
      digitalWrite(airValvePin, airValveON);                                                                  // 开启放气
    }
  }
  if (millis() - getDataTime >= getIntervalTime) {                                                            // 判断时间是否满足
    getDataTime = millis();                                                                                   // 记录时间
    if (pumpFlag == true) {                                                                                   // 如果当前气泵的状态为充气状态
      if (inflatableMarket > setInflationTime) {                                                              // 是否大于设定时间
        digitalWrite(airPumpPin, airPumpOFF);                                                                 // 关闭充气
        digitalWrite(airPumpPin, airValveOFF);                                                                // 关闭放气
      } else {
        inflatableMarket += getIntervalTime;                                                                  // 增加记录时间
      }
    } else {                                                                                                  // 否则当前气泵的状态为放气状态
      if (inflatableMarket < setInflationTime + 5) {                                                          // 是否小于设定时间
        digitalWrite(airPumpPin, airPumpOFF);                                                                 // 关闭充气
        digitalWrite(airPumpPin, airValveOFF);                                                                // 关闭放气
      } else {
        inflatableMarket -= getIntervalTime;                                                                  // 减少记录时间
      }
    }
  }

  // loop回括号
}
// 判断的是否稳定的值
bool checkStability(int _value) {
  static int previousValue = 0;                                                                               // 上一个数据值
  static unsigned long stableStartTime = 0;                                                                   // 数据稳定的开始时间

  // 如果当前数据与上一个数据之间的差值小于阈值
  if (abs(_value - previousValue) < stableThreshold) {
    // 如果数据稳定时间尚未开始，则记录开始时间
    if (stableStartTime == 0) {
      stableStartTime = millis();
    }
    // 如果数据稳定时间超过指定的持续时间，则数据稳定
    if (millis() - stableStartTime >= stableDuration) {
      stableStartTime = 0;                                                                                    // 重置稳定开始时间
      previousValue = _value;                                                                                 // 更新上一个数据值
      return true;
    }
  } else {
    stableStartTime = 0;                                                                                      // 重置稳定开始时间
  }

  previousValue = _value;                                                                                     // 更新上一个数据值
  return false;
}
