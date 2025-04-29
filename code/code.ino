/*
LYCEE ROUVIERE - année 2024-2025
TP SI - Bras Robot
Gabriel Chassaniol
Leo Cuillerier
Mathieu Forestier
Ryan Gehin
*/

#include <Servo.h>
#include "rgb_lcd.h"
Servo servoBase, servoPince, servoBras1, servoBras2;
rgb_lcd lcd;
// ********************
// Affectation des pin
// ********************
// analog pins
const int potBasePin = A0;  
const int potPincePin = A1; 
const int potXPin = A2;     
const int potYPin = A3;     

// digital pins
const int servoBasePin = 3;
const int servoPincePin = 5;
const int servoBras1Pin = 6;
const int servoBras2Pin = 9;
const int boutonMemPin = 8;
const int boutonRestaurerPin = 7;

// ************************************************
// Fenêtre XY dans laquelle le bras peut évoluer
// Le bras sera arrêté à la limite de cette fenêtre.
// 
// Par défault, le bras est considéré "out of bounds"
// pour éviter un déplacement inattendu avant le 
// premier calcul de X et Y.
// ************************************************
const float X_MIN = 8.0;
const float X_MAX = 24.0;
const float Y_MIN = -13.0;
const float Y_MAX = 24.0;  

// *****************************************************
// Constantes d'offset pour ALPHA et BETA
// Ces constantes permet d'utiliser
// toute l'amplitude 0-180deg des servo moteur.
// alpha = ALPHA_OFFSET + calculAlpha(x,y)
// beta = BETA_OFFSET + calculBeta(x,y)
// -> Cf. analyse dédiée
// *****************************************************
const float ALPHA_OFFSET = 4.345;
const float BETA_OFFSET = 1.203;  


// *****************************************************
// potentiomètre retourne un signal analogique 
// encodé entre 0 et 1023
// *****************************************************

const float POT_VAL_MAX = 1023;
//const float POT_VAL_MAX = 255;

// *****************************************************
// Amplitude du servo = 180 deg. 
// *****************************************************

const int ANGLE_SERVO_MAX = 180;

// *****************************************************
// mémorisation des valeurs de X et Y pour comparaison 
// avec les valeurs courantes et identifier un éventuel 
// changement
// *****************************************************
const float LAST_XY_INIT_VALUE = -999;
const float LAST_BRAS_INIT_VALUE = -999;
float lastX, lastY, lastXBras, lastYBras;
int lastBras1Pos, lastBras2Pos;

bool s_cligno = false;
double nextCligno = 0;
const double CLIGNO_DELAY = 200000; // in micro sec. 

// Variables pour la mémorisation/restauration de position
bool etatRestaurerPrecedent = LOW;
float memX = 0;
float memY = 0;
int memZ = 0; // Z = base
bool hasMemorized = false;

//************************************************************
// Cette fonction calcule alpha et beta en fonction de x et y
// Cf. analyse détaillée.
//************************************************************

void computeAlphaBeta(float x, float y, float &alpha, float &beta) {

    float x2_y2 = sqrt(x*x + y*y) / 25.0;
    float z_atan = atan2(x,y);
    float z_asin = asin(x2_y2);
    float alpha_rad = -z_atan - z_asin + ALPHA_OFFSET;
    float beta_rad = z_atan - z_asin + BETA_OFFSET;    
    alpha = degrees(alpha_rad);
    beta = degrees(beta_rad);

}

/*
  Fonction Setup() de l'arduino.
  initialisation des servomoteurs
  initialisation du clignotement
  initialisation de l'historisation
*/
void setup() {
    servoBase.attach(servoBasePin);
    servoPince.attach(servoPincePin);
    servoBras1.attach(servoBras1Pin);
    servoBras2.attach(servoBras2Pin);
    pinMode(boutonMemPin , INPUT_PULLUP);
    pinMode(boutonRestaurerPin , INPUT_PULLUP);
    Serial.begin(9600);
    nextCligno = micros() + CLIGNO_DELAY;
    lastX = LAST_XY_INIT_VALUE;
    lastY = LAST_XY_INIT_VALUE;
    lastBras1Pos = LAST_BRAS_INIT_VALUE;
    lastBras2Pos = LAST_BRAS_INIT_VALUE;
    lastXBras = LAST_XY_INIT_VALUE;
    lastYBras = LAST_XY_INIT_VALUE;
}

// Fonction pour enregistrer une position et pouvoir y revenir à tout moment //

void lireBoutonsMemoire(bool &etatMem, bool &etatRestaurer) {
    etatMem = digitalRead(boutonMemPin);
    etatRestaurer = digitalRead(boutonRestaurerPin);
/*
    if (etatMem == HIGH && etatMemPrecedent == LOW) {
        memX = x;
        memY = y;
        memZ = z;
        Serial.println("Position (x, y, z) enregistrée.");
    }

    if (etatRestaurer == HIGH && etatRestaurerPrecedent == LOW) {
        float alpha = 0, beta = 0;
        computeAlphaBeta(memX, memY, alpha, beta);
        servoBase.write(memZ);
        servoBras1.write(posBras1);
        servoBras2.write(posBras2);
        Serial.println("Position (x, y, z) restaurée.");
    }

    etatMemPrecedent = etatMem;
    etatRestaurerPrecedent = etatRestaurer;
*/
}

/*
  Fonction de mise à jour des positions des servo moteurs en fonction : 
   - des valeurs de alpha et beta retournés par computeAlphaBeta(x, y)
   - de la valeurs des potentiomètres dans l'intervale d'évolution du bras
*/
void computeCombinedAlphaBeta(float x, float y, bool out_of_bounds, bool positionChanged, int &sBras1Pos, int &sBras2Pos, float &x_bras, float &y_bras){

  // test si la valeur est dans la plage de valeurs accessibles par le bras
  if (!out_of_bounds) {
    
    // optimisation : on ne lance le calcul que si la position a changé.
    if (positionChanged){
        float alpha = 0, beta = 0;
        x_bras = x;
        y_bras = y;
        computeAlphaBeta(x_bras, y_bras, alpha, beta);
        sBras1Pos = constrain(alpha, 1, ANGLE_SERVO_MAX-1);
        sBras2Pos = constrain(beta, 1, ANGLE_SERVO_MAX-1);

    }else{
        // position inchangée -> on reste sur les données historisées 
        sBras1Pos = lastBras1Pos;
        sBras2Pos = lastBras2Pos;
        x_bras = lastXBras;
        y_bras = lastYBras;

    }  
  } else {
      // Cas out of range.

      // test si les potentiometres donnent une valeur out of range à l'init
      // il faut positionner le bras à la position la plus proche
      
      if (lastX == LAST_XY_INIT_VALUE) {

        float alpha = 0, beta = 0;
        float coef = computeCoef(x, y);

        x_bras = x / coef;
        y_bras = y / coef;
        computeAlphaBeta(x_bras, y_bras, alpha, beta);
        sBras1Pos = constrain(alpha, 1, ANGLE_SERVO_MAX-1);
        sBras2Pos = constrain(beta, 1, ANGLE_SERVO_MAX-1);
        
      }else{
        // cas potentiomètres déjà actionés depuis le début de l'éxpérience
        // la position doit rester inchangée -> on reste sur la derniere position historisée 
        sBras1Pos = lastBras1Pos;
        sBras2Pos = lastBras2Pos;
        x_bras = lastXBras;
        y_bras = lastYBras;
      }
  }

  // on finit par l'historisation
  lastX = x;
  lastY = y;
  lastBras1Pos = sBras1Pos;
  lastBras2Pos = sBras2Pos;
  lastXBras = x_bras;
  lastYBras = y_bras;

}

/*
  Fonction calculant le rapport entre la limite accessible par le bras et la position souhaitée
  Cette fonction ne peut jamais retourner 0 puisque y est toujours différent de 0.
*/
float computeCoef(float x, float y){
  return sqrt((x*x + y*y)/(25*25));
}

/*
  Fonction convertissant les valeurs des potentiomètres en x et y
  La fonction ne retourne jamais y=0.
*/
void computeXYfromPot(int potX, int potY, float &x, float &y, bool &out_of_range, bool &positionChanged) {
    x = map(potX, 0, POT_VAL_MAX, X_MIN * 100, X_MAX * 100);
    x=x*0.01;
    y = map(potY, 0, POT_VAL_MAX, Y_MIN * 100, Y_MAX * 100); 
    y=y*0.01;
    if (y==0) y=0.001;
    (computeCoef(x,y) < 1) ? out_of_range = false : out_of_range = true ;
    (x < lastX - 0.1 || x > lastX + 0.1 || y < lastY - 0.1 || y > lastY + 0.1 ) ? positionChanged = true : positionChanged = false;
    
}

void readPotentiometer(int &potBaseVal, int &potPinceVal, int &potXVal, int &potYVal) {
    potBaseVal = analogRead(potBasePin);
    potPinceVal = analogRead(potPincePin);
    potXVal = analogRead(potXPin);
    potYVal = analogRead(potYPin);
}

/*
  Fonction de calcul de l'angle Base et Pince à partir de la valeur
  des potentiomètres.
*/
void computeBaseEtPince(int potBaseVal, int potPinceVal, int &potBasePos, int &potPincePos){

  potBasePos = map(potBaseVal, 0, POT_VAL_MAX, 0, ANGLE_SERVO_MAX);
  potPincePos = map(potPinceVal, 0, POT_VAL_MAX, 0, ANGLE_SERVO_MAX);

}

/*
  Fonction d'écriture des consignes vers les servomoteurs base, pince, bras1 et bras2.
  Les consignes sont des entiers, exprimés en degrés.
*/
void writeServo(int potBasePos, int potPincePos,int servoBras1Pos, int servoBras2Pos){
    servoBase.write(potBasePos);
    servoPince.write(potPincePos);
    servoBras1.write(servoBras1Pos);
    servoBras2.write(servoBras2Pos);
}

/*
  Fonction de clignotement
  La fonction fait basculer le booléen s_cligno lorsque le délai CLIGNO_DELAY est écoulé
*/
void cligno(){
  if (nextCligno <= micros()) {
    s_cligno = !s_cligno;
    nextCligno = micros() + CLIGNO_DELAY;
  }
}

void loop() {
    // variables utilisées dans la loop() : 
    int potBaseVal = 0, potPinceVal = 0, potXVal = 0, potYVal = 0;
    int servoBras1Pos = 0, servoBras2Pos = 0, potBasePos = 0, potPincePos = 0; 
    float bras_x = 0, bras_y = 0;
    bool out_of_bounds = false;
    bool positionChanged = false;
    bool etatMem = false, etatRestaurer = false;

    lireBoutonsMemoire(etatMem, etatRestaurer);

    // Lecture des potentiomètres
    readPotentiometer(potBaseVal, potPinceVal, potXVal, potYVal);
    // Calcul de x et y réels à partir des valeurs des potentiomètres XVal et YVal
    float x = 0, y = 0;     
    computeXYfromPot(potXVal, potYVal, x, y, out_of_bounds, positionChanged);


    // Base et Pince
    computeBaseEtPince(potBaseVal, potPinceVal, potBasePos, potPincePos);

    // Calcul des positions servoBras1Pos et servoBras2Pos
    computeCombinedAlphaBeta(x,y, out_of_bounds, positionChanged, servoBras1Pos, servoBras2Pos, bras_x, bras_y);

    // Calcul de l'etat du bras
    if (etatMem == LOW){
      memX = servoBras1Pos;
      memY = servoBras2Pos;
      memZ = potBasePos;
      Serial.print("Position (");
      Serial.print(memX);
      Serial.print(" , ");
      Serial.print(memY);
      Serial.print(" , ");
      Serial.print(memZ);
      Serial.println(" ) enregistrée.");
      hasMemorized = true;
    }

    if (etatRestaurer == LOW && etatRestaurerPrecedent == HIGH && hasMemorized) {
        writeServo(memZ, potPincePos, memX, memY);
        Serial.print("Position (");
        Serial.print(memX);
        Serial.print(" , ");
        Serial.print(memY);
        Serial.print(" , ");
        Serial.print(memZ);
        Serial.println(" ) restaurée.");
        etatRestaurerPrecedent = LOW;
    } else {
              Serial.println(positionChanged);
    etatRestaurerPrecedent = HIGH;
    // Affichage des positions de la base, de la pince et du bras
    // fonction par défaut utilisant Serial.print()
    if (positionChanged || out_of_bounds) displayPosition(potBasePos, potPincePos, servoBras1Pos, servoBras2Pos, bras_x, bras_y, out_of_bounds, s_cligno);
    //displayPosition(potBasePos, potPincePos, servoBras1Pos, servoBras2Pos, bras_x, bras_y, out_of_bounds, s_cligno);

    // Ecriture vers les servos si les valeurs calculées sont dans les valeurs autorisées
    if (positionChanged && !out_of_bounds) writeServo(potBasePos, potPincePos, servoBras1Pos, servoBras2Pos);

    }

    // mise à jour clignotement (variable globale s_cligno)
    cligno();
    delay(200);
}

/*
  Fonction d'affichage par défaut, utilisant la fonction Serial.print().
*/
void displayPosition(int baseVal, int pinceVal, int bras1Val, int bras2Val, float xBras, float yBras, bool out_of_bounds, bool clign){

// affichage du clignotement
        if (out_of_bounds) {
          clign ? Serial.print("!!") : Serial.print("**");
        } else {
          Serial.print("  ");
        }

// affichage des données
        Serial.print(" >> base  = ");
        Serial.print(baseVal);
        Serial.print(" deg. , pince = ");
        Serial.print(pinceVal);    
        Serial.print(" deg. , bras1 (alpha) = ");
        Serial.print(bras1Val);
        Serial.print(" deg. , bras2 (beta) = ");
        Serial.print(bras2Val);
        Serial.print(" deg. , x = ");
        Serial.print(xBras);
        Serial.print(" cm , y = ");
        Serial.print(yBras);
        Serial.println(" cm.");
}
