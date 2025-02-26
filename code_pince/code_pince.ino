#include <Servo.h> 

Servo pinceServo; // Déclaration du servo qui contrôle la pince 
int potPin = A0; // Pin où est connecté le potentiomètre
int valPot; // Variable pour stocker la valeur lue du potentiomètre 
int anglePince; // Variable pour convertir la valeur du potentiomètre en angle pour le servo 
int pinServo = 9; // Pin où est connecté le servo 

void setup() { 
  pinceServo.attach(pinServo); // Attacher le servo au pin 
  pinceServo.write(90); // Initialiser le servo à une position neutre 
  delay(1000); // Attendre un peu pour stabiliser 
} // La parenthèse fermante manquait ici pour la fonction setup()

void loop() { 
  valPot = analogRead(potPin); // Lire la valeur du potentiomètre (entre 0 et 1023)
  anglePince = map(valPot, 0, 1023, 10, 170); // Mapper la valeur lue pour obtenir un angle de 0 à 180 
  pinceServo.write(anglePince); // Déplacer la pince à l'angle calculé
  delay(15); // Attendre un peu pour permettre au servo de se déplacer
}
