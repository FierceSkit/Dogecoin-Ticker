# ESP8266 Cryptocurrency Ticker with OLED Display

A sleek and modern cryptocurrency price ticker built for the ESP8266 (NodeMCU) with an OLED display.

This project is a fork of the [NexGen-Crypto-Ticker](https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker) with significant enhancements and improvements.

<img src="https://github.com/FierceSkit/DogeTicker/blob/main/images/ToTheMoon-With-ShibeLid.jpg?raw=true" width="510"/>

## New Features & Improvements

- **Enhanced Currency Support**: Added support for multiple currencies with proper symbol display:
  - USD ($), EUR (€), GBP (£), RUB (₽)
- **Improved Bitmap Display**:
  - Custom high-quality logos for DOGE, BTC, and LTC
  - Splash screens during initialization
  - Optimized bitmap handling in separate header files
- **Robust API Integration**:
  - Secure SSL connection to Gemini API
  - Improved error handling and response parsing
- **Modular Code Structure**:
  - Separated functionality into dedicated header files
  - Better maintainability and organization

## Hardware Requirements

- [ESP8266 NodeMCU Dev Board](https://www.amazon.com/gp/product/B081CSJV2V/)
- [128x32 OLED Display](https://www.amazon.com/gp/product/B08L7QW7SR/)
- [5mm RGB LED Diode](https://www.amazon.com/gp/product/B01C3ZZT8W/)
- [Micro USB Cable](https://www.amazon.com/gp/product/B072J1BSV6/)
- Hot Glue Gun & Glue
- 3D Printer (for case)

## Wiring Instructions

Connect the OLED display to the NodeMCU as follows:

|OLED|NodeMCU|
|--|--|
|GND|GND|
|VCC|3v3|
|SCL|D1|
|SDA|D2|

<img src="https://github.com/FierceSkit/DogeTicker/blob/main/images/WiringSchematic.png?raw=true" />

## Software Setup

1. **Install Arduino IDE Requirements**:
   - Add ESP8266 board support: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Board settings:
     - Board: NodeMCU 1.0 (ESP-12E Module)
     - Flash Size: 4M (3M SPIFFS)
     - CPU Frequency: 80 MHz
     - Upload Speed: 921600

2. **Required Libraries**:
   - Adafruit GFX (via IDE)
   - Adafruit SSD1306 (via IDE)
   - ArduinoJSON (via IDE)
   - Arduino_JSON (via IDE)
   - ElegantOTA (via IDE)
   - [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP)
   - [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

3. **Configuration**:
   - Set your WiFi credentials in the code
   - Choose your default cryptocurrency pair
   - Upload the sketch
   - Upload the LittleFS data

## Usage

1. Power on the device
2. Connect to your WiFi network
3. Access the web interface using the IP shown on the display
4. Select your preferred cryptocurrency and currency pair
5. Save your changes

## Example Displays

<img src="https://github.com/FierceSkit/DogeTicker/blob/main/images/BTCUSD.jpg?raw=true" />
<img src="https://github.com/FierceSkit/DogeTicker/blob/main/images/DOGEUSD.jpg?raw=true" />
<img src="https://github.com/FierceSkit/DogeTicker/blob/main/images/LTCUSD.jpg?raw=true" />

## Contributing

Contributions are welcome! Feel free to submit issues and pull requests.

## License

This project is open source and available under the MIT License.

## Acknowledgments

- [NexGen-Crypto-Ticker](https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker) for the original project
- Gemini API for providing cryptocurrency price data
- ESP8266 and OLED display communities for excellent documentation 

<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/WebInterfaceScreenshot.png?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/BTCUSD.jpg?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/DOGEUSD.jpg?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/ETHUSD.jpg?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/LTCUSD.jpg?raw=true" />
