#include <Servo.h>

Servo servoBase, servoPince, servoBras1, servoBras2;

const int potBasePin = A0;  
const int potPincePin = A1; 
const int potXPin = A2;     
const int potYPin = A3;     

const int servoBasePin = 3;
const int servoPincePin = 5;
const int servoBras1Pin = 6;
const int servoBras2Pin = 9;

const float X_MIN = 8.0;
const float X_MAX = 22.5;
const float Y_MIN = -10.0;
const float Y_MAX = 10.0;  

float lastX = -999, lastY = -999;  
float alpha = 0, beta = 0;          

void calculateAlphaBeta(float x, float y, float &alpha, float &beta) {
    float d4 = atan2(x, y) - asin(sqrt(x*x + y*y) / 25.0);
    float e4 = atan2(x, y) + asin(sqrt(x*x + y*y) / 25.0);
    alpha = degrees(d4) + (y > 0 ? 180.0 : 0);
    beta = degrees(e4) + (y > 0 ? 180.0 : 0);
}

void setup() {
    servoBase.attach(servoBasePin);
    servoPince.attach(servoPincePin);
    servoBras1.attach(servoBras1Pin);
    servoBras2.attach(servoBras2Pin);
}

void loop() {
    // Lecture des potentiomètres
    int potBaseVal = analogRead(potBasePin);
    int potPinceVal = analogRead(potPincePin);
    int potXVal = analogRead(potXPin);
    int potYVal = analogRead(potYPin);


  int x = map(potXVal, 0, 1023, X_MIN * 10, X_MAX * 10) / 10.0; 
  int y = map(potYVal, 0, 1023, Y_MIN * 10, Y_MAX * 10) / 10.0; 



    if (x != lastX || y != lastY) {
        calculateAlphaBeta(x, y, alpha, beta);
        lastX = x;
        lastY = y;
    }


    int servoBras1Pos = constrain(alpha, 0, 265);
    int servoBras2Pos = constrain(beta, 0, 265);


    servoBase.write(map(potBaseVal, 0, 1023, 0, 265));
    servoPince.write(map(potPinceVal, 0, 1023, 0, 265));
    servoBras1.write(servoBras1Pos);
    servoBras2.write(servoBras2Pos);

    delay(10);
}
