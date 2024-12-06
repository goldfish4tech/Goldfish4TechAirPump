/**
   按钮。
   按一下充气，再按一下停止。
   按另一个放气，在按一下停止放气。

   视频链接：
   https://www.bilibili.com/video/BV1Fu4m1G7eG

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/

// 气泵 ------------------------------------------------------------------------------
#define airPumpPin            5                                                       // 控制气泵充气 引脚
#define airValvePin           6                                                       // 控制气阀放气 引脚
#define airPumpON             HIGH                                                    // 气泵打开 开启充气
#define airPumpOFF            !airPumpON                                              // 气泵关闭 停止充气
#define airValveON            HIGH                                                    // 气阀打开 开启放气
#define airValveOFF           !airValveON                                             // 气阀关闭 停止放气

// 按钮 ------------------------------------------------------------------------------
#define debounceDelay         20                                                      // 间隔时间为20毫秒
int buttonPin[2] = {                                                                  // 控制按钮状态 右充 左放
  4, 7
};
bool buttonFlag[2] = {HIGH, HIGH};                                                    // 按键状态
bool lastButtonFlag[2] = {HIGH, HIGH};                                                // 上一次按键
unsigned long lastDebounceTime[2] = {0, 0};                                           // 按键按下去的初始时间
bool runState[2] = {false, false};                                                    // 控制运行状态

void setup() {
  Serial.begin(9600);                                                                 // 设置波特率9600
  // 气泵设置 -------------------
  pinMode(airPumpPin, OUTPUT);                                                        // 设置引脚输出
  pinMode(airValvePin, OUTPUT);                                                       // 设置引脚输出
  digitalWrite(airPumpPin, airPumpOFF);                                               // 初始状态 关闭气泵 停止充气
  digitalWrite(airValvePin, airValveOFF);                                             // 初始状态 气阀关闭 关闭放气
  // 按钮设置 -------------------
  for (int i = 0; i < 2; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);                                              // 设置按键模式为上拉输入
  }
  // 启动提示 -------------------
  Serial.println("Started successfully");
}

void loop() {
  getbutton();                                                                        // 获取按钮状态

  // loop回括号
}

void getbutton() {
  for (int i = 0; i < 2; i++) {
    int reading = digitalRead(buttonPin[i]);                                          // 起始状态为1（高电平）
    if (reading != lastButtonFlag[i]) {                                               // 当状态发生改变，给时间赋值
      lastDebounceTime[i] = millis();                                                 // 并记录时间
    }
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {                           // 判断时间
      if (reading != buttonFlag[i]) {                                                 // 当状态发生改变
        buttonFlag[i] = reading;                                                      // 赋值给buttonFlag
        if (buttonFlag[i] == LOW) {
          Serial.print(i + 1);
          Serial.println("press");
          runState[i] = !runState[i];                                                 // 状态取反
  
          // 根据按钮改变状态 控制气泵运行 --------------
          if (runState[i] == true) {
            if (i == 0) {
              digitalWrite(airPumpPin, airPumpON);                                    // 气泵开启 开启充气
            } else {
              digitalWrite(airValvePin, airValveON);                                  // 气阀开启 开启放气
            }
          } else {
            if (i == 0) {
              digitalWrite(airPumpPin, airPumpOFF);                                   // 气泵关闭 停止充气
            } else {
              digitalWrite(airValvePin, airValveOFF);                                 // 气阀关闭 停止放气
            }
          }
        }
      }
    }
    lastButtonFlag[i] = reading;
  }
}
