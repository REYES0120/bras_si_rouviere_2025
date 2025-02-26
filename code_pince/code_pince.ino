#include <Servo.h> 
Servo pinceServo; // Déclaration du servo qui contrôle la pince 
int potPin = A0; // Pin où est connecté le potentiomètre
int valPot; // Variable pour stocker la valeur lue du potentiomètre 
int anglePince; // Variable pour convertir la valeur du potentiomètre en angle pour le servo 
int pinServo = 9; // Pin où est connecté le servo 

void setup() { // Attacher le servo au pin 
  pinceServo.attach(pinServo); // Initialiser le servo à une position neutre 
  pinceServo.write(90); // Position de départ (fermé ou ouvert selon le type de pince)
  delay(1000); // Attendre un peu pour stabiliser } 



void loop() { // Lire la valeur du potentiomètre (entre 0 et 1023) 
    valPot = analogRead(potPin); // Mapper la valeur lue pour obtenir un angle de 0 à 180 
    anglePince = map(valPot, 0, 1023, 10, 170); // Déplacer la pince à l'angle calculé 
    pinceServo.write(anglePince); // Attendre un peu pour permettre au servo de se déplacer
    delay(15); } 
  }
}
