/**
   按钮。
   按一下充气，再按一下停止。
   按另一个放气，在按一下停止放气。

   视频链接：
   https://www.bilibili.com/video/BV1VoyoYTE7r

   Contact us
   Tel/WeChat: 18682388114
   email:      goldfish4tech@goldfish4tech.com
*/
#define buttonPin                 6                      //设置按键引脚为2
bool buttonFlag =                 true;                  //按键状态
bool lastButtonFlag =             true;                  //上一次按键
bool pressFlag =                  false;                 //按下时为true，松开为false
bool inflationFlag =              false;                 //为true时充气，为false时排气
unsigned long lastDebounceTime =  0;                     //按键按下去的初始时间
unsigned long debounceDelay =     50;                    //间隔时间为50毫秒
#define pumpPin                   5                      //充气引脚
#define svPin                     3                      //泄气引脚

/***************充气时间************/
#define inflationTime 10000                              //单位ms，1秒=1000ms，暂定都为10s
/***************放气时间************/
#define exhaustTime   8000                               //泄气时间  ，如需修改为20s，可以修改为20000

uint8_t state = 0x00;
unsigned long systemTime;

void setup() {
  Serial.begin(9600);                                    //设置波特率为9600
  pinMode(buttonPin, INPUT_PULLUP);                      //设置按键模式为上拉输入
  pinMode(pumpPin, OUTPUT);                              //充气引脚设置为输出模式
  digitalWrite(pumpPin, LOW);                            //开机后设置为低电平
  pinMode(svPin, OUTPUT);                                //泄气引脚设置为输出模式
  digitalWrite(svPin, LOW);                              //开机后设置为低电平
  systemTime = millis();                                 //记录时间
}

void loop() {
  getbutton();                                           //获取按键数据

  if (pressFlag == true) {                               //如果按钮按下
    pressFlag = false;
    inflationFlag = !inflationFlag;                      //反转状态为充气或者排气
    systemTime = millis();                               //记录时间
  }
  if (inflationFlag == true) {                           //为true时充气
    digitalWrite(pumpPin, 255);                          //开始充气
    digitalWrite(svPin, LOW);                            //关闭排气
    if (millis() - systemTime > inflationTime) {         //如果时间超过充气时间
      digitalWrite(pumpPin, LOW);                        //关闭充气
      digitalWrite(svPin, LOW);                          //关闭排气
    }
  } else {
    digitalWrite(pumpPin, LOW);                          //关闭充气
    digitalWrite(svPin, 255);                            //打开排气
    if (millis() - systemTime > exhaustTime) {           //如果时间超过排气时间
      digitalWrite(pumpPin, LOW);                        //关闭充气
      digitalWrite(svPin, LOW);                          //关闭排气
    }
  }
}

void getbutton() {
  int reading = digitalRead(buttonPin);                  //起始状态为1（高电平）
  if (reading != lastButtonFlag) {                       //当状态发生改变，给时间赋值
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonFlag) {                         //当状态发生改变
      buttonFlag = reading;                              //赋值给buttonFlag
      if (buttonFlag == false) {
        pressFlag = true;  //
        Serial.println("press");
      } else {
        pressFlag = false;
      }
    }
  }
  lastButtonFlag = reading;
}
