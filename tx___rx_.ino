#include <SPI.h>
#include <RH_RF95.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define RLED      11

//#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM95_CS      8
  #define RFM95_INT     7
  #define RFM95_RST     4

//#endif
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
int16_t packetnum = 0;  // packet counter, we increment per xmission
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
void setup() {
  delay(500);
  Serial.begin(115200);
  while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer
/*  // Initialize OLED display
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  oled.display();
  delay(500);
  oled.clearDisplay();
  oled.display();
*/
 pinMode(BUTTON_A, INPUT_PULLUP);
 pinMode(BUTTON_B, INPUT_PULLUP);
 pinMode(BUTTON_C, INPUT_PULLUP);
  
  pinMode(RLED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.println("Booting");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  rf95.setTxPower(23, false);
  
  pinMode(RLED, OUTPUT);
  Serial.print("LoRa radio @"); Serial.print((int)RF95_FREQ); Serial.println(" MHz");
/*
   // OLED text display tests
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.print("LoRa radio @ ");
  oled.print((int)RF95_FREQ);
  oled.println(" MHz");
  oled.display();
*/
  delay(500);
}
void loop() {
  if (rf95.available()) {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (! rf95.recv(buf, &len)) {
      Serial.println("Receive failed");
      return;
    }
    // digitalWrite(RLED, HIGH);
    RH_RF95::printBuffer("Received: ", buf, len);
    Serial.print("Got: "); Serial.println((char*)buf);
    Serial.print("RSSI: "); Serial.println(rf95.lastRssi(), DEC);

    if ((char*)buf != 0) {
      digitalWrite(RLED, HIGH);
      delay(1000);
      digitalWrite(RLED, LOW);

      char arr[ ] = "This is a test";
      string str(arr);
      str = "This is another string";
      str = arr;
    }
   /* oled.clearDisplay();
    oled.setCursor(0,0);
    oled.println((char*)buf);
    oled.print("RSSI: "); oled.print(rf95.lastRssi());
    oled.display(); 
   */
  }
  
  if (!digitalRead(BUTTON_A) || !digitalRead(BUTTON_B) || !digitalRead(BUTTON_C))
  {
    Serial.println("Button pressed!");
    
    char radiopacket[20] = "Button #";
    if (!digitalRead(BUTTON_A)) {
      radiopacket[8] = 'A';
    }
    if (!digitalRead(BUTTON_B)) radiopacket[8] = 'B';
    if (!digitalRead(BUTTON_C)) radiopacket[8] = 'C';
    radiopacket[9] = 0;
    Serial.print("Sending "); Serial.println(radiopacket);
    rf95.send((uint8_t *)radiopacket, strlen(radiopacket));
    rf95.waitPacketSent();
  }
}
