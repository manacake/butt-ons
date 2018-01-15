// Butt-ons
// -*- mode: C++ -*-
// Example sketch showing how pushing certain buttons on one Feather 32u4
// will trigger a led light on a nearby Feather 32u4.
// Each feather is desinged to run the same code.

// Do i need these libs?
#include <SPI.h>
// See docs: http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF95.html
#include <RH_RF95.h>

// Feather 32u4 w/Radio Settings
#define RFM95_CS      8
#define RFM95_INT     7
#define RFM95_RST     4

// Other settings
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define RLED     13

// Define frequency
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  delay(500);
  Serial.begin(115200);
  // wait until serial console is open, remove if not tethered to computer
  while (!Serial) { delay(1); }
  
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(RLED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);

  digitalWrite(RLED, LOW);
  
  digitalWrite(RFM95_RST, HIGH);
  Serial.println("Booting");

  // Manual reset
  // Why do we need manual reset?
  // TODO: try without manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  if (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  Serial.println("LoRa radio init OK!");
  // Defaults after init are 915.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  // Sets the transmitter power output level, and configures the transmitter pin.
  // For RFM95 LORA with useRFO false, valid values are from +5 to +23
  rf95.setTxPower(23, false);
  
  Serial.print("LoRa radio @"); Serial.print((int)RF95_FREQ); Serial.println(" MHz");
  delay(500);
}

void loop() {
  // Tests whether or not a new message is available
  if (rf95.available()) {
    char buf[RH_RF95_MAX_MESSAGE_LEN];
    char len = sizeof(buf);

    if (!rf95.recv(buf, &len)) {
      Serial.println("Receive failed");
      return;
    } else { // Should be a message for us now
      RH_RF95::printBuffer("\nReceived: ", buf, len);
      Serial.print("Got: "); Serial.println((char*)buf);
      // Receiver Signal Strength Indicator measured in dBm
      Serial.print("RSSI: "); Serial.println(rf95.lastRssi(), DEC);

      if (!strcmp(&buf[8], "A")) {
        Serial.println("Red LED On!");
        digitalWrite(RLED, HIGH);
        delay(1000);
        digitalWrite(RLED, LOW);
        delay(1000);
        Serial.println("Red LED Off!");
      }
    }
  }

  // If buttons are pressed
  if (!digitalRead(BUTTON_A) || !digitalRead(BUTTON_B) || !digitalRead(BUTTON_C)) {
    Serial.println("\nButton pressed!");

    char radiopacket[9] = "Button #";
    if (!digitalRead(BUTTON_A)) radiopacket[8] = 'A';
    if (!digitalRead(BUTTON_B)) radiopacket[8] = 'B';
    if (!digitalRead(BUTTON_C)) radiopacket[8] = 'C';
    radiopacket[9] = 0;
    
    Serial.print("Sending "); Serial.println(radiopacket);
    rf95.send((uint8_t *)radiopacket, strlen(radiopacket) + 1);
    // Blocks until the transmitter is no longer transmitting
    rf95.waitPacketSent();
  }
}
