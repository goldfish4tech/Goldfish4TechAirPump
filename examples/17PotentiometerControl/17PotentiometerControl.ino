
/**
  电位器控制集成化气泵充气量

  视频链接：
  https://www.bilibili.com/video/BV1ct421t7Wf

  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

// 电位器 -------------------------------------------------------------------------------------------------
#define filterState             true                                                  // 滤波状态（true为开启  false为关闭）

#define deviceCount             1                                                     // 电位器数量
int getAnalogPin[deviceCount] = {A0};                                                 // 读取模拟量引脚

#define getIntervalTime         1000                                                  // 获取数据间隔时间   【可调】
unsigned long getDataTime  =    0;                                                    // 获取数据记录时间
#define dataIntervalValue       2                                                     // 数据差间隔值      【可调】
#define judgmentMaxCount        3                                                     // 判断数据最大变化次数（超过当前次数数据重置）【可调】
int getRunDataCount[deviceCount] = {0};                                               // 获取数据差值次数变量
// 数据值结构体
typedef struct dataMessage {
  int minValue =    0;                                                                // 获取数据 最小数据值
  int maxValue =    1023;                                                             // 获取数据 最大数据值
  int newGetValue = 0;                                                                // 存储当前新数据
  int oldGetValue = 0;                                                                // 存储当前老数据
} dataMessage;
dataMessage objectData[deviceCount];                                                  // 存储对象数据
// ---------------------------------------------------------------------------------------------------------

// 气泵 -----------------------------------------------------------------------------------------------------
#define airPumpPin              5                                                     // 控制气泵充气 引脚
#define airValvePin             6                                                     // 控制气阀放气 引脚
#define airPumpON               HIGH                                                  // 气泵打开 开启充气
#define airPumpOFF              !airPumpON                                            // 气泵关闭 停止充气
#define airValveON              HIGH                                                  // 气阀打开 开启放气
#define airValveOFF             !airValveON                                           // 气阀关闭 停止放气                                                      
// ---------------------------------------------------------------------------------------------------------

// 参数设置 -------------------------------------------------------------------------------------------------
int oldStoreData = 0;                                                                 // 存储上一次就数据
bool runState = false;                                                                // 运行状态
int runSubscript = 0;                                                                 // 运行下标 0为充气 1为放气

unsigned long recordTime =  0;                                                        // 记录时间
unsigned long controlTime = 0;                                                        // 控制时间

#define minTime                 (1 * 1000UL)                                          // 运行最短时间【可调】
#define maxTime                 (10 * 1000UL)                                         // 运行最长时间【可调】
// ---------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                                 // 设置波特率9600
  // 气泵设置 -------------------
  pinMode(airPumpPin, OUTPUT);                                                        // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                       // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                               // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveOFF);                                             // 初始状态 气阀关闭 关闭放气
  // 启动打印 --------------------
  Serial.println(F("Started successfully!"));
}

void loop() {
  // 采集获取数据 ----------------------------------------------
  if (millis() - getDataTime >= getIntervalTime) {                                    // 间隔获取数据
    getDataTime = millis();                                                           // 记录当前阶段时间
    int tempVal = getAnalogValue(0);                                                  // 数据值
    Serial.println(tempVal);                                                          // 打印数据
    if (tempVal != oldStoreData && runState == false) {                               // 如果不等于则开启
      runState = true;
      controlTime = map(abs(tempVal - oldStoreData), 0, 1023, minTime, maxTime);
      recordTime = millis();

      if (tempVal > oldStoreData) {                                                   // 大于则充气
        runSubscript = 0;
      } else {                                                                        // 小于则放气
        runSubscript = 1;
      }
      oldStoreData = tempVal;
    }
  }

  // 控制充气 放气状态 ------------------------------------------
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

// 返回当前稳定数据值
int getAnalogValue(int tempValue) {
  objectData[tempValue].newGetValue = analogRead(getAnalogPin[tempValue]);            // 读取当前模拟量

  // 整理数据  最大值 最小值
  if (objectData[tempValue].newGetValue > objectData[tempValue].maxValue) {           // 最大值
    objectData[tempValue].newGetValue = objectData[tempValue].maxValue;               // 赋值最大值
  } else if (objectData[tempValue].newGetValue < objectData[tempValue].minValue) {    // 最小值
    objectData[tempValue].newGetValue = objectData[tempValue].minValue;               // 赋值最小值
  }

  // 判断当前读取到的模拟量是否大于记录值
  if (abs(objectData[tempValue].newGetValue - objectData[tempValue].oldGetValue) > dataIntervalValue) {
    // 判断是否满足间隔差次数值
    if (++getRunDataCount[tempValue] >= judgmentMaxCount) {                           // 判断次数是否达到
      getRunDataCount[tempValue] = judgmentMaxCount;
      objectData[tempValue].oldGetValue = objectData[tempValue].newGetValue;          // 赋值当前值
    }
  } else {
    getRunDataCount[tempValue] = 0;                                                   // 记录清零
  }
  // 根据不同状态反馈不同数据
  if (filterState == true) {
    return objectData[tempValue].oldGetValue;                                         // 返回int类型数据值
  } else {
    return objectData[tempValue].newGetValue;                                         // 返回int类型数据值
  }
}
