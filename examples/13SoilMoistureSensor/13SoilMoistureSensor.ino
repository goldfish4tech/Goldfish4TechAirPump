/**
   气泵效果13
   通过不同土壤湿度 控制对应气球充入不同时间

   视频链接：
   https://www.bilibili.com/video/BV1Eu411F79c

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

#define logicalState          true                                    // 设置逻辑状态 true为越湿润气球越大  false为越越湿润气球越小

// -------------------------------- 气泵 ----------------------------------
#define airPumpPin            5                                       // 控制气泵充气 引脚
#define airValvePin           6                                       // 控制气阀放气 引脚
#define airPumpON             HIGH                                    // 气泵打开 开启充气
#define airPumpOFF            !airPumpON                              // 气泵关闭 停止充气
#define airValveON            HIGH                                    // 气阀打开 开启放气
#define airValveOFF           !airValveON                             // 气阀关闭 停止放气
// -----------------------------------------------------------------------

// -------------------------------- 湿度 ----------------------------------
int humidity = 0;                                                     // 湿度
// -----------------------------------------------------------------------

// -------------------------------- 参数 ----------------------------------
#define minHum                0                                       // 最小湿度 百分百                   *可调*
#define maxHum                100                                     // 最大湿度 百分百                   *可调*

#define minRunTime            (1 * 1000UL)                            // 对应最小充气时间（1秒 * 1000毫秒）  *可调*
#define maxRunTime            (35 * 1000UL)                           // 对应最大充气时间（50秒 * 1000毫秒） *可调*

#define initializeTime        (5 * 1000UL)                            // 初始化放气时间（1秒 = 1000毫秒）    *可调*
unsigned long runRecordTime = 0;                                      // 运行记录时间

#define getDataIntervalTime   (500)                                   // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long getDataTime =   0;                                      // 运行记录时间

uint8_t systemRunState =      0x00;                                   // 系统运行状态
int LastHumData =             0;                                      // 存储上次湿度数据
unsigned long elapsedTime =   0;                                      // 记录已经运行时间

unsigned long tempRunTime =   0;
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(airPumpPin, OUTPUT);                                        // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                       // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                               // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                              // 初始状态 气阀打开 开启放气
  // 初始化开始时，先放气
  runRecordTime = millis();                                           // 记录当前时间
  Serial.println(F("初始化放气中~"));
  while (millis() - runRecordTime < initializeTime) {                 // 判断时间是否符合条件
    Serial.println(F("deflating ~~~~"));                              // 打印提示
    delay(500);
    // 放气中
  }
  runRecordTime = millis();                                           // 记录当前时间
  Serial.println(F("Start successfully"));                            // 打印提示
}

void loop() {
  switch (systemRunState) {
    case 0x00: {                                                      // 获取当前温度 湿度数据
        if (millis() - getDataTime >= getDataIntervalTime) {          // 判断间隔时间是否满足
          getDataTime = millis();                                     // 记录当前时间
          int tempVal = analogRead(A0);                               // 获取模拟量
          if (logicalState) {
            humidity = map(tempVal, 1023, 0, 0, 100);
          } else {
            humidity = map(tempVal, 1023, 0, 100, 0);
          }
          Serial.print("Humidity: ");
          Serial.print(humidity);
          Serial.println("%");
          Serial.println();
          systemRunState = 0x01;                                      // 状态改变
        }
      } break;
    case 0x01: {                                                      // 判断是否需要开启充气 或 放气
        if (humidity != LastHumData) {                                // 如果没有记录数据
          if (humidity - LastHumData > 0) {                           // 判断数据差多少 为正充气
            systemRunState = 0x02;                                    // 状态改变
            LastHumData = humidity;                                   // 赋值
            humidity = constrain(humidity, minHum, maxHum);           // 设置数据范围
            tempRunTime = map(humidity, minHum, maxHum, minRunTime, maxRunTime);
            Serial.print("充气 Run Time: ");
            Serial.print((tempRunTime - elapsedTime) / 1000.0);
            Serial.println("s");
          } else {                                                    // 判断数据差多少 为负放气
            systemRunState = 0x03;                                    // 状态改变
            LastHumData = humidity;                                   // 赋值
            humidity = constrain(humidity, minHum, maxHum);           // 设置数据范围
            tempRunTime = map(humidity, minHum, maxHum, minRunTime, maxRunTime);
            Serial.print("放气 Run Time: ");
            Serial.print((elapsedTime - tempRunTime) / 1000.0);
            Serial.println("s");
          }
        } else {
          systemRunState = 0x00;                                      // 状态改变
        }
        runRecordTime = millis();                                     // 记录当前时间
      } break;
    case 0x02: {                                                      // 设置充气
        digitalWrite(airPumpPin, airPumpON);                          // 打开气泵 开启充气
        digitalWrite(airValvePin, airValveOFF);                       // 气阀关闭 关闭放气
        if (millis() - runRecordTime >= tempRunTime - elapsedTime) {
          digitalWrite(airPumpPin, airPumpOFF);                       // 关闭气泵 关闭充气
          digitalWrite(airValvePin, airValveOFF);                     // 气阀关闭 关闭放气
          elapsedTime = tempRunTime;
          systemRunState = 0x00;
        }
      } break;
    case 0x03: {                                                      // 设置放气
        digitalWrite(airPumpPin, airPumpOFF);                         // 关闭气泵 关闭充气
        digitalWrite(airValvePin, airValveON);                        // 气阀打开 打开放气
        if (millis() - runRecordTime >= elapsedTime - tempRunTime) {
          digitalWrite(airPumpPin, airPumpOFF);                       // 打开气泵 开启充气
          digitalWrite(airValvePin, airValveOFF);                     // 气阀关闭 关闭放气
          elapsedTime = tempRunTime;
          systemRunState = 0x00;
        }
      } break;
  }

  // loop回括号
}
