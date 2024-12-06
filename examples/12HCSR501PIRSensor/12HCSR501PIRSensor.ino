
/**
   气泵效果12
   通过判断人是否 存在 或 离开的情况下，控制气球充气 和 放气

   人体热释电红外感应模块：https://item.taobao.com/item.htm?id=560524808864

   模块说明：https://www.sohu.com/a/460679898_207853

   视频链接：
   https://www.bilibili.com/video/BV1zm4y1g7sq

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// -------------------------------- 气泵 ----------------------------------
#define airPumpPin            5                               // 控制气泵充气 引脚
#define airValvePin           6                               // 控制气阀放气 引脚
#define airPumpON             HIGH                            // 气泵打开 开启充气
#define airPumpOFF            !airPumpON                      // 气泵关闭 停止充气
#define airValveON            HIGH                            // 气阀打开 开启放气
#define airValveOFF           !airValveON                     // 气阀关闭 停止放气
// -----------------------------------------------------------------------

// -------------------------------- 人体热释 ----------------------------------
#define humanBodyPin          4                               // 人体存在传感器模块引脚
// -----------------------------------------------------------------------

// -------------------------------- 参数 ----------------------------------
#define initializeTime        (5 * 1000UL)                    // 初始化放气时间（1秒 = 1000毫秒）    *可调*
unsigned long runRecordTime = 0;                              // 运行记录时间

#define getDataIntervalTime   50                              // 获取数据间隔时间（1秒 = 1000毫秒）  *可调*
unsigned long getDataTime =   0;                              // 运行记录时间

uint8_t systemRunState =      0x00;                           // 系统运行状态

#define MaxRunTime            (25 * 1000UL)                   // 有人情况下，最大运行充气时间（1秒 = 1000毫秒）    *可调*
#define DeflateTime           (20 * 1000UL)                   // 没人情况下，最小放气时间（1秒 = 1000毫秒）    *可调*

bool runState = true;
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(humanBodyPin, INPUT);                               // 设置引脚输入
  // ------------- 设置气泵气阀 --------------
  pinMode(airPumpPin, OUTPUT);                                // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                               // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                       // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveON);                      // 初始状态 气阀打开 开启放气
  //analogWrite(airPumpPin, 0);                               // 初始状态 关闭气泵 停止充气
  //analogWrite(airValvePin, 255);                            // 初始状态 气阀打开 开启放气
  // 初始化开始时，先放气
  runRecordTime = millis();                                   // 记录当前时间
  Serial.println(F("初始化放气中~"));
  while (millis() - runRecordTime < initializeTime) {         // 判断时间是否符合条件
    Serial.println(F("deflating ~~~~"));                      // 打印提示
    delay(500);
    // 放气中
  }
  // ----------------------------------------
  Serial.println(F("Start successfully"));                    // 打印提示
}

void loop() {
  if (millis() - getDataTime >= getDataIntervalTime) {        // 判断间隔时间是否满足
    getDataTime = millis();                                   // 记录当前时间
    if (digitalRead(humanBodyPin) == true) {                  // 读取引脚状态
      if (runState == true) {
        runState = false;
        runRecordTime = millis();                             // 记录当前时间
      }
      if (millis() - runRecordTime <= MaxRunTime) {           // 时间如何小于
        digitalWrite(airPumpPin, airPumpON);                  // 打开气泵 开启充气
        digitalWrite(airValvePin, airValveOFF);               // 气阀关闭 关闭放气
      } else {
        digitalWrite(airPumpPin, airPumpOFF);                 // 关闭气泵 关闭充气
        digitalWrite(airValvePin, airValveOFF);               // 气阀关闭 关闭放气
      }
      Serial.println(1);                                      // 有人
    } else {
      if (runState == false) {
        runState = true;
        digitalWrite(airPumpPin, airPumpOFF);                 // 关闭气泵 关闭充气
        digitalWrite(airValvePin, airValveON);                // 气阀打开 打开放气
        delay(DeflateTime);
      }
      Serial.println(0);                                      // 没人
    }
  }

  // loop回括号
}
