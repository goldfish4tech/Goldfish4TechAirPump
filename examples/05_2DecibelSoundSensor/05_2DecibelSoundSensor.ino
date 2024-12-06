/**
   气泵效果03
   通过声音传感器进行吹气

   视频链接：
   https://www.bilibili.com/video/BV1mn1LYCEg6

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/
#include <math.h>

#define micPin                  A0                                                                // 定义声音传感器引脚接模拟引脚A0
#define sampleWindow            100                                                               // 采样窗口宽度（毫秒）

unsigned int sample;                                                                              // 取样
const unsigned long interval =  300;                                                              // 定义时间间隔和上次更新时间
unsigned long previousMillis =  0;

// 气泵和气阀相关定义
#define airPumpPin              5                                                                 // 控制气泵充气引脚
#define airValvePin             6                                                                 // 控制气阀放气引脚
#define airPumpON               HIGH                                                              // 气泵打开
#define airPumpOFF              !airPumpON                                                        // 气泵关闭
#define airValveON              HIGH                                                              // 气阀打开
#define airValveOFF             !airValveON                                                       // 气阀关闭

// 参数定义
#define startValue              60                                                                //气泵开始工作的阈值(可修改，不要高于80，也不要低于50)
#define initializeTime          0                                                                 // 初始化放气时间（毫秒）
unsigned long runRecordTime =   0;                                                                // 运行记录时间
#define inflatableTimeMin       200                                                               // 最小充气时间（毫秒）- 对应110最大分贝(22，23，24，25行代码这四个时间都可以修改)
#define inflatableTimeMax       800                                                               // 最大充气时间（毫秒）- 对应startValue(60)分贝
#define deflationTimeMin        50                                                                // 最小放气时间（毫秒）- 对应110最大分贝
#define deflationTimeMax        200                                                               // 最大放气时间（毫秒）- 对应startValue(60)分贝
uint8_t systemRunState =        0x00;                                                             // 系统运行状态
int inflatableTime =            inflatableTimeMax;                                                // 充气时间
int deflationTime =             deflationTimeMax;                                                 // 放气时间
float db = 0;

void setup() {
  Serial.begin(9600);

  pinMode(airPumpPin, OUTPUT);                                                                    // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                                   // 设置引脚输出
  digitalWrite(airPumpPin, airPumpON);                                                            // 初始状态关闭气泵
  digitalWrite(airValvePin, airValveOFF);                                                         // 初始状态气阀打开

  // 初始化开始时，先放气
  runRecordTime = millis();                                                                       // 记录当前时间
  Serial.println(F("初始化放气中~"));
  while (millis() - runRecordTime < initializeTime) {                                             // 判断时间是否符合条件
    //digitalWrite(airPumpPin, airPumpOFF);                                                       // 初始状态关闭气泵
    //digitalWrite(airValvePin, airValveOFF);                                                     // 初始状态气阀打开
  }
  Serial.println(F("Start successfully"));
}

void loop() {
  unsigned long currentMillis = millis();                                                         // 获取当前时间

  if (currentMillis - previousMillis >= interval) {                                               // 检查是否到达更新间隔
    previousMillis = currentMillis;                                                               // 更新上次更新时间

    unsigned long startMillis = millis();                                                         // 开始采样窗口
    float peakToPeak = 0;                                                                         // 峰值-峰值水平

    unsigned int signalMax = 0;                                                                   // 最大值
    unsigned int signalMin = 1024;                                                                // 最小值

    // 收集数据100毫秒
    while (millis() - startMillis < sampleWindow) {
      sample = analogRead(micPin);                                                                // 从麦克风读取数据
      if (sample < 1024)                                                                          // 排除错误读数
      {
        if (sample > signalMax) {
          signalMax = sample;                                                                     // 保存最大值
        }
        if (sample < signalMin) {
          signalMin = sample;                                                                     // 保存最小值
        }
      }
    }

    peakToPeak = signalMax - signalMin;                                                           // max - min = 峰顶-峰底幅度

    // 将峰顶-峰底值转换为电压值
    float voltage = (peakToPeak * 5.0) / 1024.0;                                                  // 将ADC值转换为电压

    // 仅当电压值大于0时计算分贝值，避免log(0)或负数的问题
    if (voltage > 0) {
      db = (int)(40.0 * log10(voltage / 0.0029331));                                              // 假设基准电压为0.0009331，转换为整型
      //db = (int)(20.0 * log10(voltage / 0.0000931));                                            // 假设基准电压为0.0009331，转换为整型
    } else {
      db = 0;  // 如果电压为0或负数，则将分贝值设置为0
    }

    // 更新充气和放气时间
    if (db > startValue) {
      inflatableTime = map(db, startValue, 110, inflatableTimeMax, inflatableTimeMin);
      deflationTime = map(db, startValue, 110, deflationTimeMax, deflationTimeMin);               // 放气时间比充气时间略短
      if (inflatableTime < inflatableTimeMin) {
        inflatableTime = inflatableTimeMin;                                                       // 确保放气时间不低于最小值
      }
      if (inflatableTime > inflatableTimeMax) {
        inflatableTime = inflatableTimeMax;                                                       // 确保放气时间不低于最小值
      }
      if (deflationTime < deflationTimeMin) {
        deflationTime = deflationTimeMin;                                                         // 确保放气时间不低于最小值
      }
      if (deflationTime > deflationTimeMax) {
        deflationTime = deflationTimeMax;                                                         // 确保放气时间不低于最小值
      }
    } else {
      digitalWrite(airPumpPin, airPumpOFF);                                                       // 关闭充气
      digitalWrite(airValvePin, airValveOFF);                                                     // 关闭放气
    }

    // 打印到串行监视器以便调试
    Serial.print("当前分贝: ");
    Serial.print(db);
    Serial.println(" dB");
  }

  // 控制气泵和气阀的状态机逻辑
  switch (systemRunState) {
    case 0x00: {
        if (db < startValue) {
          digitalWrite(airPumpPin, airPumpOFF);                                                   // 关闭充气
          digitalWrite(airValvePin, airValveOFF);                                                 // 关闭放气
        }
        else {
          systemRunState = 0x01;
        }
      } break;
    case 0x01: {
        //Serial.println(F("开始充气"));
        runRecordTime = millis();
        digitalWrite(airPumpPin, airPumpON);
        digitalWrite(airValvePin, airValveOFF);
        systemRunState = 0x02;
      } break;
    case 0x02: {
        if (millis() - runRecordTime >= inflatableTime) {
          systemRunState = 0x03;
        }
      } break;
    case 0x03: {
        //Serial.println(F("开始放气"));
        runRecordTime = millis();
        digitalWrite(airPumpPin, airPumpOFF);
        digitalWrite(airValvePin, airValveON);
        systemRunState = 0x04;
      } break;
    case 0x04: {
        if (millis() - runRecordTime >= deflationTime) {
          systemRunState = 0x00;
        }
      } break;
  }
}
