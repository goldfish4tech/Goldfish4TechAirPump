/**
   气泵效果03
   通过声音传感器进行吹气

   视频链接：
   https://www.bilibili.com/video/BV1x8411D7kt

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// -------------------------------- 气泵 ----------------------------------
#define airPumpPin              5                       // 控制气泵充气 引脚
#define airValvePin             6                       // 控制气阀放气 引脚
#define airPumpON               HIGH                    // 气泵打开 开启充气
#define airPumpOFF              !airPumpON              // 气泵关闭 停止充气
#define airValveON              HIGH                    // 气阀打开 开启放气
#define airValveOFF             !airValveON             // 气阀关闭 停止放气
// -----------------------------------------------------------------------
// -------------------------------- 声音 ----------------------------------
#define soudPin                 A0                      // 读取声音传感器模拟量引脚
#define acquisitionTime         1000                    // 自适应采集时间（1秒 = 1000毫秒） *可调*
unsigned long startMillis =     0;                      // 时间
int signalMax =                 0;                      // 存储最大值
int signalMin =                 4096;                   // 存储最小值
long runCount[2] =              {0, 0};                 // 运行次数
long triangleValue =            0;                      // 累加数
#define runMax                  3                       // 判断是否开启持续次数
#define runMin                  5                       // 判断是否关闭持续次数
int airflowValue =              0;                      // 气流大小值
// -----------------------------------------------------------------------
// -------------------------------- 参数 ----------------------------------
#define getIntervalTime         5                       // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long getDataTime =     0;                      // 运行记录时间
#define setAirflowData          20                      // 设置最大气流值（大于则开始充气）
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(airPumpPin, OUTPUT);                          // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                         // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                 // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                // 初始状态 气阀打开 开启放气
  //analogWrite(airPumpPin, 0);                         // 初始状态 关闭气泵 停止充气
  //analogWrite(airValvePin, 255);                      // 初始状态 气阀打开 开启放气
  // --------------- 声音传感器 --------------------
  // 声音自适应获取当前 最小值 和 最大值
  startMillis = millis();                               // 记录当前时间
  while (millis() - startMillis < acquisitionTime) {    // 时间范围内自适应
    int sample = analogRead(soudPin);                   // 读取数据
    if (sample > signalMax) {                           // 获取最大值
      signalMax = sample;                               // 最大
    }
    else if (sample < signalMin) {                      // 获取最小值
      signalMin = sample;                               // 最小
    }
  }
  // ---------------------------------------------
  Serial.println(F("Start successfully"));
}

void loop() {
  if (millis() - getDataTime >= getIntervalTime) {      // 判断时间是否满足
    getDataTime = millis();                             // 记录时间
    getAirflowChange();                                 // 获取吹气变化
    if (airflowValue >= setAirflowData) {
      digitalWrite(airPumpPin, airPumpON);              // 开启充气
      digitalWrite(airValvePin, airValveOFF);           // 关闭放气
    } else {
      digitalWrite(airPumpPin, airPumpOFF);             // 关闭充气
      digitalWrite(airValvePin, airValveON);            // 开启放气
    }
  }

  // loop回括号
}

// 获取气流大小  经过声音传感器3次处理
void getAirflowChange() {
  // 采集原始值
  int sample = analogRead(soudPin);                     // 获取原始数据值
  /*
    // 原始波形打印
    Serial.print(sample);
    Serial.print(",");
    Serial.print(signalMax);
    Serial.print(",");
    Serial.print(signalMin);
    Serial.print(",");
  */
  // 处理波形
  if (sample > signalMax) {                             // 判断原始数据值大于采集最大值
    sample -= signalMax;                                // 原始值减最大值
  } else if (sample < signalMin) {                      // 判断原始数据值小于采集最大值
    sample = abs(signalMin - sample);                   // 得到间隔差值
  } else {                                              // 其它情况下
    sample = 0;                                         // 等于0
  }
  /*
    // 正值表
    Serial.print(sample);
    Serial.print(",");
  */
  // 倒三角
  if (sample > 10) {                                    // 判断数据是否大于10
    if (++runCount[0] > runMax) {                       // 次数记录自增 如果 大于设定次数后
      runCount[1] = 0;                                  // 重置值
      triangleValue += sample;                          // 累加值
    }
  } else if (sample == 0) {                             // 判断数据如果等于0
    if (++runCount[1] > runMin) {                       // 次数记录自增 如果 大于设定次数后
      runCount[0] = 0;                                  // 重置值
      runCount[1] = runMin;                             // 设定值
      triangleValue = 0;                                // 初始值
    }
  }
  // 获取幅度变化
  if (triangleValue != 0) {                             // 如果不等于0
    airflowValue = triangleValue / runCount[0];         // 得到平均值
    Serial.print(airflowValue);                         // 打印平均值
    Serial.print(",");
  } else {                                              // 如果等于0
    airflowValue = 0;                                   // 则等于0
    Serial.print(airflowValue);
    Serial.print(",");
  }
  // 打印触发累积量
  //  Serial.println(triangleValue);
  Serial.println();
}
