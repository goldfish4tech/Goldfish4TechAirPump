/**
   按钮。
   不同按钮对应的充气时间不同

   视频链接：
   https://www.bilibili.com/video/BV1p9SuYaEjb

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// --------------------------------- 系统参数可修改 --------------------
#define oneTime                     3000                             // 第一个按钮运行时间 3秒数（1000毫秒 = 1秒）
#define twoTime                     5000                             // 第二个按钮运行时间
#define threeTime                   7000                             // 第三个按钮运行时间
// -------------------------------------------------------------------

// --------------------------------- 按钮 --------------------
#define debounceDelay               20                               // 间隔时间为20毫秒
#define buttonNumber                3                                // 按钮个数
int buttonPin[3] =                  {10, 11, 12};                    // 设置按键引脚为
bool buttonFlag[3] =                {HIGH, HIGH, HIGH};              // 按键状态
bool lastButtonFlag[3] =            {HIGH, HIGH, HIGH};              // 上一次按键
unsigned long lastDebounceTime[3] = {0, 0, 0};                       // 按键按下去的初始时间
int reading[3] =                    {};                              // 按钮状态
// ----------------------------------------------------------

// --------------------------------- MOS管 --------------------
#define airPumpPin                  9                                // 气泵引脚
// ------------------------------------------------------------

// ------------------------- 程序运行参数 ---------------------------
int airPumpRunTime[3] =       {oneTime, twoTime, threeTime};         // 间隔时间
uint8_t systemState =         0x00;                                  // 系统状态机
unsigned long systemRunTime = 0;                                     // 记录程序阶段运行时间
long systemRunIntervalTime =  0;                                     // 系统运行间隔时间
bool airPumpFlag =            true;                                  // 启用状态
// ----------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                // 串口波特率9600
  // 设置MOS管
  pinMode(airPumpPin, OUTPUT);                                       // 针脚输出
  digitalWrite(airPumpPin, LOW);                                     // 气泵初始化
  // 设置按钮
  for (int i = 0; i < buttonNumber; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);                             // 设置按键模式为上拉输入
  }
  Serial.println(F("System started successfully!"));
}

void loop() {
  switch (systemState) {
    case 0x00: {      // 获取状态按钮触发状态
        getbutton();
      } break;
    case 0x01: {      // 进入对应按钮运行触发时间
        if (airPumpFlag) {
          // 开启气泵
          digitalWrite(airPumpPin, HIGH);
          airPumpFlag = false;
        }
        // 判断时间条件是否满足退出 并且 判断按钮是否还存在低电平状态
        if (millis() - systemRunTime >= systemRunIntervalTime) {
          if (digitalRead(buttonPin[0]) != LOW && digitalRead(buttonPin[1]) != LOW && digitalRead(buttonPin[2]) != LOW) {
            systemState = 0x02;
          } else {
            Serial.println(F("时间满足，但按钮被长按中，继续工作"));
          }
        }
      } break;
    case 0x02: {      // 状态结束退出 回到状态一
        // 关闭气泵
        digitalWrite(airPumpPin, LOW);

        systemState = 0x00;
        airPumpFlag = true;
      } break;
    default: {
      } break;
  }
}

void getbutton() {
  for (int i = 0; i < buttonNumber; i++) {
    reading[i] = digitalRead(buttonPin[i]);                          // 起始状态为1（高电平）
    if (reading[i] != lastButtonFlag[i]) {                           // 当状态发生改变，给时间赋值
      lastDebounceTime[i] = millis();                                // 并记录时间
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading[i] != buttonFlag[i]) {                             // 当状态发生改变
        buttonFlag[i] = reading[i];                                  // 赋值给buttonFlag
        if (buttonFlag[i] == LOW) {
          Serial.print("第几个: "); Serial.print(i + 1); Serial.println("按下");
          Serial.print("运行时间: "); Serial.println(airPumpRunTime[i]);
          // 记录触发按钮间隔时间
          systemRunIntervalTime = airPumpRunTime[i];
          // 记录当前程序阶段时间
          systemRunTime = millis();
          // 状态更新
          systemState = 0x01;
        }
      }
    }
    lastButtonFlag[i] = reading[i];
  }
}
