
# Dogecoin Ticker
A smart 3D Printed ESP8266 Cryptocurrency Ticker - adapted towards Dogecoin;
Forked from http://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker


<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/ToTheMoon-With-ShibeLid.jpg?raw=true" width="510"/>
<!-- <img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/Interior-OriginalEnclosure.jpg?raw=true" width="256" /> -->
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/ScreenRecording.gif?raw=true" />

## What You'll Need

- [3D Printer]()
- x1 [ESP8266 NodeMCU Dev Board](https://www.amazon.com/gp/product/B081CSJV2V/)
- x1 [128x32 OLED Display](https://www.amazon.com/gp/product/B08L7QW7SR/)
- x1 [5mm RGB LED Diode](https://www.amazon.com/gp/product/B01C3ZZT8W/)
- Hot Glue Gun & Glue
- x1 [Micro USB Cable](https://www.amazon.com/gp/product/B072J1BSV6/)

## Step 1: Clone this repository

## Step 2: Prepare Your Board

If you're using the same devboard and OLED as we are, you'll connect everything as follows:

|OLED|NodeMCU|
|--|--|
|GND|GND|
|VCC|3v3 |
|SCL|D1 |
|SDA|D2 |

<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/WiringSchematic.png?raw=true" />

## Step 3: Prepare Arduino IDE

In order to successfully run the project as-provided, you'll need to install some libraries and edit a few files.

**1. Install your board.**

If you're using the board shown in this tutorial you'll need to do the following:

- Navigate to: File -> Preferences -> Additional Boards Manager URLs:

-- Enter: **http://arduino.esp8266.com/stable/package_esp8266com_index.json**

- Select "OK"

- Next, go to: **Tools > Board > Board Manager> Type "esp8266" and download the Community esp8266 and install.**

- Then, set up your chip as:
- - Tools -> Board -> NodeMCU 1.0 (ESP-12E Module)
- - Tools -> Flash Size -> 4M (3M SPIFFS)
- - Tools -> CPU Frequency -> 80 Mhz
- - Tools -> Upload Speed -> 921600
- - Tools-->Port--> (whatever it is)

[Manufacturer Instructions](http://www.hiletgo.com/ProductDetail/1906570.html)

**2. Install the following libraries from the Arduino IDE by navigating to:**

**Sketch -> Include Library -> Manage Libraries**  *OR*  **Sketch -> Include Library -> Add .ZIP Library**

The libraries you'll need to install are:

- Adafruit GFX (via IDE)

- Adafruit SSD1306 (via IDE)

- ArduinoJSON (via IDE)

- Arduino_JSON (via IDE)

- ElegantOTA (via IDE)

- [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) (via .ZIP)

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) (via .ZIP)

**Once you've installed the libraries, restart your IDE.**

## Step 4: Edit the Code

In order to use this project as-provided, you'll need to provide your WiFi credentials in the .INO code file before uploading. Go ahead and open up the latest release .INO file and edit the following lines under ***Definitions and Variables***:

    // Network Credentials
    #define ssid "YOUR_SSID" // Your WiFi Network Name (Case Sensitive)
    #define password "YOUR_PASSWD" // Your WiFi Network Password (Case Sensitive)

If you would like, you can also set up your default Cryptopair. This is the default Cryptopair that is display when the device is powered on or reset.

The value(s) can be any of [these pairs](https://api.gemini.com/v1/symbols).

You'll need to enter the Cryptocoin ticker symbol and the appropriate conversion currency in all uppercase.

The default values are Bitcoin (BTC) to USD (United States Dollar).

    // API Variables
    String currentCrypto = "BTC"; // Stores default/currently selected cryptocoin
    String currentCurrency = "USD"; // Stores default/currently selected currency

## Step 5: Modify/Copy Files

Next, we need to copy a few files into our Arduino Sketch Folder.

Copy the LittleFS Data ["data" directory](https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/tree/main/LittleFS%20Data) over to your Sketch Folder.

If you're using a default Arduino IDE installation it should be as follows:

    C:\Users\{USERNAME}\Documents\Arduino\Cryptoticker_V{RELEASE VERSION}

## Step 6: Upload the Code

Start by uploading the main sketch to your device.

Once it's uploaded, navigate to **Tools > ESP8266 LittleFS Data Upload**

Once the data files finish uploading and the device reboots, navigate directly to the IP address displayed on the OLED screen / Serial Monitor.

After 30 seconds, your OLED will show the default price/pair as mentioned above.

From the web interface you can select a new supported crypto currency and currency pair to display.

To save your changes, just click/tap the "Save Changes" button.

## Congratulations! You've successfully set up your Cryptoticker!

<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/WebInterfaceScreenshot.png?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/BTCUSD.jpg?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/DOGEUSD.jpg?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/ETHUSD.jpg?raw=true" />
<img src="https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/images/LTCUSD.jpg?raw=true" />
