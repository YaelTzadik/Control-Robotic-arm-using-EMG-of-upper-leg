/*
 * Simple EMG Data Logger
 * 
 * This code reads EMG signals from two analog inputs (A1 and A2),
 * stores them in arrays, and then outputs the recorded data over Serial.
 * 
 * It can be used for testing, calibration, or offline analysis of raw EMG signals.
 */

#define MAX_VAL (1 << 10)  // Maximum ADC value (1023)
#define MIM_VAL 0          // Minimum ADC value
#define DEV_MODE 1         // Developer mode flag (currently unused)

const int ledPin = 13;     // LED pin for status indication
const int arr_size = 300;  // Number of samples to record

// Arrays to store the samples
int array1[arr_size] = {0};  // Channel A1
int array2[arr_size] = {0};  // Channel A2

// Variables to hold sensor readings
int sensorValue_A1 = 0;
int sensorValue_A2 = 0;

void setup() {
  pinMode(ledPin, OUTPUT);  // Set LED pin as output
  // Serial is started later after recording to avoid affecting timing
}

void loop() {
  // Data collection phase
  for (int i = 0; i < arr_size; i++) {
    digitalWrite(ledPin, HIGH);  // LED on during recording

    sensorValue_A1 = analogRead(A1);  // Read left EMG signal
    sensorValue_A2 = analogRead(A2);  // Read right EMG signal

    array1[i] = sensorValue_A1;
    array2[i] = sensorValue_A2;

    delay(30);  // Sampling interval (~33 Hz)
  }

  // Data transmission phase
  Serial.begin(9600);
  digitalWrite(ledPin, LOW);  // LED off during sending

  for (int i = 0; i < arr_size; i++) {
    // Send data as (A1, A2)
    String output = "(" + String(array1[i]) + "," + String(array2[i]) + ")";
    Serial.println(output);
  }
  Serial.end();

  delay(100);  // Short pause before restarting
}