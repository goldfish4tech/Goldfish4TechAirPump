/**
   气泵效果02
   呼吸效果，由小变大，由大变小

   视频链接：
   集成化气泵02——动态呼吸   https://www.bilibili.com/video/BV1WN411S7LJ
   气泵交互艺术——血液心脏   https://www.bilibili.com/video/BV1tT42117g6
   气泵交互艺术——神经元     https://www.bilibili.com/video/BV1VtsteRErj
   气泵交互艺术——棺柩心脏   https://www.bilibili.com/video/BV1Yz421y7xE

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
// -------------------------------- 参数 ----------------------------------
#define initializeTime        5000                            // 初始化放气时间（1秒 = 1000毫秒）  *可调*
unsigned long runRecordTime = 0;                              // 运行记录时间
#define inflatableTime        5000                            // 充气时间（1秒 = 1000毫秒）       *可调*
#define deflationTime         3000                            // 放气时间（1秒 = 1000毫秒）       *可调*
uint8_t systemRunState =      0x00;                           // 系统运行状态
// -----------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
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
                                                              // 放气中
  }
  Serial.println(F("Start successfully"));
}

void loop() {
  switch (systemRunState) {
    case 0x00: {                                              // 设置充气参数
        Serial.println(F("开始充气"));
        runRecordTime = millis();                             // 记录时间
        digitalWrite(airPumpPin, airPumpON);                  // 开启充气
        digitalWrite(airValvePin, airValveOFF);               // 关闭放气
        //analogWrite(airPumpPin, 255);                       // 开启充气
        //analogWrite(airValvePin, 0);                        // 关闭放气
        systemRunState = 0x01;                                // 状态改变
      } break;
    case 0x01: {                                              // 等待充气时间
        if (millis() - runRecordTime >= inflatableTime) {     // 判断充气时间是否达到
          systemRunState = 0x02;                              // 状态改变
        }
      } break;
    case 0x02: {                                              // 设置放气参数
        Serial.println(F("开始放气"));
        runRecordTime = millis();                             // 记录时间
        digitalWrite(airPumpPin, airPumpOFF);                 // 关闭充气
        digitalWrite(airValvePin, airValveON);                // 开启放气
        //analogWrite(airPumpPin, 0);                         // 关闭充气
        //analogWrite(airValvePin, 255);                      // 开启放气
        systemRunState = 0x03;                                // 状态改变
      } break;
    case 0x03: {                                              // 等待放气时间
        if (millis() - runRecordTime >= deflationTime) {      // 判断放气时间是否达到
          systemRunState = 0x00;                              // 状态改变
        }
      } break;
  }

  // loop回括号
}
