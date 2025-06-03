#include <VoiceRecognitionV3.h>
#include <Servo.h>
#include <SoftwareSerial.h>


// 步进电机引脚定义
const int stepX = 2;
const int dirX = 5;
const int stepY = 3;
const int dirY = 6;
const int enPin = 8;

// 舵机
Servo penServo;

// 当前位置记录
float posX = 0;
float posY = 0;
const int XmaxSteps = 584;
const int YmaxSteps = 278;
const float stepsPermm = 0.254;

// 语音识别模块
SoftwareSerial mySerial(9,10);  // Elechouse TX->D9, RX->D10
VR myVR(9,10);
uint8_t records[] = {0, 1, 2, 3, 4};  // 所有语音指令编号
uint8_t buf[64];

// 指令编号定义
#define CMD_CIRCLE   0
#define CMD_SQUARE   1
#define CMD_RESET         2
#define CMD_PENUP        3
#define CMD_PENDOWN      4

void setup() {
  Serial.begin(9600);
  myVR.begin(9600);

  pinMode(stepX, OUTPUT);
  pinMode(dirX, OUTPUT);
  pinMode(stepY, OUTPUT);
  pinMode(dirY, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);

  penServo.attach(11);
  penUp();

  // 语音模块加载指令
  if (myVR.load(records, 5) >= 0) {
    Serial.println("语音命令加载成功！");
  } else {
    Serial.println("语音命令加载失败！");
  }
  digitalWrite(dirX, HIGH);
  digitalWrite(dirY, HIGH);
  float posX = 0;
  float posY = 0;
}

void loop() {
  int ret = myVR.recognize(buf, 50);
  if (ret > 0) {
    uint8_t command = buf[1];
    Serial.print("识别到命令 ID:");
    Serial.println(command);

    switch (command) {
      case CMD_CIRCLE:
        drawCircle(0, 55, 20);
        break;
      case CMD_SQUARE:
        drawSquare(0, 40, 30);
        break;
      case CMD_RESET:
        moveTo(0, 0);
        break;
      case CMD_PENUP:
        penUp();
        break;
      case CMD_PENDOWN:
        penDown();
        break;
      default:
        Serial.println("未知命令");
        break;
    }
  }
if (Serial.available()){
    String line = Serial.readStringUntil('/n');
    line.trim();
    if (line.equals("circle")){
      drawCircle(0, 55, 20);
    }else if(line.equals("square")){
      drawSquare(0, 40, 30);
    }
  
  }
}

void drawCircle(float Xo, float Yo, float r) {
  penDown();
  for (int angle = 0; angle <= 360; angle += 5) {
    float rad = radians(angle);
    float x = Xo + r * cos(rad);
    float y = Yo + r * sin(rad);
    moveTo(x, y);
  }
  penUp();
}

void drawSquare(float x0, float y0, float size) {
  penDown();
  moveTo(x0, y0);
  moveTo(x0 + size, y0);
  moveTo(x0 + size, y0 + size);
  moveTo(x0, y0 + size);
  moveTo(x0, y0);
  penUp();
}

void moveTo(float targetX, float targetY) {
  float targetposXSteps = targetX / stepsPermm;
  float targetposYSteps = targetY / stepsPermm;
  moveAxis(stepX, dirX, posX, targetposXSteps);
  moveAxis(stepY, dirY, posY, targetposYSteps);
  posX = targetposXSteps;
  posY = targetposYSteps;
}

void moveAxis(int stepPin, int dirPin, float currentPos, float targetPos) {
  if (currentPos < targetPos) {
    digitalWrite(dirPin, HIGH);
  } else {
    digitalWrite(dirPin, LOW);
  }

  int steps = abs(targetPos - currentPos);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
}

void penUp() {
  penServo.write(15);  // 抬笔角度
  delay(500);
}

void penDown() {
  penServo.write(0);   // 放笔角度
delay(500);
}