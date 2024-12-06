/**
  使用Arduino mega 2560开发板
  蓝牙控制9个气泵有交互效果

  视频链接：
  https://www.bilibili.com/video/BV1Qj411L7mb
  
  Contact us
  Tel/WeChat: 18682388114
  email:      goldfish4tech@goldfish4tech.
*/

// ---------------------------------------------------- 蓝牙模块 ---------------------------------------------------------
#define BTBaudRate            9600                                                          // 蓝牙连接波特率
#define BTSerial              Serial1                                                       // 定义串口

#define BTSendIntervalTime 500                                                              // 蓝牙发送间隔时间（1秒 = 1000毫秒）
unsigned long BTSendTime = 0;                                                               // 蓝牙发送阶段时间
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------- 超声波模块 ---------------------------------------------------------
#define trigPin               2                                                             // 定义超声波的trig为2号引脚
#define echoPin               3                                                             // 定义超声波的echo为3号引脚
#define getDataInterval       200                                                           // 获取超声波检测的距离数据的时间间隔(ms)
#define maxDistance           350                                                           // 最大距离
unsigned long getDistanceTime = 0;                                                          // 用于计算获取超声波数据间隔的时间
float distance = 999;                                                                       // 定义超声波获取到的距离变量
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------- 气泵控制 ---------------------------------------------------------
#define airPumpCont           9                                                             // 气泵数量
int airPumpPin[airPumpCont] = {                                                             // 控制气泵充气 引脚
  22, 24, 26, 28, 30, 32, 34, 36, 38
};
int airValvePin[airPumpCont] = {                                                            // 控制气阀放气 引脚
  23, 25, 27, 29, 31, 33, 35, 37, 39
};
#define airPumpON             HIGH                                                          // 气泵打开 开启充气
#define airPumpOFF            !airPumpON                                                    // 气泵关闭 停止充气
#define airValveON            HIGH                                                          // 气阀打开 开启放气
#define airValveOFF           !airValveON                                                   // 气阀关闭 停止放气
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------- 系统参数 ---------------------------------------------------------
uint8_t modelRunState = 0x00;                                                               // 模式运行状态
unsigned long modelRunTime = 0;                                                             // 记录模式运行时间
int runSubscript = 0;                                                                       // 记录运行下标

// 模式1 ----------------
#define modelTime_a           (1 * 1000UL)                                                  // 模式1 间隔时间
bool runState_a = true;                                                                     // 运行模式状态

// 模式2 ----------------
unsigned long runModelTime_b = 1000;                                                        // 间隔变化时间
#define normalTime_b          (1 * 1000UL)                                                  // 模式2 正常 间隔时间
#define unusualTime_b         (5 * 1000UL)                                                  // 模式2 异常 间隔时间
int runSequence[airPumpCont] = {                                                            // 运行顺序
  1, 2, 3, 6, 9, 8, 7, 4, 5
};
bool runState_b = true;                                                                     // 运行模式状态

// 模式3 ----------------
#define waitTime_c            (1 * 1000UL)                                                  // 充气第一个等待时间
#define inflateTime_c         (3 * 1000UL)

// 模式4 ----------------
#define runCount_d            5                                                             // 活动数量
int activityCount[runCount_d] = {                                                           // 活动下标
  1, 2, 3, 4, 6
};
#define inflateTime_d         (5 * 1000UL)                                                  // 充气时间
#define holdTime              (5 * 1000UL)                                                  // 保持时间
#define deflateTime           (5 * 1000UL)                                                  // 放气时间

// 模式5 ----------------
int oneRun_e[6] = {                                                                         // 第一组
  1, 2, 3, 7, 8, 9
};
int twoRun_e[3] = {                                                                         // 第二组
  4, 5, 6
};
#define inflateTime_e         (3 * 1000UL)                                                  // 充放气时间长
bool switchState = true;                                                                    // 切换状态

// 模式6 ----------------
#define setMinDisData         10                                                            // 设置距离参数【cm】 充气
#define setMaxDisData         15                                                            // 设置距离参数【cm】 放气

// 模式7 ----------------
int runGroup[3][3] = {                                                                      // 运行组号
  {1, 2, 3},
  {4, 5, 6},
  {7, 8, 9},
};
#define inflateTime_g         (3 * 1000UL)                                                  // 充放气时间长

// 模式8 ----------------
#define chooseTime            (1 * 1000UL)                                                  // 选择间隔时间
int chooseData[3] = {                                                                       // 存储记录
  0, 0, 0
};

// ---------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);                                                                       // 设置波特率为9600
  // 超声波 ------------------------
  pinMode(trigPin, OUTPUT);                                                                 // 设置超声波的trigPin为输出模式
  digitalWrite(trigPin, LOW);                                                               // 开机时将超声波的trigPin设置为低电平
  pinMode(echoPin, INPUT);                                                                  // 设置超声波的echoPin为接受模式
  // 气泵 ------------------------
  for (int i = 0; i < airPumpCont; i++) {                                                   // for循环控制
    pinMode(airPumpPin[i], OUTPUT);                                                         // 设置引脚输出
    pinMode(airValvePin[i], OUTPUT);                                                        // 设置引脚输出
    digitalWrite(airPumpPin[i], airPumpOFF);                                                // 初始状态 关闭气泵 停止充气
    digitalWrite(airValvePin[i], airValveON);                                               // 初始状态 气阀打开 开启放气
  }
  // 蓝牙模块 ------------------------
  BTSerial.begin(BTBaudRate);                                                               // 设置蓝牙连接
  // --------------------------------
  Serial.println(F("Start successfully"));                                                  // 启动提示
}

void loop() {
  // 获取蓝牙数据 ----------------------------------
  if (BTSerial.available() > 0) {                                                           // 检测软串口是否有数据
    String BTVal = "";                                                                      // 接收数据
    while (BTSerial.available() > 0) {                                                      // 如果串口有数据
      BTVal += char(BTSerial.read());                                                       // 读出串数据
      delay(2);                                                                             // 延迟2毫秒
    }
    Serial.print("Receive: ");                                                              // 打印数据
    Serial.println(BTVal);                                                                  // 打印数据
    if (BTVal.toInt() < 9) {
      modelRunState = BTVal.toInt();                                                        // 状态改变
      BTSerial.println("ok");                                                               // 蓝牙数据反馈

      // 重置状态 --------------------
      modelRunTime = millis();                                                              // 记录当前时间
      runSubscript = 0;                                                                     // 记录当前下标
      for (int i = 0; i < airPumpCont; i++) {
        digitalWrite(airPumpPin[i], airPumpOFF);                                            // 气泵关闭 停止充气
        digitalWrite(airValvePin[i], airValveON);                                           // 气阀开启 开启放气
      }
    } else {
      Serial.println("ERROR");                                                              // 打印提示
    }
  }

  // 获取串口数据 ----------------------------------
  if (Serial.available() > 0) {                                                             // 检测软串口是否有数据
    String BTVal = "";                                                                      // 接收数据
    while (Serial.available() > 0) {                                                        // 如果串口有数据
      BTVal += char(Serial.read());                                                         // 读出串数据
      delay(2);                                                                             // 延迟2毫秒
    }
    Serial.print("Receive: ");                                                              // 打印数据
    Serial.println(BTVal);                                                                  // 打印数据
    if (BTVal.toInt() < 9) {
      modelRunState = BTVal.toInt();                                                        // 状态改变
      // 重置状态 --------------------
      modelRunTime = millis();                                                              // 记录当前时间
      runSubscript = 0;                                                                     // 记录当前下标
      for (int i = 0; i < airPumpCont; i++) {
        digitalWrite(airPumpPin[i], airPumpOFF);                                            // 气泵关闭 停止充气
        digitalWrite(airValvePin[i], airValveON);                                           // 气阀开启 开启放气
      }
    } else {
      Serial.println("ERROR");                                                              // 打印提示
    }
  }

  // 模式运行状态 ----------------------------------
  switch (modelRunState) {
    case 0x00: {                    // 无运行
      } break;
    case 0x01: {                    // 模式1
        modelRun01();                                                                       // 模式一
      } break;
    case 0x02: {                    // 模式2
        modelRun02();
      } break;
    case 0x03: {                    // 模式3
        modelRun03();
      } break;
    case 0x04: {                    // 模式4
        modelRun04();
      } break;
    case 0x05: {                    // 模式5
        modelRun05();
      } break;
    case 0x06: {                    // 模式6
        getDistance();
        modelRun06();
      } break;
    case 0x07: {                    // 模式7
        modelRun07();
      } break;
    case 0x08: {                    // 模式8
        modelRun08();
      } break;
    default: break;
  }


  // loop回括号
}

// 运行模式状态01
void modelRun01() {
  if (millis() - modelRunTime >= modelTime_a) {                                             // 判断时间间隔
    modelRunTime = millis();                                                                // 记录当前时间
    if (runState_a == true) {
      digitalWrite(airPumpPin[runSubscript], airPumpON);                                    // 气泵开启 开启充气
      digitalWrite(airValvePin[runSubscript], airValveOFF);                                 // 气阀关闭 停止放气
    } else {
      digitalWrite(airPumpPin[runSubscript], airPumpOFF);                                   // 气泵关闭 停止充气
      digitalWrite(airValvePin[runSubscript], airValveON);                                  // 气阀开启 开启放气
      if (++runSubscript >= airPumpCont) {                                                  // 下标变化
        runSubscript = 0;
      }
    }
    runState_a = !runState_a;                                                               // 状态反转
  }
}

// 运行模式状态02
void modelRun02() {
  // 获取间隔运行时间 ------------------------------
  if (runSequence[runSubscript] != 5) {                                                     // 判断情况
    if (runState_b == true && runSubscript == 0) {
      runModelTime_b = unusualTime_b;
    } else {
      runModelTime_b = normalTime_b;
    }
  } else {
    if (runState_b == true) {
      runModelTime_b = normalTime_b;
    } else {
      runModelTime_b = unusualTime_b;
    }
  }

  // 运行切换状态 ----------------------------------
  int tempVal = runSequence[runSubscript] - 1;                                              // 获取数据值
  if (millis() - modelRunTime >= runModelTime_b) {                                          // 判断时间间隔
    modelRunTime = millis();                                                                // 记录当前时间
    if (runState_b == true) {
      digitalWrite(airPumpPin[tempVal], airPumpON);                                         // 气泵开启 开启充气
      digitalWrite(airValvePin[tempVal], airValveOFF);                                      // 气阀关闭 停止放气
    } else {
      digitalWrite(airPumpPin[tempVal], airPumpOFF);                                        // 气泵关闭 停止充气
      digitalWrite(airValvePin[tempVal], airValveON);                                       // 气阀开启 开启放气
      if (++runSubscript >= airPumpCont) {                                                  // 下标变化
        runSubscript = 0;
      }
    }
    runState_b = !runState_b;                                                               // 状态反转
  }
}

// 运行模式状态03
void modelRun03() {
  if (runSubscript == 0) {
    digitalWrite(airPumpPin[4], airPumpON);                                                 // 气泵开启 开启充气
    digitalWrite(airValvePin[4], airValveOFF);                                              // 气阀关闭 停止放气
    if (millis() - modelRunTime >= waitTime_c) {
      digitalWrite(airPumpPin[4], airPumpOFF);                                              // 气泵关闭 停止充气
      runSubscript = 1;                                                                     // 状态切换
      modelRunTime = millis();                                                              // 记录时间
    }
  } else if (runSubscript == 1) {
    for (int i = 0; i < airPumpCont; i++) {
      if (i != 4) {
        digitalWrite(airPumpPin[i], airPumpON);                                             // 气泵开启 开启充气
        digitalWrite(airValvePin[i], airValveOFF);                                          // 气阀关闭 停止放气
      }
    }
    if (millis() - modelRunTime >= inflateTime_c) {                                         // 判断充气时间
      runSubscript = 2;                                                                     // 状态切换
      modelRunTime = millis();                                                              // 记录时间
    }
  } else if (runSubscript == 2) {
    for (int i = 0; i < airPumpCont; i++) {
      digitalWrite(airPumpPin[i], airPumpOFF);                                              // 气泵关闭 停止充气
      digitalWrite(airValvePin[i], airValveON);                                             // 气阀开启 开启放气
    }
    if (millis() - modelRunTime >= inflateTime_c) {                                         // 判断充气时间
      runSubscript = 0;                                                                     // 状态切换
      modelRunTime = millis();                                                              // 记录时间
    }
  }
}

// 运行模式状态04
void modelRun04() {
  if (runSubscript == 0) {
    for (int i = 0; i < runCount_d; i++) {                                                  //
      int tempVal = activityCount[i] - 1;
      digitalWrite(airPumpPin[tempVal], airPumpON);                                         // 气泵开启 开启充气
      digitalWrite(airValvePin[tempVal], airValveOFF);                                      // 气阀关闭 停止放气
    }
    if (millis() - modelRunTime >= inflateTime_d) {
      runSubscript = 1;                                                                     // 状态切换
      modelRunTime = millis();                                                              // 记录时间
    }
  } else if (runSubscript == 1) {
    for (int i = 0; i < runCount_d; i++) {                                                  //
      int tempVal = activityCount[i] - 1;
      digitalWrite(airPumpPin[tempVal], airPumpOFF);                                        // 气泵关闭 停止充气
    }
    if (millis() - modelRunTime >= holdTime) {
      runSubscript = 2;                                                                     // 状态切换
      modelRunTime = millis();                                                              // 记录时间
    }
  } else if (runSubscript == 2) {
    for (int i = 0; i < runCount_d; i++) {                                                  //
      int tempVal = activityCount[i] - 1;
      digitalWrite(airPumpPin[tempVal], airPumpOFF);                                        // 气泵关闭 停止充气
      digitalWrite(airValvePin[tempVal], airValveON);                                       // 气阀开启 开启放气
    }

    if (millis() - modelRunTime >= deflateTime) {
      runSubscript = 0;                                                                     // 状态切换
      modelRunTime = millis();                                                              // 记录时间
    }
  }
}

// 运行模式状态05
void modelRun05() {
  if (millis() - modelRunTime >= inflateTime_e) {
    modelRunTime = millis();
    switchState = !switchState;
  }
  if (switchState == true) {                                                                // 根据状态切换
    for (int i = 0; i < 6; i++) {                                                           // for循环设置
      int tempVal = oneRun_e[i] - 1;                                                        // 下标
      digitalWrite(airPumpPin[tempVal], airPumpON);                                         // 气泵开启 开启充气
      digitalWrite(airValvePin[tempVal], airValveOFF);                                      // 气阀关闭 停止放气
    }
    for (int i = 0; i < 3; i++) {                                                           // for循环设置
      int tempVal = twoRun_e[i] - 1;                                                        // 下标
      digitalWrite(airPumpPin[tempVal], airPumpOFF);                                        // 气泵关闭 停止充气
      digitalWrite(airValvePin[tempVal], airValveON);                                       // 气阀开启 开启放气
    }
  } else {
    for (int i = 0; i < 6; i++) {                                                           // for循环设置
      int tempVal = oneRun_e[i] - 1;                                                        // 下标
      digitalWrite(airPumpPin[tempVal], airPumpOFF);                                        // 气泵关闭 停止充气
      digitalWrite(airValvePin[tempVal], airValveON);                                       // 气阀开启 开启放气
    }
    for (int i = 0; i < 3; i++) {                                                           // for循环设置
      int tempVal = twoRun_e[i] - 1;                                                        // 下标
      digitalWrite(airPumpPin[tempVal], airPumpON);                                         // 气泵开启 开启充气
      digitalWrite(airValvePin[tempVal], airValveOFF);                                      // 气阀关闭 停止放气
    }
  }
}

// 运行模式状态06
void modelRun06() {
  if (distance < setMinDisData) {                                                           // 充气
    for (int i = 0; i < airPumpCont; i++) {
      digitalWrite(airPumpPin[i], airPumpON);                                               // 气泵开启 开启充气
      digitalWrite(airValvePin[i], airValveOFF);                                            // 气阀关闭 停止放气
    }
  } else if(distance > setMaxDisData){                                                      // 放气
    for (int i = 0; i < airPumpCont; i++) {
      digitalWrite(airPumpPin[i], airPumpOFF);                                              // 气泵关闭 停止充气
      digitalWrite(airValvePin[i], airValveON);                                             // 气阀开启 开启放气
    }
  } else {
    for (int i = 0; i < airPumpCont; i++) {
      digitalWrite(airPumpPin[i], airPumpOFF);                                              // 气泵关闭 停止充气
      digitalWrite(airValvePin[i], airValveOFF);                                            // 气阀关闭 停止放气
    }    
  }
}

// 运行模式状态07
void modelRun07() {
  if (millis() - modelRunTime >= inflateTime_g) {
    modelRunTime = millis();
    for (int i = 0; i < 3; i++) {
      if (i == runSubscript) {
        for (int j = 0; j < 3; j++) {
          int tempVal = runGroup[i][j] - 1;                                                 // 下标
          digitalWrite(airPumpPin[tempVal], airPumpON);                                     // 气泵开启 开启充气
          digitalWrite(airValvePin[tempVal], airValveOFF);                                  // 气阀关闭 停止放气
        }
      } else {
        for (int j = 0; j < 3; j++) {
          int tempVal = runGroup[i][j] - 1;                                                 // 下标
          digitalWrite(airPumpPin[tempVal], airPumpOFF);                                    // 气泵关闭 停止充气
          digitalWrite(airValvePin[tempVal], airValveON);                                   // 气阀开启 开启放气
        }
      }
    }
    if (++runSubscript >= 3) {                                                              // 重置
      runSubscript = 0;
    }
  }
}

// 运行模式状态08
void modelRun08() {
  if (millis() - modelRunTime >= chooseTime) {
    modelRunTime = millis();
    randomSeed(analogRead(0));                                                              // 生成随机种子 避免重复随机数
    int tempVal = random(0, 9);                                                             // 设置随机数
    if (runSubscript == 0) {
      chooseData[0] = tempVal;
      digitalWrite(airPumpPin[tempVal], airPumpON);                                         // 气泵开启 开启充气
      digitalWrite(airValvePin[tempVal], airValveOFF);                                      // 气阀关闭 停止放气
      runSubscript++;
    } else {
      bool tempFalg = true;
      for (int i = 0; i < runSubscript; i++) {
        if (tempVal == chooseData[i]) {
          tempFalg = false;
        }
      }
      if (tempFalg == true) {
        chooseData[runSubscript] = tempVal;
        digitalWrite(airPumpPin[tempVal], airPumpON);                                       // 气泵开启 开启充气
        digitalWrite(airValvePin[tempVal], airValveOFF);                                    // 气阀关闭 停止放气
        if (++runSubscript >= 3) {
          runSubscript = 0;
          for (int i = 0; i < airPumpCont; i++) {
            digitalWrite(airPumpPin[i], airPumpOFF);                                        // 气泵关闭 停止充气
            digitalWrite(airValvePin[i], airValveON);                                       // 气阀开启 开启放气
          }
        }
      }
    }
  }
}





// 获取超声波数据
void getDistance() {
  if (millis() - getDistanceTime > getDataInterval) {  // 每隔s获取一次超声波模块的距离数据
    getDistanceTime = millis();
    digitalWrite(trigPin, LOW);                        // 低高低电平发一个短时间脉冲去trigPin
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    distance = float( pulseIn(echoPin, HIGH) * 17 ) / 1000;
    if (distance < 0 || distance > maxDistance) {
      distance = maxDistance;
    }
    //读取一个引脚的脉冲（HIGH或LOW）。例如，如果value是HIGH，pulseIn()会等待引脚变为HIGH，开始计时，再等待引脚变为LOW并停止计时。
    //接收到的高电平的时间(us)*340m/s/2=接收到高电平的时间(us)*17000cm/1000000us = 接收到高电平的时间*17/1000(cm)
    Serial.print(distance);
    Serial.println("cm");
  }
}
