#include <Servo.h>

Servo pinceServo;  // Déclaration du servo qui contrôle la pince

int potPin = A0;    // Pin où est connecté le potentiomètre
int boutonPin = 2;   // Pin où est connecté le bouton pour mémoriser la position
int valPot;          // Variable pour stocker la valeur lue du potentiomètre
int anglePince;      // Variable pour stocker l'angle du servo
int positionMémorisée = 90; // Position mémorisée du servo, initialisée à 90° (neutre)

int pinServo = 9;    // Pin où est connecté le servo

void setup() {
  // Initialisation du servo
  pinceServo.attach(pinServo);

  // Initialiser le servo à la position mémorisée
  pinceServo.write(positionMémorisée);
  delay(1000);  // Attendre 1 seconde pour stabiliser

  // Initialisation du bouton en mode entrée
  pinMode(boutonPin, INPUT);
}

void loop() {
  // Lire la valeur du potentiomètre (entre 0 et 1023)
  valPot = analogRead(potPin);
  
  // Mapper la valeur lue pour obtenir un angle entre 0 et 180
  anglePince = map(valPot, 0, 1023, 0, 180);
  
  // Déplacer la pince à l'angle calculé
  pinceServo.write(anglePince);
  
  // Vérifier si le bouton est appuyé pour mémoriser la position
  if (digitalRead(boutonPin) == HIGH) {
    positionMémorisée = anglePince;  // Mémoriser la position actuelle de la pince
    delay(500);  // Attendre un peu pour éviter plusieurs mémorisations rapides
  }
  
  // Option pour remettre la pince à la position mémorisée (par exemple, avec un délai ou un autre bouton)
  // Exemple : Remise en position mémorisée quand un bouton est appuyé.
  if (digitalRead(boutonPin) == LOW) {
    pinceServo.write(positionMémorisée);  // Retour à la position mémorisée
    delay(500);  // Attendre un peu pour stabiliser
  }

  delay(15);  // Attendre un peu pour permettre au servo de se déplacer
}
