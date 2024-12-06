/**
  红外传感器控制气泵充气呈现波浪地毯效果

  视频链接：
  https://www.bilibili.com/video/BV15k4y197f1
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

#define sensor A0                                                     // 传感器信号引脚
#define PumpCount 3                                                   // 气泵数量
#define ProcessCount 7                                                // 流程数量
#define PumpStartDelay  3000                                          // 气泵启动延时
#define initializeTime  5000                                          // 初始化放气时间（1秒 = 1000毫秒）
#define TriggerDistance 30                                            // 距离阈值，小于触发
#define DetectionPeriod 100                                           // 传感器检测检测周期
#define analogThreshold 80                                            // 模拟量阈值
#define distanceConversionFactor 27.726                               // 转换因子，用于将电压转换为距离
#define sensorVoltageAtZeroDistance 0.4                               // 传感器在零距离时的输出电压，单位是伏特

uint8_t i = 0, j = 0;                                                 // 数组循环变量
uint8_t systemState = 0x00;                                           // 系统状态
float Value = 0, distance = 80;                                       // 传感器读取模拟值和距离初始值
unsigned long systemtimes = 0;                                        // 记录系统时间
unsigned long lastDetection = 0;                                      // 记录上一次检测时间
int Pump1time = 0, Pump2time = 0, Pump3time = 0;                      // 记录气泵放气时间

uint8_t count[PumpCount] = { 0, 0, 0 };  // 记录每个气泵的状态

// 气泵控制引脚数组
char PumpPin[PumpCount][2] = {
  { 2, 3 },
  { 4, 5 },
  { 6, 7 },
};

// 不同流程中每个气泵的运行时间 大于零充气，小于零放气， 单位:ms
int Pumptime[ProcessCount][PumpCount] = {
  { 10000, 3000, 2000 },                                              // 充充充
  { -7000, 7000, 0 },                                                 // 放充停
  { 3000, 0, 4000 },                                                  // 充停充
  { 3000, 5000, -2000 },                                              // 充充放
  { 2000, -3000, -1000 },                                             // 充放放
  { -1000, 0, 3000 },                                                 // 放停充
  { 3000, -2000, -1000 },                                             // 放停充
};

// 红外测距函数
void InfraredRanging() {
  Value = analogRead(sensor);                                         // 读取引脚模拟量
  if (Value > analogThreshold) {                                      // 当模拟量大于analogThreshold才计算距离，否则距离不准确
    float voltage = Value * (5.0 / 1023.0);                           // 将传感器值转换为电压
    distance = 1.0 / (voltage / sensorVoltageAtZeroDistance - 1.0);   // 将电压转换为距离
    distance = distance * distanceConversionFactor * 2 - 0.9;         // 使用转换因子进行校准
  }
}

// 气泵初始化放气函数，当开发板的板载LED灯(标识为 L)亮起时初始化完成
void Pumpinitialize() {
  digitalWrite(LED_BUILTIN, LOW);                                     // 关闭板载LED灯
  for (i = 0; i < PumpCount; i++) {                                   // 开启气阀，开始放气
    digitalWrite(PumpPin[i][1], HIGH);
  }
  systemtimes = millis();                                             // 记录当前时间
  while(millis() - systemtimes < initializeTime){                     // 判断时间是否符合条件
    // 初始化放气中
  }
  for (i = 0; i < PumpCount; i++) {                                   // 关闭气阀，停止放气
    digitalWrite(PumpPin[i][1], LOW);
  }
  Serial.println(F("初始化放气完成!"));                                 // 串口打印
}

// 气泵1控制函数
void Pump1Control() {
  if (millis() - systemtimes < abs(Pumptime[i][0])) {                 // 判断时间是否符合条件,符合条件
    if (Pumptime[i][0] > 0) {                                         // 如果当前步骤控制时间大于0就充气
      digitalWrite(PumpPin[0][0], HIGH);
      digitalWrite(PumpPin[0][1], LOW);
    } else {                                                          // 否则就放气
      digitalWrite(PumpPin[0][0], LOW);
      digitalWrite(PumpPin[0][1], HIGH);
    }
  } else {                                                            // 不符合条件时
    count[0] = 1;                                                     // count数组第一位赋值为1
    digitalWrite(PumpPin[0][0], LOW);                                 // 停止充气
    digitalWrite(PumpPin[0][1], LOW);                                 // 停止放气
  }
}

// 气泵2控制函数
void Pump2Control() {
  if (millis() - systemtimes < abs(Pumptime[i][1])) {                 // 判断时间是否符合条件,符合条件
    if (Pumptime[i][1] > 0) {                                         // 如果当前步骤控制时间大于0就充气
      digitalWrite(PumpPin[1][0], HIGH);
      digitalWrite(PumpPin[1][1], LOW);
    } else {                                                          // 否则就放气
      digitalWrite(PumpPin[1][0], LOW);
      digitalWrite(PumpPin[1][1], HIGH);
    }
  } else {                                                            // 不符合条件时
    count[1] = 1;                                                     // count数组第二位赋值为1
    digitalWrite(PumpPin[1][0], LOW);                                 // 停止充气
    digitalWrite(PumpPin[1][1], LOW);                                 // 停止放气
  }
}

// 气泵3控制函数
void Pump3Control() {
  if (millis() - systemtimes < abs(Pumptime[i][2])) {                 // 判断时间是否符合条件,符合条件
    if (Pumptime[i][2] > 0) {                                         // 如果当前步骤控制时间大于0就充气
      digitalWrite(PumpPin[2][0], HIGH);
      digitalWrite(PumpPin[2][1], LOW);
    } else {                                                          // 否则就放气
      digitalWrite(PumpPin[2][0], LOW);
      digitalWrite(PumpPin[2][1], HIGH);
    }
  } else {                                                            // 不符合条件时
    count[2] = 1;                                                     // count数组第三位赋值为1
    digitalWrite(PumpPin[2][0], LOW);                                 // 停止充气
    digitalWrite(PumpPin[2][1], LOW);                                 // 停止放气
  }
}

// 当前步骤气球控制结束运行函数
void PumpControlOver() {
  if ((count[0] + count[1] + count[2]) == 3) {                        // 如果count数组每一位都为1
    i++;                                                              // i自增
    systemState++;                                                    // 系统状态自增
    systemtimes = millis();                                           // 获取当前时间
    Serial.println("步骤" + String(i) + "已完成!");                    // 串口打印
    count[0] = count[1] = count[2] = 0;                               // count数组所有位清零
  }
}

// 初始化函数
void setup() {
  Serial.begin(9600);                                                 // 设置串口波特率
  pinMode(LED_BUILTIN, OUTPUT);                                       // 设置板载LED为输出模式
  for (i = 0; i < PumpCount; i++) {                                   // 设置气泵控制引脚为输出模式
    for (j = 0; j < 2; j++)
      pinMode(PumpPin[i][j], OUTPUT);
    delay(5);
  }
  Pumpinitialize();                                                   // 气泵初始化放气
}

// 主函数
void loop() {
  switch (systemState) {                                              // 根据系统状态位执行不同的指令
    case 0x00:
      {
        if (millis() - lastDetection > DetectionPeriod) {             // 判断时间是否符合条件,符合条件
          InfraredRanging();                                          // 红外测距
          if (distance < TriggerDistance) {                           // 如果测量距离小于距离阈值
            systemState++;                                            // 系统状态位+1
            Serial.println(F("有人进入!"));                            // 串口打印
          }
          lastDetection = millis();                                   // 更新上一次检测时间
          digitalWrite(LED_BUILTIN, HIGH);                            // 点亮板载LED
        }
      }
      break;
    case 0x01:
      {
        Value = analogRead(sensor);                                   // 读取传感器输出模拟量
        if (Value < analogThreshold) {                                // 当人离开时,气泵开始运行
          i = 0;                                                      // 初始化 i值为0
          systemState++;                                              // 系统状态位+1
          systemtimes = millis();                                     // 获取系统当前运行时间
          digitalWrite(LED_BUILTIN, LOW);                             // 关闭板载LED
          Serial.println("i = " + String(i));
          Serial.println(F("人已离开,开始启动延时等待!"));                // 串口打印
        }
      }
      break;
    case 0x02:
      {
        if(millis() - systemtimes > PumpStartDelay){
          systemState++;
          systemtimes = millis();
          Serial.println(F("开始充放气!"));                             // 串口打印
        }
      }
      break;
    case 0x03:
      {
        Pump1Control();     // 气泵1控制
        Pump2Control();     // 气泵2控制
        Pump3Control();     // 气泵3控制
        PumpControlOver();  // 气泵控制结束,系统状态位+1，进入下一步骤
      }
      break;
    case 0x04:
      {
        Pump1Control();     // 气泵1控制
        Pump2Control();     // 气泵2控制
        Pump3Control();     // 气泵3控制
        PumpControlOver();  // 气泵控制结束,系统状态位+1，进入下一步骤
      }
      break;
    case 0x05:
      {
        Pump1Control();     // 气泵1控制
        Pump2Control();     // 气泵2控制
        Pump3Control();     // 气泵3控制
        PumpControlOver();  // 气泵控制结束,系统状态位+1，进入下一步骤
      }
      break;
    case 0x06:
      {
        Pump1Control();     // 气泵1控制
        Pump2Control();     // 气泵2控制
        Pump3Control();     // 气泵3控制
        PumpControlOver();  // 气泵控制结束,系统状态位+1，进入下一步骤
      }
      break;
    case 0x07:
      {
        Pump1Control();     // 气泵1控制
        Pump2Control();     // 气泵2控制
        Pump3Control();     // 气泵3控制
        PumpControlOver();  // 气泵控制结束,系统状态位+1，进入下一步骤
      }
      break;
    case 0x08:
      {
        Pump1Control();     // 气泵1控制
        Pump2Control();     // 气泵2控制
        Pump3Control();     // 气泵3控制
        PumpControlOver();  // 气泵控制结束,系统状态位+1，进入下一步骤
      }
      break;
    case 0x09:
      {
        Pump1Control();     // 气泵1控制
        Pump2Control();     // 气泵2控制
        Pump3Control();     // 气泵3控制
        PumpControlOver();  // 气泵控制结束,系统状态位+1，进入下一步骤
      }
      break;
    case 0x0A:
      {
        Serial.println(F("充放气流程已完成!"));                    // 串口打印
        for (i = 0; i < ProcessCount; i++) {                     // 计算放气时间
          Pump1time += Pumptime[i][0];
          Pump2time += Pumptime[i][1];
          Pump3time += Pumptime[i][2];
        }
        Serial.println("气泵1放气时间: " + String(Pump1time));    // 串口打印
        Serial.println("气泵2放气时间: " + String(Pump2time));
        Serial.println("气泵3放气时间: " + String(Pump3time));
        systemState++;                                          // 系统状态位+1
        Serial.println(F("放气中!"));                            // 串口打印
        systemtimes = millis();                                 // 更新系统时间
      }
      break;
    case 0x0B:
      {
        if (Pump1time > 0) {                                    // 如果气泵1放气时间大于0
          if (millis() - systemtimes < Pump1time) {             // 判断时间是否符合条件，符合条件
            digitalWrite(PumpPin[0][0], LOW);
            digitalWrite(PumpPin[0][1], HIGH);                  // 气泵放气
          } else {                                              // 不符合条件
            count[0] = 1;                                       // count数组第一位赋值为1
            digitalWrite(PumpPin[0][0], LOW);
            digitalWrite(PumpPin[0][1], LOW);                   // 气泵停止放气
          }
        } else {                                                // 如果气泵1放气时间小于0
          count[0] = 1;                                         // count数组第一位赋值为1
        }

        if (Pump2time > 0) {                                    // 如果气泵2放气时间大于0
          if (millis() - systemtimes < Pump2time) {             // 判断时间是否符合条件，符合条件
            digitalWrite(PumpPin[1][0], LOW);
            digitalWrite(PumpPin[1][1], HIGH);                  // 气泵放气
          } else {                                              // 不符合条件
            count[1] = 1;                                       // count数组第二位赋值为1
            digitalWrite(PumpPin[1][0], LOW);
            digitalWrite(PumpPin[1][1], LOW);                   // 气泵停止放气
          }
        } else {                                                // 如果气泵1放气时间小于0
          count[1] = 1;                                         // count数组第二位赋值为1
        }

        if (Pump3time > 0) {                                    // 如果气泵3放气时间大于0
          if (millis() - systemtimes < Pump3time) {             // 判断时间是否符合条件，符合条件
            digitalWrite(PumpPin[2][0], LOW);
            digitalWrite(PumpPin[2][1], HIGH);                  // 气泵放气
          } else {                                              // 不符合条件
            count[2] = 1;                                       // count数组第三位赋值为1
            digitalWrite(PumpPin[2][0], LOW);
            digitalWrite(PumpPin[2][1], LOW);                   // 气泵停止放气
          }
        } else {                                                // 如果气泵1放气时间小于0
          count[2] = 1;                                         // count数组第三位赋值为1
        }

        if ((count[0] + count[1] + count[2]) == 3) {            // 如果count数组每一位都为1
          i = 0;                                                // 将i的值重置为0
          distance = 80;                                        // 将距离重置为80
          systemState = 0x00;                                   // 将系统状态位重置为0x00      
          Serial.println(F("放气完成!"));                        // 串口打印
          count[0] = count[1] = count[2] = 0;                   // 将count数组重置为0
          Pump1time = Pump2time = Pump3time = 0;                // 将三个气泵的放气时间重置为0
        }
      }
      break;
    default: break;
  }
}
