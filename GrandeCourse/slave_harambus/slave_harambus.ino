#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10,11); // RX | TX
int bitIn1 = 5;
int bitIn2 = 6;
int bitIn3 = 7;
int bitOut1 = 8;
int bitOut2 = 9;
int bitOut3 = 2;
int out_value = 0;
int in_value = 0;
int digital = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(bitIn1, INPUT); //petit bit (1)
  pinMode(bitIn2, INPUT); //moyen bit (2)
  pinMode(bitIn3, INPUT); //gros bit  (4)
  pinMode(bitOut1, OUTPUT); //petit bit (1)
  pinMode(bitOut2, OUTPUT); //moyen bit (2)
  pinMode(bitOut3, OUTPUT); //gros bit  (4)
  digitalWrite(bitOut1, LOW);
  digitalWrite(bitOut2, LOW);
  digitalWrite(bitOut3, LOW);
  
  Serial.begin(9600);
  Serial.println("Slave module : BAUD 9600");
  BTSerial.begin(9600);  // HC-05 default speed in AT command more
}

void loop() {
  // Boucle principale et perpétuelle du arduino
  /////////////////////// \\\\\\\\\\\\\\\\\\\\\\
  // Convertir les 3 bits du RobUS à envoyer
  // Conversion du bit 1 : ( 00X )
  out_value = 0;
  if(digitalRead(bitIn1) == HIGH){
    out_value = out_value + 1;
  }
  // Conversion du bit 2 : ( 0X0 )
  if(digitalRead(bitIn2) == HIGH){
    out_value = out_value + 2;
  }
  // Conversion du bit 1 : ( X00 )
  if(digitalRead(bitIn3) == HIGH){
    out_value = out_value + 4;
  }
  
  /////////////////////// \\\\\\\\\\\\\\\\\\\\\\
  // Envoyer OUT VALUE
  // enlever valeur test et remplacé par out_value
  BTSerial.print(out_value);
  /////////////////////// \\\\\\\\\\\\\\\\\\\\\\
  // Recevoir la valeur IN VALUE
  if(BTSerial.available()>0){
    in_value = BTSerial.read();
  }
  Serial.print(in_value);
  Serial.print("\r\n");
  if(in_value>='0' && in_value<='8'){
    /////////////////////// \\\\\\\\\\\\\\\\\\\\\\
    // Conversion de la valeur reçu en valeur digitale
    digital = in_value - 48; 
    //Serial.print(digital);
    /////////////////////// \\\\\\\\\\\\\\\\\\\\\\
    // Écrire la valeur INPUT transformée en digitale
    if(digital>=4){
      digitalWrite(bitOut3, HIGH);
      digital -= 4;
    }
    else{
      digitalWrite(bitOut3, LOW);
    }
    if(digital>=2){
      digitalWrite(bitOut2, HIGH);
      digital -= 2;
    }
    else{
      digitalWrite(bitOut2, LOW);
    }
    if(digital>=1){
      digitalWrite(bitOut1, HIGH);
      digital -= 1;
    }
    else{
      digitalWrite(bitOut1, LOW);
    }
  }
  
  delay(250);
}
