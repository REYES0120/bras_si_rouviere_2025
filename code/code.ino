const int buttonPin = 2;// Pin pour le bouton
const int servoPin=6;

int elevservo;
void setup() {
  pinMode(buttonPin, INPUT);  // Déclare le pin du bouton comme entrée
  elevservo.attach(servoElevPin);  
  elevServo.write(storedElevAngle);  
  delay(1000);  
}

void loop() {
  // Vérifie si le bouton est pressé pour démarrer le retour lent
  if (digitalRead(buttonPin) == HIGH) {
    startMovingToStoredPosition();  // Démarre la remise en position lente
  }
  
  // Autres opérations de contrôle du bras
  if (!isMovingToStoredPosition) {
    potElevValue = analogRead(potElevPin);  
    elevAngle = map(potElevValue, 0, 1023, 0, 150);  
    elevServo.write(elevAngle);  
  } else {
    moveToStoredPosition();  
  }
}
