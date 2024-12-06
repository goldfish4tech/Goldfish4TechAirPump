/**
  使用气泵实现呼吸草坪的效果

  视频链接：
  https://www.bilibili.com/video/BV1HsHLetED5

  库文件：
  https://github.com/adafruit/Adafruit_MPR121
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

#include <Wire.h>
#include <Adafruit_MPR121.h>

Adafruit_MPR121 cap = Adafruit_MPR121();          // MPR121触摸传感器对象

// 气泵数量
#define PumpCount        2
#define ProcessCount     7
#define PumpStartDelay   500                      // 启动气泵前的延时
#define initializeTime   2000                     // 初始化气泵的放气时间
#define DetectionPeriod  200                      // 触摸检测周期
#define touchThreshold   80                       // 触摸检测阈值

uint8_t i = 0, j = 0;
uint8_t systemState = 0x00;                       // 系统状态
unsigned long systemtimes = 0;
unsigned long lastDetection = 0;
int Pump1time = 0, Pump2time = 0;                 // 气泵运行的时间

uint8_t count[PumpCount] = {0, 0};                // 记录气泵是否完成操作

// 气泵控制引脚数组
char PumpPin[PumpCount][2] = {
  {2, 3},                                         // 气泵1的控制引脚
  {4, 5},                                         // 气泵2的控制引脚
};

// 不同流程中每个气泵的运行时间，大于零为充气，小于零为放气，单位为毫秒
int Pumptime[ProcessCount][PumpCount] = {
  {3000, 0},                                      // 充气
  { -3000, 3000},                                 // 放气和充气
  {3000, -3000},                                  // 充气和停止
  { -3000, 3000},                                 // 放气和充气
  {3000, -3000},                                  // 充气和放气
  { -3000, 3000},                                 // 放气和停止
  {3000, -3000},                                  // 充气和放气
};

// 气泵初始化放气函数，当开发板的板载LED灯(标识为 L)亮起时初始化完成
void Pumpinitialize() {
  digitalWrite(LED_BUILTIN, LOW);                 // 关闭LED指示灯
  for (i = 0; i < PumpCount; i++) {
    digitalWrite(PumpPin[i][1], HIGH);            // 放气
  }
  systemtimes = millis();
  while (millis() - systemtimes < initializeTime) {
    // 初始化放气中
  }
  for (i = 0; i < PumpCount; i++) {
    digitalWrite(PumpPin[i][1], LOW);             // 停止放气
  }
  Serial.println(F("初始化放气完成!"));
}

// 气泵控制函数
void PumpControl(uint8_t pumpIndex, int pumpTime) {
  if (millis() - systemtimes < abs(pumpTime)) {
    if (pumpTime > 0) {
      digitalWrite(PumpPin[pumpIndex][0], HIGH);  // 开始充气
      digitalWrite(PumpPin[pumpIndex][1], LOW);
    } else {
      digitalWrite(PumpPin[pumpIndex][0], LOW);   // 开始放气
      digitalWrite(PumpPin[pumpIndex][1], HIGH);
    }
  } else {
    count[pumpIndex] = 1;  // 标记气泵已完成操作
    digitalWrite(PumpPin[pumpIndex][0], LOW);     // 停止气泵
    digitalWrite(PumpPin[pumpIndex][1], LOW);
  }
}

// 当前步骤气泵控制结束运行函数
void PumpControlOver() {
  if ((count[0] + count[1]) == 2) {               // 如果所有气泵都完成了操作
    i++;  // 进入下一个步骤
    systemState++;  // 改变系统状态
    systemtimes = millis();  // 记录当前时间
    Serial.println("步骤" + String(i) + "已完成!");
    count[0] = count[1] = 0;  // 重置计数器
  }
}

// 初始化函数
void setup() {
  Serial.begin(9600);                             // 初始化串口通信
  pinMode(LED_BUILTIN, OUTPUT);                   // 设置板载LED引脚为输出模式
  for (i = 0; i < PumpCount; i++) {
    for (j = 0; j < 2; j++)
      pinMode(PumpPin[i][j], OUTPUT);             // 设置气泵控制引脚为输出模式
    delay(5);
  }
  Pumpinitialize();                               // 初始化气泵
  if (!cap.begin(0x5A)) {                         // MPR121 I2C地址为0x5A
    Serial.println("MPR121触摸传感器初始化失败");
    while (1);                                    // 如果初始化失败，停机
  }
}

// 检查是否有触摸事件
bool isAnyTouched() {
  for (uint8_t i = 0; i < 12; i++) {
    uint16_t data = cap.filteredData(i);          // 获取触摸数据
    if (data < touchThreshold) {                  // 如果触摸值低于阈值，表示被触摸
      return true;
    }
  }
  return false;                                   // 如果没有触摸事件，返回false
}

// 主函数
void loop() {
  switch (systemState) {
    case 0x00:  // 等待触摸事件
      if (millis() - lastDetection > DetectionPeriod) {  // 每隔一段时间检查是否触摸
        if (isAnyTouched()) {
          systemState++;                                 // 如果有触摸，进入下一状态
          systemtimes = millis();                        // 记录触摸消失的时间
          Serial.println(F("检测到触摸! 开始启动延时等待!"));
        }
        lastDetection = millis();                       // 更新最后检测时间
        digitalWrite(LED_BUILTIN, HIGH);                // 点亮LED表示正在检测
      }
      break;

    case 0x01:  // 启动延时等待
      if (millis() - systemtimes > PumpStartDelay) {    // 如果延时完成，进入下一状态
        systemState++;
        systemtimes = millis();
        Serial.println(F("开始充放气!"));
      }
      break;

    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
      if (isAnyTouched()) {                            // 如果有触摸事件，启动气泵
        PumpControl(0, Pumptime[i][0]);
        PumpControl(1, Pumptime[i][1]);
      } else {                                         // 如果没有触摸事件，停止气泵
        digitalWrite(PumpPin[0][0], LOW);
        digitalWrite(PumpPin[0][1], LOW);
        digitalWrite(PumpPin[1][0], LOW);
        digitalWrite(PumpPin[1][1], LOW);
      }
      PumpControlOver();                               // 检查当前步骤是否完成
      break;

    case 0x09:                                         // 充放气流程完成
      Serial.println(F("充放气流程已完成!"));
      for (i = 0; i < ProcessCount; i++) {
        Pump1time += Pumptime[i][0];                   // 累加气泵1的运行时间
        Pump2time += Pumptime[i][1];                   // 累加气泵2的运行时间
      }
      Serial.println("气泵1放气时间: " + String(Pump1time));
      Serial.println("气泵2放气时间: " + String(Pump2time));
      digitalWrite(LED_BUILTIN, HIGH);                 // 点亮LED表示完成
      systemState++;                                   // 进入下一个步骤
      break;

    case 0x0A:                                         // 完成放气
      {
                                                       // 检查气泵1
        if (Pump1time > 0) {
          if (millis() - systemtimes < Pump1time) {
            digitalWrite(PumpPin[0][0], LOW);          // 放气
            digitalWrite(PumpPin[0][1], HIGH);
          } else {
            count[0] = 1;                              // 标记气泵1完成操作
            digitalWrite(PumpPin[0][0], LOW);
            digitalWrite(PumpPin[0][1], LOW);
          }
        } else {
          count[0] = 1;
        }

        // 检查气泵2
        if (Pump2time > 0) {
          if (millis() - systemtimes < Pump2time) {
            digitalWrite(PumpPin[1][0], LOW);          // 放气
            digitalWrite(PumpPin[1][1], HIGH);
          } else {
            count[1] = 1;                              // 标记气泵2完成操作
            digitalWrite(PumpPin[1][0], LOW);
            digitalWrite(PumpPin[1][1], LOW);
          }
        } else {
          count[1] = 1;
        }

        // 如果所有气泵都完成放气
        if ((count[0] + count[1]) == 2) {
          i = 0;                                       // 重置步骤
          systemState = 0x00;                          // 回到初始状态
          Serial.println(F("放气完成!"));
          count[0] = count[1] = 0;                     // 重置计数器
          Pump1time = Pump2time = 0;                   // 重置气泵时间
        }
      }
      break;

    default:
      break;
  }
}
