#include <Wire.h>
#include <BMP390.h>

// For ESP32, define your SDA and SCL pins.
// For Arduino Uno/Mega, these are usually fixed and don't need to be passed.
// If using fixed pins, you can use BMP390 bmp;
// For ESP32 example:
const int I2C_SDA_PIN = 21; // Example SDA pin for ESP32
const int I2C_SCL_PIN = 22; // Example SCL pin for ESP32

BMP390 bmp(I2C_SDA_PIN, I2C_SCL_PIN); // Initialize BMP390 object with I2C pins

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000); // Wait for serial port to connect (up to 5 seconds)

  Serial.println("Initializing BMP390 sensor...");
  if (bmp.init_bmp390()) {
    Serial.println("BMP390 Initialization Successful!");
  } else {
    Serial.println("BMP390 Initialization Failed! Check wiring and sensor.");
  }
}

void loop() {
  bmp3_data data = bmp.get_bmp_values();

  if (data.success) {
    Serial.print("Temperature: ");
    Serial.print(data.temperature, 2); // Print with 2 decimal places
    Serial.println(" °C");

    Serial.print("Pressure: ");
    Serial.print(data.pressure / 100.0, 2); // Convert Pa to hPa (hectopascals)
    Serial.println(" hPa");
  } else {
    Serial.println("Failed to read BMP390 data.");
  }

  delay(1000); // Read every 1 second
}
