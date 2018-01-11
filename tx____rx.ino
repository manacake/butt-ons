//include libraries
#include <SPI.h>
#include <RH_RF95.h>
//define
#define button 9
#define led 11
#define RFM95_CS 8
#define RFM95_INT 7
#define RFM95_RST 4
#define RF95_FREQ 915.0
int ledBool = LOW;
int16_t packetnum = 0; //packet counter
char radiopacket[5] = "Booty";
RH_RF95 rf95(RFM95_CS, RFM95_INT);
void setup() {
  //initialize serial connection
  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }

  //setup pins
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
    // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  //check radio initialization
  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
   }
  //check radio frequency
  Serial.println("LoRa radio init OK!");
  if (!rf95.setFrequency(RF95_FREQ)){
    Serial.println("setFrequency failed");
    while(1);
  }
  rf95.setTxPower(23, false);
  Serial.print("LoRa radio @"); Serial.print((int)RF95_FREQ); Serial.println(" MHz");
  digitalWrite(led, HIGH);
  delay(50);
  digitalWrite(led, LOW);
}
void loop() {
  if (rf95.available()){
    if (ledBool = LOW) {
      digitalWrite(led, HIGH);
      ledBool = HIGH;
    }
    else
      digitalWrite(led, LOW);
      ledBool = LOW;
  }
  else {
    if (button == HIGH && ledBool == HIGH){
        rf95.send((uint8_t *)radiopacket, 20);
        digitalWrite(led, LOW);
        ledBool = LOW;
    }
    else if (button == HIGH && ledBool == LOW){
        rf95.send((uint8_t *)radiopacket, 20);
        digitalWrite(led, HIGH);
        ledBool = HIGH;
    }
  }
}
