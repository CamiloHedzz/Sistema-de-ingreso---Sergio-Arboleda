#include <SPI.h>
#include <Servo.h>
#include <EEPROM.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN         9
#define SS_PIN          10

Servo miServo;
int LEDA = 3;
int LEDR = 2;
int segundos = 0;
LiquidCrystal_I2C lcd (0x27,2,1,0,4,5,6,7);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

String tarjeta = "C3 F9 09 2E";
String formato = "XX XX XX XX";

int notas[] ={ 3, 4, 2, 4.0, 5.0, 2.3, 3.8};
 
void setup() {
  EEPROM.put(0, tarjeta);
  miServo.attach(5);
  pinMode(LEDA, OUTPUT);
  pinMode(LEDR, OUTPUT);
  inicializarTarjeta();
  inicializarPantalla();  
}

void inicializarPantalla(){          //Inicializar Pantalla 
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.clear();
  //imprimirInicio();
}

void inicializarTarjeta(){          //Inicializa la tarjeta RFID
  Serial.begin(9600); 
  while (!Serial); 
  SPI.begin();  
  mfrc522.PCD_Init();  
  delay(4);       
  mfrc522.PCD_DumpVersionToSerial(); 
  
}

void loop() {

  imprimirInicio();
  //Serial.println(EEPROM.get(0, tarjeta));
  if ( ! mfrc522.PICC_IsNewCardPresent()) {// valida si hay una tarjeta presente
    imprimirInicio();
    Serial.println("Hay algo...");
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) {// leer la tarjeta
    return;
  }
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));// imprime la informacion al serial
  
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == EEPROM.get(0, formato)){
     abrirPuerta();
  }else{
    Serial.println("Esa no es");
     imprimirRechazo();
     digitalWrite(LEDR, HIGH);  // Prende un LED rojo si no esta registrado
     delay(500);
     digitalWrite(LEDR, LOW);
  }
}

void imprimirInicio(){
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACERCA LA");
  lcd.setCursor(0, 1);
  lcd.print("TARGETA");
}

void imprimirBienvenida(){
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("BIENVENIDO");
  lcd.setCursor(4, 1);
  lcd.print(segundos+1);
  lcd.print(" Seg.");
}

void imprimirRechazo(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("USTED NO HA SIDO");
  lcd.setCursor(3, 1);
  lcd.print("RECONOCIDO");
}

void abrirPuerta(){
  digitalWrite(LEDA, HIGH);  //Prende un led azul si esta registrado
  miServo.write(180);
  delay(200);
  miServo.write(90);
  while(segundos<5){
    imprimirBienvenida();
    delay(1000);
    segundos++;
  }
  segundos = 0;
  digitalWrite(LEDA, LOW);
  miServo.write(0);
  delay(200);
  miServo.write(90);
  
}
