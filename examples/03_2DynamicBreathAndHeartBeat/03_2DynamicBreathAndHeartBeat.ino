/**
   双气泵控制：一个实现动态呼吸，一个实现心脏心跳效果

   视频链接：
   https://www.bilibili.com/video/BV1TW421c7iU
   
   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// -------------------------------- 气泵1（呼吸效果） ----------------------------------
#define airPumpPin1            3                                // 控制气泵1充气 引脚
#define airValvePin1           4                                // 控制气阀1放气 引脚
#define airPump1ON             HIGH                             // 气泵1打开 开启充气
#define airPump1OFF            !airPump1ON                      // 气泵1关闭 停止充气
#define airValve1ON            HIGH                             // 气阀1打开 开启放气
#define airValve1OFF           !airValve1ON                     // 气阀1关闭 停止放气

// -------------------------------- 气泵2（心跳效果） ----------------------------------
#define airPumpPin2            5                                // 控制气泵2充气 引脚
#define airValvePin2           6                                // 控制气阀2放气 引脚
#define airPump2ON             HIGH                             // 气泵2打开 开启充气
#define airPump2OFF            !airPump2ON                      // 气泵2关闭 停止充气
#define airValve2ON            HIGH                             // 气阀2打开 开启放气
#define airValve2OFF           !airValve2ON                     // 气阀2关闭 停止放气

// -------------------------------- 参数 ----------------------------------
#define initializeTime1        2500                             // 初始化放气时间（1秒 = 1000毫秒）  *可调*
unsigned long runRecordTime1 = 0;                               // 运行记录时间
#define inflatableTime1        5000                             // 充气时间（1秒 = 1000毫秒）       *可调*
#define deflationTime1         3000                             // 放气时间（1秒 = 1000毫秒）       *可调*
uint8_t systemRunState1 =      0x00;                            // 系统1运行状态

#define initialize             5000                             // 初始化放气时间（1秒 = 1000毫秒）  *可调*
#define initializeTime2        3000                             // 初始化充气 加载时间（1秒 = 1000毫秒）  *可调*
unsigned long runRecordTime2 = 0;                               // 运行记录时间
#define inflatableTime2        500                              // 充气时间（1秒 = 1000毫秒）       *可调*
#define deflationTime2         280                              // 放气时间（1秒 = 1000毫秒）       *可调*
uint8_t systemRunState2 =      0x00;                            // 系统2运行状态
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);

  // 设置气泵1引脚
  pinMode(airPumpPin1, OUTPUT);
  pinMode(airValvePin1, OUTPUT);
  digitalWrite(airPumpPin1, airPump1OFF);
  digitalWrite(airValvePin1, airValve1ON);

  // 初始化气泵1
  runRecordTime1 = millis();
  Serial.println(F("气泵1初始化放气中~"));
  while (millis() - runRecordTime1 < initializeTime1) {
    // 放气中
  }
  Serial.println(F("气泵1 Start successfully"));

  // 设置气泵2引脚
  pinMode(airPumpPin2, OUTPUT);
  pinMode(airValvePin2, OUTPUT);
  digitalWrite(airPumpPin2, airPump2OFF);
  digitalWrite(airValvePin2, airValve2ON);

  // 初始化气泵2
  runRecordTime2 = millis();
  Serial.println(F("气泵2初始化放气中~"));
  while (millis() - runRecordTime2 < initialize) {
    // 放气中
  }
  runRecordTime2 = millis();
  digitalWrite(airPumpPin2, airPump2ON);
  digitalWrite(airValvePin2, airValve2OFF);
  Serial.println(F("气泵2 Start successfully"));
}

void setup() {
  Serial.begin(9600);

  // 设置气泵1引脚
  pinMode(airPumpPin1, OUTPUT);
  pinMode(airValvePin1, OUTPUT);
  digitalWrite(airPumpPin1, airPump1OFF);                       // 初始化时关闭气泵1
  digitalWrite(airValvePin1, airValve1ON);                      // 初始化时打开气阀1进行放气

  // 初始化气泵1
  runRecordTime1 = millis();                                    // 记录气泵1的运行时间
  Serial.println(F("气泵1初始化放气中~"));
  while (millis() - runRecordTime1 < initializeTime1) {
    // 在此期间气泵1进行放气
  }
  Serial.println(F("气泵1 启动成功"));

  // 设置气泵2引脚
  pinMode(airPumpPin2, OUTPUT);
  pinMode(airValvePin2, OUTPUT);
  digitalWrite(airPumpPin2, airPump2OFF);                       // 初始化时关闭气泵2
  digitalWrite(airValvePin2, airValve2ON);                      // 初始化时打开气阀2进行放气

  // 初始化气泵2
  runRecordTime2 = millis();                                    // 记录气泵2的运行时间
  Serial.println(F("气泵2初始化放气中~"));
  while (millis() - runRecordTime2 < initialize) {
    // 在此期间气泵2进行放气
  }
  runRecordTime2 = millis();                                    // 重新记录时间
  digitalWrite(airPumpPin2, airPump2ON);                        // 启动气泵2开始充气
  digitalWrite(airValvePin2, airValve2OFF);                     // 关闭气阀2停止放气
  Serial.println(F("气泵2 启动成功"));
}

void loop() {
  // 气泵1：动态呼吸效果
  switch (systemRunState1) {
    case 0x00: { // 充气状态
        Serial.println(F("气泵1开始充气"));
        runRecordTime1 = millis();                              // 记录当前时间
        digitalWrite(airPumpPin1, airPump1ON);                  // 启动气泵1
        digitalWrite(airValvePin1, airValve1OFF);               // 关闭气阀1
        systemRunState1 = 0x01;                                 // 进入下一状态：充气完成
      } break;
    case 0x01: { // 判断充气是否完成
        if (millis() - runRecordTime1 >= inflatableTime1) {
          systemRunState1 = 0x02;                               // 充气完成，进入放气状态
        }
      } break;
    case 0x02: { // 放气状态
        Serial.println(F("气泵1开始放气"));
        runRecordTime1 = millis();                              // 记录当前时间
        digitalWrite(airPumpPin1, airPump1OFF);                 // 停止气泵1
        digitalWrite(airValvePin1, airValve1ON);                // 打开气阀1进行放气
        systemRunState1 = 0x03;                                 // 进入放气状态
      } break;
    case 0x03: { // 判断放气是否完成
        if (millis() - runRecordTime1 >= deflationTime1) {
          systemRunState1 = 0x00;                               // 放气完成，重新开始充气
        }
      } break;
  }

  // 气泵2：心跳效果
  switch (systemRunState2) {
    case 0x00: { // 等待初始化完成
        if (millis() - runRecordTime2 >= initializeTime2) {
          systemRunState2 = 0x01;                               // 初始化完成，开始充气
        }
      } break;
    case 0x01: { // 充气状态
        Serial.println(F("气泵2开始充气"));
        runRecordTime2 = millis();                              // 记录当前时间
        digitalWrite(airPumpPin2, airPump2ON);                  // 启动气泵2
        digitalWrite(airValvePin2, airValve2OFF);               // 关闭气阀2
        systemRunState2 = 0x02;                                 // 进入下一状态：充气完成
      } break;
    case 0x02: { // 判断充气是否完成
        if (millis() - runRecordTime2 >= inflatableTime2) {
          systemRunState2 = 0x03;                               // 充气完成，进入放气状态
        }
      } break;
    case 0x03: { // 放气状态
        Serial.println(F("气泵2开始放气"));
        runRecordTime2 = millis();                              // 记录当前时间
        digitalWrite(airPumpPin2, airPump2OFF);                 // 停止气泵2
        digitalWrite(airValvePin2, airValve2ON);                // 打开气阀2进行放气
        systemRunState2 = 0x04;                                 // 进入放气状态
      } break;
    case 0x04: { // 判断放气是否完成
        if (millis() - runRecordTime2 >= deflationTime2) {
          systemRunState2 = 0x01;                               // 放气完成，重新开始充气
        }
      } break;
  }
}
