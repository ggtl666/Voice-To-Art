#include<SoftwareSerial.h>
#include <Servo.h>


// 步进电机引脚定义
const int stepX = 2;
const int dirX = 5;
const int stepY = 3;
const int dirY = 6;
const int enPin = 8;

// 舵机
Servo penServo;
SoftwareSerial blueTooth(11,255);

// 当前位置记录
float posX = 0;
float posY = 0;
const int XmaxSteps = 584;
const int YmaxSteps = 278;
const float stepsPermm = 0.254;
const float mmPerstep=3.937;

void setup() {
  Serial.begin(9600);
  blueTooth.begin(9600);
  
  pinMode(stepX, OUTPUT);
  pinMode(dirX, OUTPUT);
  pinMode(stepY, OUTPUT);
  pinMode(dirY, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);

  penServo.attach(10);
  penUp();

  digitalWrite(dirX, HIGH);
  digitalWrite(dirY, HIGH);
  float posX = 0;
  float posY = 0;
}

void loop() {
if (blueTooth.available()){
  String line = blueTooth.readStringUntil('\n');
    line.trim();
    line.toLowerCase();
    Serial.println("bluetooth received:" + line);
    if (line.equals("circle")){
      Serial.println("draw a circle");
      drawCircle(0, 0, 20);
    }else if(line.equals("square")){
      Serial.println("draw a square");
      drawSquare(0, 0, 30);
    }else if(line.equals("triangle")){
      Serial.println("draw a triangle");
      drawTriangle(0, 0, 20);
    }else if(line.equals("parabola")){
      Serial.println("draw apa parabola");
      drawParabola(-20,40);
    }
}
if (Serial.available()){
    String line = Serial.readStringUntil('\n');
    line.trim();
    line.toLowerCase();
    Serial.println("received:" + line);
    if (line.equals("circle")){
      Serial.println("draw a circle");
      drawCircle(0, 0, 20);
    }else if(line.equals("square")){
      Serial.println("draw a square");
      drawSquare(0, 0, 30);
    }else if(line.equals("parabola")){
      Serial.println("draw a parabola");
      drawParabola(-20,40);
    }else if(line.equals("check")){
      Serial.println("checking the Servo");
      penUp();
      penDown();
    }
  }
}
void drawCircle(float Xo, float Yo, float r) {
  penDown();
  for (int angle = 0; angle <= 360; angle += 5) {
    float rad = radians(angle);
    float x = Xo + r * cos(rad);
    float y = Yo + r * sin(rad);
    moveTo(x,y);
  }
  penUp();
  moveTo(Xo,Yo);
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

void drawTriangle(float x0,float y0, float sideLength){
  penDown();
  (x0,y0,x0 + sideLength,y0);
  drawLine(x0 + sideLength,y0,x0 + sideLength/2 ,y0 + sqrt(sideLength*sideLength-(sideLength/2)*(sideLength/2)));
  drawLine(x0 + sideLength/2 ,y0 + sqrt(sideLength*sideLength-(sideLength/2)*(sideLength/2)),x0,y0);
  penUp();
}

void drawParabola(float x, float y){
  penDown();
  for (int i=0; i <= 80; i +=1){
    x += 0.5;
    y = 0.1*x*x;
    moveTo(x,y);
  }
  penUp();
}

void drawLine(float x1,float y1,float x2,float y2){
  float dx = x2-x1;float dy = y2-y1;
  float length = sqrt(dx*dx + dy*dy);
  float steps = length / mmPerstep;
  for (int i =0;i <= steps;i++){
    float x = x1+ i * mmPerstep;
    float y = y1+ i * mmPerstep;
    moveTo(x,y);  }
}

void moveTo(float targetX, float targetY) {
  float targetposXSteps = targetX / stepsPermm;
  float targetposYSteps = targetY / stepsPermm;

  int dx = targetposXSteps - posX;
  int dy = targetposYSteps - posY;

  int stepsX = abs(dx);
  int stepsY = abs(dy);
  int dirXVal = dx > 0 ? HIGH : LOW;
  int dirYVal = dy > 0 ? HIGH : LOW;

  digitalWrite(dirX, dirXVal);
  digitalWrite(dirY, dirYVal);

  int countX = 0;
  int countY = 0;
  int maxSteps = max(stepsX, stepsY);

  for (int i = 0; i < maxSteps; i++) {
    if (countX < stepsX) {
      digitalWrite(stepX, HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepX, LOW);
      delayMicroseconds(2000);
      countX++;
    }

    if (countY < stepsY) {
      digitalWrite(stepY, HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepY, LOW);
      delayMicroseconds(2000);
      countY++;
    }
  }

  posX = targetposXSteps;
  posY = targetposYSteps;
}


void penUp() {
  penServo.write(15);  // 抬笔角度
  delay(1000);
}

void penDown() {
  penServo.write(0);   // 放笔角度
delay(1000);
}
