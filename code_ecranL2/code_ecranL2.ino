#include <rgb_lcd.h>
#include <Servo.h>

int Tourelle;
int Hauteur;
int Extension;
int TourelleM;
int HauteurM;
int ExtensionM;
rgb_lcd lcd;
int boutonpoussoir;




void setup() {
lcd.begin(16,2);
}







void loop() {
  if (boutonpoussoir == 1){
  Tourelle = TourelleM;
  Hauteur = HauteurM;
  Extension = ExtensionM;
}
  lcd.clear();
  lcd.setCursor(0,1);
  Serial.print("M");
  lcd.setCursor(2,1);
  Serial.print("E");
  lcd.setCursor(3,1);
  Serial.print(ExtensionM);
  lcd.setCursor(7,1);
  Serial.print("H");
  lcd.setCursor(8,1);
  Serial.print(HauteurM);
  lcd.setCursor(12,1);
  Serial.print("T");
  lcd.setCursor(13,1);
  Serial.print(TourelleM);
}
