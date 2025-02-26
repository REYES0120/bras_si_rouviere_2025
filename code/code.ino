const int buttonPin = 2; // Pin pour le bouton
const int servoPin = 6;

#include <Servo.h>
Servo elevServo;

int storedElevAngle = 90; // Angle initial du servo
bool isMovingToStoredPosition = false;
int potElevValue;
int elevAngle;

void setup() {
  pinMode(buttonPin, INPUT); // Déclare le pin du bouton comme entrée
  elevServo.attach(servoPin);  
  elevServo.write(storedElevAngle);  
  delay(1000);  
}

void loop() {
  // Vérifie si le bouton est pressé pour démarrer le retour lent
  if (digitalRead(buttonPin) == HIGH) {
    startMovingToStoredPosition(); // Démarre la remise en position lente
  }
  
  // Autres opérations de contrôle du bras
  if (!isMovingToStoredPosition) {
    potElevValue = analogRead(A0); // Assurez-vous que le pin analogique est correct
    elevAngle = map(potElevValue, 0, 1023, 0, 150);  
    elevServo.write(elevAngle);  
  } else {
    moveToStoredPosition();  
  }
}

void startMovingToStoredPosition() {
  isMovingToStoredPosition = true;
}

void moveToStoredPosition() {
  // Logique pour déplacer lentement le servo à la position stockée
  int currentAngle = elevServo.read();
  if (currentAngle < storedElevAngle) {
    elevServo.write(currentAngle + 1);
  } else if (currentAngle > storedElevAngle) {
    elevServo.write(currentAngle - 1);
  } else {
    isMovingToStoredPosition = false;
  }
  delay(10); // Ajustez le délai pour contrôler la vitesse du mouvement
}
