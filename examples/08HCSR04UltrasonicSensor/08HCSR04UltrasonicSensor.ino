
/**
   气泵效果08
   通过超声波距离传感器进行变化

   视频链接：
   https://www.bilibili.com/video/BV19u4y1Q79C

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// -------------------------------- 气泵 ----------------------------------
#define airPumpPin                  5                                                  // 控制气泵充气 引脚
#define airValvePin                 6                                                  // 控制气阀放气 引脚
#define airPumpON                   HIGH                                               // 气泵打开 开启充气
#define airPumpOFF                  !airPumpON                                         // 气泵关闭 停止充气
#define airValveON                  HIGH                                               // 气阀打开 开启放气
#define airValveOFF                 !airValveON                                        // 气阀关闭 停止放气
// -----------------------------------------------------------------------
// -------------------------------- 距离 ----------------------------------
#define EchoPin                     3                                                  // 读取距离传感器模拟量引脚
#define TrigPin                     4                                                  // 读取距离传感器模拟量引脚
//#define runIntervalTime           300                                                // 运行采集间隔时间
#define collectionIntervalCount     2                                                  // 间隔差 次数
#define distanceIntervalValue       3                                                  // 距离间隔差 cm

#define distanceMinVal              10                                                 // 超声波的检测范围最小的距离（最小不应超过5）
#define distanceMaxVal              100                                                // 超声波的检测范围最大的距离（最大不应超过400）

float newDistance =                 0;                                                 // 新数据 默认值
// -----------------------------------------------------------------------
// -------------------------------- 参数 ----------------------------------
#define runIntervalTime             300                                                // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long runTime =             0;                                                 // 运行记录时间
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(airPumpPin, OUTPUT);                                                         // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                        // 设置引脚输出
  pinMode(TrigPin, OUTPUT);                                                            // 设置引脚输出
  pinMode(EchoPin, INPUT);                                                             // 设置引脚输入
  digitalWrite(airPumpPin, airPumpOFF);                                                // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                                               // 初始状态 气阀打开 开启放气
  Serial.println(F("Start successfully"));
}

void loop() {
  if (millis() - runTime >= runIntervalTime) {                                         // 判断时间是否满足
    runTime = millis();                                                                // 记录时间
    newDistance = getDistanceData();
    Serial.println(newDistance);
    if (newDistance < 50) {
      digitalWrite(airPumpPin, airPumpON);                                             // 开启充气
      digitalWrite(airValvePin, airValveOFF);                                          // 关闭放气
    } else {
      digitalWrite(airPumpPin, airPumpOFF);                                            // 关闭充气
      digitalWrite(airValvePin, airValveON);                                           // 开启放气
    }
    /*
      srFilteredData();                                                                // 获取距离变化
      if (abs(olddistanceValue - newdistanceValue) > tolerance) {                      //新旧变化超过容差
      setInflationTime = map(newdistanceValue, distanceMinVal, distanceMaxVal, inflationTime * 100, 0) * 10;
      if (inflatableMarket < setInflationTime) {
        pumpFlag = true;                                                               //气泵状态为充气状态
        digitalWrite(airPumpPin, airPumpON);                                           // 开启充气
        digitalWrite(airValvePin, airValveOFF);                                        // 关闭放气
      } else {
        pumpFlag = false;                                                              //气泵状态为放气状态
        digitalWrite(airPumpPin, airPumpOFF);                                          // 关闭充气
        digitalWrite(airValvePin, airValveON);                                         // 开启放气
      }
      olddistanceValue = newdistanceValue;                                             //更新olddistanceValue值
      } else if (newdistanceValue >= distanceMaxVal) {
      pumpFlag = false;                                                                //气泵状态为放气状态
      digitalWrite(airValvePin, airValveON);                                           // 开启放气
      }

      if (pumpFlag == true) {                                                          //如果当前气泵的状态为充气状态
      if (inflatableMarket > setInflationTime) {                                       //是否大于设定时间
        digitalWrite(airPumpPin, airPumpOFF);                                          // 关闭充气
      } else {
        inflatableMarket += getIntervalTime;                                           //增加记录时间
      }
      } else {                                                                         //否则当前气泵的状态为放气状态
      if (inflatableMarket < setInflationTime + 5) {                                   //是否小于设定时间
        digitalWrite(airPumpPin, airValveOFF);                                         // 关闭放气
      } else {
        inflatableMarket -= getIntervalTime;                                           //减少记录时间
      }
      }
    */
  }
  // loop回括号
}
/*
// 对距离数据进行滤波
void srFilteredData() {
  if (millis() - runTime >= runIntervalTime) {                                         // 判断时间间隔
    runTime = millis();                                                                // 处理完成后，重置记录阶段时间
    newDistance = constrain(getDistanceData(), distanceMinVal, distanceMaxVal) * 1.0;  // 将检测到数据存储于新距离存储
    if (abs(newDistance - oldDistance) >= distanceIntervalValue) {                     // 如果检测数据值大于上一次数据值间隔差
      if (++collectionCount >= collectionIntervalCount) {                              // 超过五次则重置
        oldDistance = newDistance;                                                     // 更新数据值
        collectionCount = 0;                                                           // 运行次数重置
      }
    } else {                                                                           // 如果数据变化在固定范围内，则直接更新
      oldDistance = newDistance;                                                       // 更新数据值
    }
    newdistanceValue = oldDistance;
    Serial.print("Distance ");
    Serial.print(" : ");
    Serial.print(oldDistance);
    Serial.print("cm---");
    Serial.print("设定时间(ms): ");
    Serial.print(setInflationTime);
    Serial.print("---------当前时间 ");
    Serial.println(inflatableMarket);
  }
}
*/

// 获取超声波距离数据 返回float数据类型 距离值 // 注意：超声波模块最大测试距离为  2cm~450cm
float getDistanceData() {
  float tempDistance;                                                                  // 临时存储变量
  digitalWrite(TrigPin, LOW);                                                          // 设置引脚低电平
  delayMicroseconds(2);                                                                // 延迟2微妙
  digitalWrite(TrigPin, HIGH);                                                         // 设置引脚高电平
  delayMicroseconds(10);                                                               // 延迟等待10微妙
  digitalWrite(TrigPin, LOW);                                                          // 设置引脚低电平
  tempDistance = pulseIn(EchoPin, HIGH) / 58.0;                                        // 公式计算距离
  tempDistance = (int(tempDistance * 100.0)) / 100.0;                                  // 转换为cm
  if (tempDistance < 0) {                                                              // 判断检测距离数据小于0则
    tempDistance = 400;                                                                // 等于400
  } else if (tempDistance > 400) {                                                     // 判断检测距离数据大于400则等于400
    tempDistance = 400;                                                                // 等于400
  }
  return tempDistance;                                                                 // 输出反馈距离
}
