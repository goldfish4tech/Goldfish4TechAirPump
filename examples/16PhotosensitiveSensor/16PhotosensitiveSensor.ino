/**
   光敏可视化
   有光线充气，没光线放气

   视频链接：
   https://www.bilibili.com/video/BV1b2421M74S

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

#define ruleState             true                                                    // true为正关系  false为反关系（数据有光越大为正，数据有光越小为反）

// 光敏传感器 -----------------------------------------------------------------------------------------------
#define photosensitivePin     A0                                                      // 获取传感器数据引脚
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
// 滑动取平均光数据 -------------------------------------
#define numReadings           100                                                     // 设置滑动平均的样本数量【可调】
int readings[numReadings] = {0};                                                      // 存储样本值的数组
int runIndex = 0;                                                                     // 当前数组索引
long total = 0;                                                                       // 累加总和
#define intervalTime          5                                                       // 检测采集时间 【可调】
unsigned long runTime = 0;                                                            // 记录阶段运行时间
int average;

// 间隔设置充气放气变化状态 ------------------------------
#define judgmentInterval      2000                                                    // 判断间隔时间
unsigned long judgmentTime =  0;                                                      // 判断时间
int oldAverageData =          0;                                                      // 存储上一次数据
bool runState =               false;                                                  // 运行状态
int runSubscript =            0;                                                      // 运行下标 0为充气 1为放气
unsigned long recordTime =    0;                                                      // 记录时间
unsigned long controlTime =   0;                                                      // 控制时间
#define minTime               (1 * 1000UL)                                            // 运行最短时间【可调】
#define maxTime               (10 * 1000UL)                                           // 运行最长时间【可调】
// ---------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                                 // 设置波特率9600
  // 气泵设置 -------------------
  pinMode(airPumpPin, OUTPUT);                                                        // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                       // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                               // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveOFF);                                             // 初始状态 气阀关闭 关闭放气
  // 启动打印 --------------------
  Serial.println(F("Started successfully!"));                                         // 启动成功打印提示
}

void loop() {
  // 判断检测时间采集获取数据 -----------------------------------------------------------------------------------
  if (millis() - runTime >= intervalTime) {                                           // 判断记录时间
    runTime = millis();                                                               // 记录当前时间
    int sensorValue = analogRead(photosensitivePin);                                  // 读取传感器数据
    if (ruleState == true) {
      sensorValue = abs(1023 - sensorValue);                                          // 取正
    }

    total -= readings[runIndex];                                                      // 减去最旧的样本值
    readings[runIndex] = sensorValue;                                                 // 将新的样本值加入数组
    total += sensorValue;                                                             // 累加总和
    runIndex = (runIndex + 1) % numReadings;                                          // 移动数组索引
    average = total / numReadings;                                                    // 计算平均值

    // 输出结果
    Serial.println(average);
  }

  // 控制充气 放气状态 -----------------------------------------------------------------------------------------
  if (millis() - judgmentTime >= judgmentInterval) {
    judgmentTime = millis();
    // 判断数据变化的充气变化
    if (average != oldAverageData && runState == false) {                             // 如果不等于则开启
      if (abs(average - oldAverageData) > 3) {
        runState = true;
        controlTime = map(abs(average - oldAverageData), 0, 1023, minTime, maxTime);  // 获取设置时间
        recordTime = millis();
        if (average > oldAverageData) {                                               // 大于则充气
          runSubscript = 0;
        } else {                                                                      // 小于则放气
          runSubscript = 1;
        }
      }
      oldAverageData = average;                                                       // 数据赋值
    }
  }
  if (runState == true) {
    if (runSubscript == 0) {
      digitalWrite(airPumpPin, airPumpON);                                            // 开启充气
      digitalWrite(airValvePin, airValveOFF);                                         // 关闭放气
    } else {
      digitalWrite(airPumpPin, airPumpOFF);                                           // 停止充气
      digitalWrite(airValvePin, airValveON);                                          // 开启放气
    }
    if (millis() - recordTime >= controlTime) {
      runState = false;
      digitalWrite(airPumpPin, airPumpOFF);                                           // 停止充气
      digitalWrite(airValvePin, airValveOFF);                                         // 关闭放气
    }

  }

  // loop回括号
}
