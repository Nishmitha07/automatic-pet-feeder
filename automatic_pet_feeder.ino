#include <WiFi.h>
#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID "TMPL3VxZ70EqD"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#include <HCSR04.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <time.h>
// Blynk Auth Token
char auth[] = "YOUR_BLYNK_AUTH_TOKEN";

// WiFi Credentials
char ssid[] = "Nish";
char pass[] = "nishu___#prerana";

// Servo Pins
const int servo1Pin = 14;  // Servo 1 pin
const int servo2Pin = 15;  // Servo 2 pin

// Ultrasonic Sensor Pins
const int trigPin = 23; // Trigger pin for ultrasonic sensor
const int echoPin = 22; // Echo pin for ultrasonic sensor

// Define the Blynk virtual pins you want to use
const int servo1ControlPin = V0; // Virtual pin for Servo 1 control (e.g., for setting rotation count)
const int servo2ControlPin = V1; // Virtual pin for Servo 2 control (e.g., for setting rotation angle)
const int servo1StartTimeHourPin = V2; // Virtual pin for Servo 1 start hour
const int servo1StartTimeMinutePin = V3; // Virtual pin for Servo 1 start minute

// Servo objects
Servo servo1;
Servo servo2;

// Ultrasonic sensor object
HCSR04 sonar(trigPin, echoPin);

// Variables for Servo 1 timing and control
int servo1Rotations = 0; // Number of rotations for Servo 1
int servo1RotationCount = 3; // Default rotation count (can be set from Blynk)
unsigned long servo1LastRotationTime = 0;
int servo1StartHour = 8; // Default start hour (can be set from Blynk)
int servo1StartMinute = 0; // Default start minute (can be set from Blynk)

// Variables for Servo 2 and Ultrasonic Sensor
int waterLevelDistance = 0; // Distance measured by ultrasonic sensor
int servo2Angle = 90; // Default angle for Servo 2 (can be set from Blynk)

BlynkTimer timer;

void setup() {
  Serial.begin(115200);

  configTime(3600, 0, "pool.ntp.org"); // Set time zone (3600 seconds = GMT+1), and NTP server
  Serial.print("Waiting for NTP time sync: ");
  while (time(nullptr) < 1510644900) {  // Some initial time
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Time synced!");

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  Blynk.begin(auth, ssid, pass);

  // Set initial servo positions
  servo1.write(0);
  servo2.write(0);

  timer.setInterval(1000L, checkServo1Rotation);  // Check Servo 1 rotation time every second
  timer.setInterval(500L, checkWaterLevel);      // Check water level every 0.5 seconds
}

void loop() {
  Blynk.run();
  timer.run();

    // Read values from Blynk app
  servo1RotationCount = Blynk.get(servo1ControlPin).asInt();
  servo2Angle = Blynk.get(servo2ControlPin).asInt();
  servo1StartHour = Blynk.get(servo1StartTimeHourPin).asInt();
  servo1StartMinute = Blynk.get(servo1StartTimeMinutePin).asInt();

}

void checkServo1Rotation() {
  time_t now = time(nullptr);
  struct tm* timeinfo;
  timeinfo = localtime(&now);

  if (timeinfo->tm_hour == servo1StartHour && timeinfo->tm_min == servo1StartMinute) {
      if (servo1Rotations < servo1RotationCount) {
        servo1.write(180); // Rotate servo 1 (adjust angle as needed)
        delay(500);       // Adjust delay as needed for servo movement
        servo1.write(0);   // Return servo 1 to initial position
        delay(500);
        servo1Rotations++;
        servo1LastRotationTime = now; // Update last rotation time
      }
  }

  if (servo1Rotations >= servo1RotationCount) {
      // Reset rotations at the end of the day (or after a certain period)
      if (timeinfo->tm_hour == 0 && timeinfo->tm_min == 0) { // Reset at midnight
        servo1Rotations = 0;
      }
  }
}

void checkWaterLevel() {
  waterLevelDistance = sonar.read();
  Serial.print("Distance: ");
  Serial.print(waterLevelDistance);
  Serial.println(" cm");

  // Adjust the threshold as per your setup
  if (waterLevelDistance > 10) { // If water level is low (distance is greater than threshold)
    servo2.write(servo2Angle); // Rotate servo 2 (adjust angle as needed)
    delay(1000); // Adjust delay as needed for servo movement
    servo2.write(0); // Return servo 2 to initial position
  }
}
