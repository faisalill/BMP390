# BMP390 Arduino Library

A simple and efficient Arduino library for the Bosch BMP390 barometric pressure and temperature sensor. This library provides an easy-to-use interface for reading compensated temperature and pressure data from the BMP390 sensor via I2C.

## Features

- **Easy Initialization**: Simple setup for the BMP390 sensor.
- **Temperature and Pressure Readings**: Accurately reads compensated temperature and pressure values.
- **Internal Calibration**: Handles sensor calibration data and compensation calculations automatically.
- **Standard I2C Communication**: Utilizes the Arduino Wire library for robust I2C communication.
- **Broad Compatibility**: Works with Arduino UNO, ESP32, and other Arduino-compatible boards.

## Installation

Using Arduino IDE Library Manager
(This method will be available once the library is officially added to the registry)

Open the Arduino IDE.

Go to Sketch > Include Library > Manage Libraries....

In the Library Manager, search for "BMP390".

Select the "BMP390 by Mohammed Faisal I"and click "Install".

### Manual Installation

Download the latest release ZIP file from the [GitHub repository](https://github.com/faisalill/BMP390/releases).

In the Arduino IDE, go to Sketch > Include Library > Add .ZIP Library....

Navigate to and select the downloaded ZIP file.

Restart the Arduino IDE.

Alternatively, you can manually:

Download the latest release ZIP file.

Unzip the file.

Rename the unzipped folder to BMP390.

Move the BMP390 folder into your Arduino libraries directory (e.g., Documents/Arduino/libraries/).

Restart the Arduino IDE.

Usage
Connect your BMP390 sensor to your Arduino/ESP32 board's I2C pins.

**BMP390 SDA** to **SDA**

**BMP390 SCL** to **SCL**

**BMP390 VCC** to **3.3V** (or 5V if your sensor module has a voltage regulator)

**BMP390 GND** to **GND**

**For ESP32**: You can specify the SDA and SCL pins in the constructor.
**For Arduino Uno/Mega**: The SDA/SCL pins are typically fixed (e.g., A4/A5 for Uno, 20/21 for Mega) and don't need to be explicitly passed in the constructor. You can use BMP390 bmp; if you're using the default I2C pins.

## Example

Here's a basic example sketch to get you started

```cpp

#include <Wire.h>
#include <BMP390.h>

// Define I2C pins for ESP32. Adjust these to your specific wiring. // For Arduino Uno/Mega, you can often omit these and use BMP390 bmp; const int I2C_SDA_PIN = 21; // Example SDA pin for ESP32
const int I2C_SCL_PIN = 22; // Example SCL pin for ESP32

BMP390 bmp(I2C_SDA_PIN, I2C_SCL_PIN); 

void setup() {
  Serial.begin(115200);
  // Wait for serial port to connect. Needed for boards with native USB (e.g., ESP32, Leonardo)
  while (!Serial && millis() < 5000); 

  Serial.println("Initializing BMP390 sensor...");
  bmp3_data bmp_data = bmp.get_bmp_values();
  if (bmp_data.success) {
    Serial.println("BMP390 Initialized Successfully!");
  } else {
    Serial.println("BMP390 Initialization Failed! Please check wiring and sensor connection.");
  }
}

void loop() {
  bmp3_data data = bmp.get_bmp_values();

  if (data.success) {
    Serial.print("Temperature: ");
    Serial.print(data.temperature, 2); // Print with 2 decimal places
    Serial.println(" °C");

    Serial.print("Pressure: ");
    Serial.print(data.pressure / 100.0, 2); // Convert Pascals to hPa (hectopascals)
    Serial.println(" hPa");
  } else {
    Serial.println("Failed to read BMP390 data.");
  }

  delay(1000); // Read every 1 second
}
```

## API Reference

#### BMP390(int sda_pin, int scl_pin)

Constructor for the `BMP390` class.

- `sda_pin`: (int) The SDA pin for I2C communication. This is required for ESP32 and other boards where I2C pins are configurable.

- `scl_pin`: (int) The SCL pin for I2C communication. Required for configurable I2C pins.

Note: For boards with fixed I2C pins (like Arduino Uno/Mega), you can use the default constructor `BMP390 bmp`;

- `bool init_bmp390()`
Initializes the BMP390 sensor. This function should be called once in your setup() function.

**Returns**: true on successful initialization, false otherwise.

- `bmp3_data get_bmp_values()`
Reads raw temperature and pressure data from the sensor, applies Bosch's compensation algorithm, and returns the results.

**Returns**: A `bmp3_data struct` containing:

`double temperature`: Compensated temperature in degrees Celsius.

`double pressure`: Compensated pressure in Pascals.

`bool success`: `true` if the reading was successful, false if there was an error (e.g., sensor not found, zero raw data).

## License

This project is licensed under the MIT License - see the LICENSE.txt file for details.
