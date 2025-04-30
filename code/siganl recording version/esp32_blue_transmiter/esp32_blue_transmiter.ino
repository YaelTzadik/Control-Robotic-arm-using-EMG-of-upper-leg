
/*
 * UART to Bluetooth Bridge
 * 
 * This code reads incoming data from UART1 (Serial1) and forwards it to:
 * - The USB Serial (for debugging/monitoring on a PC)
 * - A Bluetooth Serial connection (for wireless transmission)
 * 
 * Additionally, an LED on pin 13 lights up when data is being read.
 */
#include "BluetoothSerial.h"  // Include Bluetooth Serial library
const int ledPin = 13;

BluetoothSerial Serial_BT;  // Create a BluetoothSerial object


void setup() {
  Serial.begin(115200);            // Initialize the default Serial interface (connected to USB)
  Serial1.begin(9600);             // Initialize UART1 with a baud rate of 9600
  Serial_BT.begin("ESP32_Sense");  // Initialize Bluetooth with a name
  pinMode(ledPin, OUTPUT);         // Set pin 13 as an output for the led
}

void loop() {
  // Check if there's data available on UART1
  if (Serial1.available()) {
    digitalWrite(ledPin, HIGH);
    String data = Serial1.readString();  // Read a character from UART1
    digitalWrite(ledPin, LOW);
    Serial.print(data);          // Print the character to the default Serial (USB)
    if (Serial_BT.connected()) {
      Serial_BT.println(data);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
}
