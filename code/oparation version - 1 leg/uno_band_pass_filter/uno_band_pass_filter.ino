// Includes
#include "DCAutoCalibrator.h"
#include "BPF.h"
#include "PeakRecognizer.h"

// Modes
#define RECORD_MODE 0  // Record sensor data mode
#define DEBUG_MODE 0   // Debug mode with serial prints
#define RUN_MODE 1     // Run mode: active operation

// Constants
#define ANALOG_MAX_VAL (1 << 10)  // Max analog value (1024), typical for 10-bit ADC
#define ANALOG_MIN_VAL 0          // Min analog value
#define LED_PIN 13                // Built-in LED pin

// Sensor reading variables
int sensor_value_A1 = 0;  // Sensor value from left leg
int sensor_value_A2 = 0;  // Sensor value from right leg

// Signal processing objects
DCAutoCalibrator dc_calibrator_left;
BPF bpf_left;
PeakRecognizer peak_recognizer_left;

DCAutoCalibrator dc_calibrator_right;
BPF bpf_right;
PeakRecognizer peak_recognizer_right;

#if RECORD_MODE
int record_left[256];  // Buffer for recording left leg data
#endif

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Configure LED pin as output
#if DEBUG_MODE
  Serial.begin(115200);      // Start serial communication for debugging
#endif
}

void loop() {
#if RECORD_MODE
  // Recording mode: capture 256 samples
  digitalWrite(LED_PIN, HIGH);  // Turn LED on during recording
  for (auto i = 0; i <= 256; ++i) {
    // --- Left leg processing ---
    int value_left = analogRead(A1);          // Read left sensor
    bool no_jump_left = dc_calibrator_left.push(value_left);  // DC offset calibration
    int dc_left = dc_calibrator_left.get_calibrated_dc();
    int filtered_left = bpf_left.push(value_left, dc_left);   // Apply bandpass filter
    if (!no_jump_left) peak_recognizer_left.reset();          // Reset peak recognizer if jump detected
    int res_left = peak_recognizer_left.push(filtered_left);  // Detect peaks
    record_left[i] = filtered_left;                           // Save filtered value

    // --- Right leg processing ---
    int value_right = analogRead(A2);          // Read right sensor
    bool no_jump_right = dc_calibrator_right.push(value_right);
    int dc_right = dc_calibrator_right.get_calibrated_dc();
    int filtered_right = bpf_right.push(value_right, dc_right);
    if (!no_jump_right) peak_recognizer_right.reset();
    int res_right = peak_recognizer_right.push(filtered_right);

    delay(20);  // Small delay between samples
  }

#else
  // Normal operation (not recording)
  
  // --- Left leg processing ---
  int value_left = analogRead(A1);
  bool no_jump_left = dc_calibrator_left.push(value_left);
  int dc_left = dc_calibrator_left.get_calibrated_dc();
  int filtered_left = bpf_left.push(value_left, dc_left);
  if (!no_jump_left) peak_recognizer_left.reset();
  int res_left = peak_recognizer_left.push(filtered_left);

  // --- Right leg processing ---
  int value_right = analogRead(A2);
  bool no_jump_right = dc_calibrator_right.push(value_right);
  int dc_right = dc_calibrator_right.get_calibrated_dc();
  int filtered_right = bpf_right.push(value_right, dc_right);
  if (!no_jump_right) peak_recognizer_right.reset();
  int res_right = peak_recognizer_right.push(filtered_right);

#endif

#if DEBUG_MODE
  // Debugging output over Serial
  Serial.print(dc_left);
  Serial.print(",");
  Serial.print(filtered_left);
  Serial.print(",");
  Serial.println(res_left);
  delay(20);
#endif

#if RUN_MODE
  // Run mode: send detected peaks to ESP32
  if (res_left > 0) {
    Serial.begin(9600);
    Serial.println(res_left);  // Send left leg peak detection result
    Serial.end();
  }
  delay(20);
#endif

#if RECORD_MODE
  // After recording: turn LED off and send recorded data over Serial
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600);
  for (auto i = 0; i < 256; ++i) {
    Serial.println(record_left[i]);
  }
  Serial.end();
  delay(1000);
#endif
}
