
/**
   气泵效果08
   通过超声波距离传感器进行变化

   视频链接：
   https://www.bilibili.com/video/BV1hr42187fd

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// -------------------------------- 气泵 ------------------------------------------------------
#define airPumpCont               4                                                    // 气泵数量
int airPumpPin[airPumpCont] = {                                                        // 控制气泵充气 引脚
    2, 4, 6, 8
};
int airValvePin[airPumpCont] = {                                                       // 控制气阀放气 引脚
    3, 5, 7, 9                                                                         //A0 = 14, A1 = 15, A2 = 16, A3 = 17
};
#define airPumpON                 HIGH                                                 // 气泵打开 开启充气
#define airPumpOFF                !airPumpON                                           // 气泵关闭 停止充气
#define airValveON                HIGH                                                 // 气阀打开 开启放气
#define airValveOFF               !airValveON                                          // 气阀关闭 停止放气
// --------------------------------------------------------------------------------------------
// -------------------------------- 距离 ------------------------------------------------------
#define EchoPin                   10                                                   // 读取距离传感器模拟量引脚
#define TrigPin                   11                                                   // 读取距离传感器模拟量引脚

#define collectionIntervalCount   2                                                    // 间隔差 次数
#define distanceIntervalValue     3                                                    // 距离间隔差 cm

#define distanceMinVal            10                                                   // 超声波的检测范围最小的距离（最小不应超过5）
#define distanceMaxVal            100                                                  // 超声波的检测范围最大的距离（最大不应超过400）

float newDistance =               0;                                                   // 新数据 默认值
// --------------------------------------------------------------------------------------------
// -------------------------------- 参数 ------------------------------------------------------
#define runIntervalTime           300                                                  // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long runTime =           0;                                                   // 运行记录时间
// -------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(TrigPin, OUTPUT);                                                            // 设置引脚输出
  pinMode(EchoPin, INPUT);                                                             // 设置引脚输入
  for (int i = 0; i < airPumpCont; i++) {
    pinMode(airPumpPin[i], OUTPUT);                                                    // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                                                   // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                                           // 初始状态 关闭气泵 停止充气
    digitalWrite(airValvePin[i], airValveON);                                          // 初始状态 气阀打开 开启放气
  }
  Serial.println(F("Start successfully"));
}

void loop() {
  if (millis() - runTime >= runIntervalTime) {                                         // 判断时间是否满足
    runTime = millis();                                                                // 记录时间
    newDistance = getDistanceData();
    Serial.println(newDistance);
    if (newDistance < 50) {
      for (int i = 0; i < airPumpCont; i++) {                                          // 循环设置4个气泵
        digitalWrite(airPumpPin[i], airPumpON);                                        // 开启充气
        digitalWrite(airValvePin[i], airValveOFF);                                     // 关闭放气
      }
    } else {
      for (int i = 0; i < airPumpCont; i++) {                                          // 循环设置4个气泵
        digitalWrite(airPumpPin[i], airPumpOFF);                                       // 关闭充气
        digitalWrite(airValvePin[i], airValveON);                                      // 开启放气
      }
    }
  }
  
  // loop回括号
}

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
