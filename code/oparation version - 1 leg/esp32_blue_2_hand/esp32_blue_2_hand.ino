/*
 * This code is a modified version of the demo software for controlling a wireless prosthetic system
 * based on EMG (electromyography) signals. The goal is to recognize muscle activity peaks and 
 * translate them into commands for the Oded Hand — a low-cost, open-source prosthetic hand
 * (https://github.com/Haifa3D).
 * 
 * Communication is done via a custom BLE protocol developed for this purpose.
 * For full protocol details, see: 
 * https://github.com/Haifa3D/haifa3d-hand-app/blob/master/Protocol.md
 * 
 * In this system:
 * - The Oded Hand acts as the BLE server.
 * - The EMG device (based on an ESP32 controller) acts as the BLE client.
 * 
 * Overview:
 * EMG signals are continuously sampled. Using signal processing (DC calibration, bandpass filtering, 
 * and peak detection), the system identifies meaningful muscle contractions.
 * These detected peaks are translated into predefined hand movements, triggered via BLE communication.
 * 
 * Getting started (for ESP32-based EMG system):
 * - Configure your BLE parameters (under "BLE Parameters").
 * - Set your hardware/software system settings (under "Your System Settings").
 * - Initialize your system in the setup() loop.
 * - Define how you want to map EMG peaks to hand commands (under "Your System Functions").
 * 
 * Communication options:
 * - To send a specific movement: use the 'Execute Characteristic' (pRemoteCharExecute).
 *   Example:
 *      unsigned char msg[] = {5, 0b11111000, 20, 0b01111000, 0b00000000};
 *      pRemoteCharExecute->writeValue(msg, msg[0]);
 * - To trigger a predefined preset: use the 'Trigger Characteristic' (pRemoteCharTrigger).
 *   Example:
 *      uint8_t preset_id = 0;  // 0-11 for 12 presets
 *      pRemoteCharTrigger->writeValue(&preset_id, 1);
 * 
 * 
 */
/***********************************************************/
/*                Libraries                                */
/***********************************************************/
#include <BLEDevice.h>
#include <BLEAdvertisedDevice.h>
#include "Mod_Manager.h"

/*************************************************************************************/
/*                                  BlE Parameters                                   */
/*************************************************************************************/
//#define MY_DEVICE_ADDRESS "24:62:ab:f2:af:46"  // add here you devicec address
#define MY_DEVICE_ADDRESS "b8:d6:1a:40:ef:d6"  // add here you devicec address


#define HAND_DIRECT_EXECUTE_SERVICE_UUID "e0198000-7544-42c1-0000-b24344b6aa70"
#define EXECUTE_ON_WRITE_CHARACTERISTIC_UUID "e0198000-7544-42c1-0001-b24344b6aa70"

#define HAND_TRIGGER_SERVICE_UUID "e0198002-7544-42c1-0000-b24344b6aa70"
#define TRIGGER_ON_WRITE_CHARACTERISTIC_UUID "e0198002-7544-42c1-0001-b24344b6aa70"

static BLEUUID serviceExeUUID(HAND_DIRECT_EXECUTE_SERVICE_UUID);   // The remote service we wish to connect to.
static BLEUUID charExeUUID(EXECUTE_ON_WRITE_CHARACTERISTIC_UUID);  // The characteristic of the remote service we are interested in - execute.

static BLEUUID serviceTrigUUID(HAND_TRIGGER_SERVICE_UUID);          // The remote service we wish to connect to.
static BLEUUID charTrigUUID(TRIGGER_ON_WRITE_CHARACTERISTIC_UUID);  // The characteristic of the remote service we are interested in - trigger

// Connection parameters:
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = true;
static BLERemoteCharacteristic* pRemoteCharExecute;
static BLERemoteCharacteristic* pRemoteCharTrigger;
static BLEAdvertisedDevice* myDevice;

unsigned char* msg;
uint8_t preset_id;
/*************************************************************************************/
/*                 Go to Sleep and Keep Scanning Setting:                            */
/*************************************************************************************/
#define DT_SCAN 5000               //if not connected to BLE device scan every 5 seconds.
#define DT_SLEEP (10 * 60 * 1000)  //if not connected to BLE device go to sleep after 10 minute.
unsigned long t_scan;              //the elapsed time between BLE scanning
unsigned long t_disconnected;      //the elapsed time from a disconecting event

/*************************************************************************************/
/*                              Your System Setting:                                 */
/*************************************************************************************/
#define NO_UNO_DATA -1
#define SwitchLeg_PRESET_TRIGGER 0

int ledPin = 13;
Mod_Manager mod_manager;

bool is_open_action = true;
unsigned char close_task[] = { 5, 0b11111000, 20, 0b01111000, 0b11111000 };  //movement length byte, torque,time,active motor, motor direction
unsigned char open_task[] = { 5, 0b11111000, 20, 0b01111000, 0b00000000 };   //movement length byte, torque,time,active motor, motor direction

/*************************************************************************************/
/*                  BLE Class Definition and Set Callbacks:                          */
/*************************************************************************************/
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
    doScan = true;
    t_disconnected = millis();
  }
};

bool connectToServer() {

  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

  // Execute Charachteristics:
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteExeService = pClient->getService(serviceExeUUID);
  if (pRemoteExeService == nullptr) {
    Serial.print("Failed to find our Execute service UUID: ");
    Serial.println(serviceExeUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our Execute service");
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharExecute = pRemoteExeService->getCharacteristic(charExeUUID);
  if (pRemoteCharExecute == nullptr) {
    Serial.print("Failed to find our Execute characteristic UUID: ");
    Serial.println(charExeUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our Execute characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharExecute->canRead()) {
    // std::string value = pRemoteCharExecute->readValue(); //todo: remove after successfull compilations
    String value = pRemoteCharExecute->readValue();
    Serial.print("Execute: The characteristic value was: ");
    Serial.println(value.c_str());
  }

  // Trigger Charachteristics:
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteTrigService = pClient->getService(serviceTrigUUID);
  if (pRemoteTrigService == nullptr) {
    Serial.print("Failed to find our Trigger service UUID: ");
    Serial.println(serviceTrigUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our Trigger service");
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharTrigger = pRemoteTrigService->getCharacteristic(charTrigUUID);
  if (pRemoteCharTrigger == nullptr) {
    Serial.print("Failed to find our Trigger characteristic UUID: ");
    Serial.println(charTrigUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our Trigger characteristic");
  // Read the value of the characteristic.
  if (pRemoteCharTrigger->canRead()) {
    String value = pRemoteCharTrigger->readValue();
    Serial.print("Trigger: The characteristic value was: ");
    Serial.println(value.c_str());
  } else {
    Serial.println("Trigger: Failed to read ");
  }

  connected = true;
  return true;  //eden
}

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {  // Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (((String)advertisedDevice.getAddress().toString().c_str()).equals(MY_DEVICE_ADDRESS)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false;

    }  // Found our server
  }    // onResult
};     // MyAdvertisedDeviceCallbacks

void InitBLE() {
  BLEDevice::init("SwitchLeg");
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(1, false);
}

/******************************************************************************/
/*                          Setup Loop:                                       */
/******************************************************************************/
void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  // Create the BLE Device
  InitBLE();
  t_scan = millis();

  // enable deep sleep mode for the esp32:
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 1);  //1 = High, 0 = Low , same GPIO as the button pin
  t_disconnected = millis();

  // add here your system setup:
  pinMode(ledPin, OUTPUT);  // Check Leg SwitchStatus
}

/******************************************************************************/
/*                      Your System Functions:                                */
/******************************************************************************/
// Check the leg switch status
int waitForUnoRead() {
  if (Serial1.available()) {
    String data = Serial1.readString();  // Read the input from UART1
    data.trim();                         // Trim any leading/trailing spaces

    int commaIndex = data.indexOf(',');  // Find the comma separating the two integers

    if (commaIndex != -1) {                                     // Ensure there's a comma present
      String press_left_str = data.substring(0, commaIndex);    // First integer (before comma)
      String press_right_str = data.substring(commaIndex + 1);  // Second integer (after comma)

      int press_left = press_left_str.toInt();    // Convert the first part to integer
      int press_right = press_right_str.toInt();  // Convert the second part to integer

      if (press_left > 0 && press_right == 0)
        return 1; // left leg
      else if (press_left == 0 && press_right > 0)
        return 2;  // right leg
      else if (press_left > 0 && press_right > 0)
        return 3; //both legs
    }
  }

  // Return a default value (NO_UNO_DATA) if no data is available
  return NO_UNO_DATA;
}

/******************************************************************************/
/*                          Main Loop:                                        */
/******************************************************************************/
void loop() {

  if (doConnect == true) {
    Serial.println("Found our server attemping connection");  //TRUE when we scanned and found the desired BLE server
    if (connectToServer())
      Serial.println("We are now connected to the BLE Server.");  // connect to the server. TRUE if connection was established
    else
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    doConnect = false;  //no need to reconnect to the server
  }

  if (connected) {  //TRUE if we are already connected to the server
    // Read the updated value of the characteristic.
    int press_type = waitForUnoRead();  //read the data from the uno.
    if (press_type == NO_UNO_DATA) {
      delay(3);
    } else {  // press detected.
      Serial.print("Action Length:");
      Serial.println(press_type);
      int action_id = mod_manager.processCommand(press_type);
      Serial.print("Action Message was sent:");
      Serial.println(action_id);
      if (action_id != mod_manager.NO_RESPONSE) {
        Serial.print("Action Message was sent:");
        uint8_t action_id_1 = (uint8_t)action_id;
        Serial.println(action_id_1);
        pRemoteCharTrigger->writeValue(&action_id_1, 1);
        delay(5 * 1000);  // bluetooth stack will go into congestion, if too many packets are sent
      }
    }
  } else {  //not connected
    //scanning for server:
    if ((millis() - t_scan > DT_SCAN)) {  //not connected
      Serial.println("Scanning...");
      BLEDevice::getScan()->start(1, false);
      t_scan = millis();
    }
    // going to sleep if long time without connection
    else if (millis() - t_disconnected > DT_SLEEP) {
      //Go to sleep now
      Serial.println("Going to sleep now");
      esp_deep_sleep_start();
    }
  }
}
