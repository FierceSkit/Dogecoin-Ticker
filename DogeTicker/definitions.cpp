// ===================================== //
// ===== Definitions and Variables ===== //
// ===================================== //
// Code Version Info
String codeVersion = "1.2.0";

// Network Credentials
#define ssid "YOURWIFI"  // Your WiFi Network Name (Case Sensitive)
#define password "YOURPASS"  // Your WiFi Network Password (Case Sensitive)

// API Variables
String currentCrypto = "DOGE"; // Stores default/currently selected cryptocoin
String currentCurrency = "USD"; // Stores default/currently selected currency

const long fetchInterval = 5000; // Time between API Fetch :: 30 seconds

// API Definitions
#define API_HOST ("api.gemini.com") // API Endpoint Host
#define API_URL ("/v1/pricefeed/" + coin + target) // API Endpoint Target

// API SHA1 HASH
// SHA1 fingerprint of the certificate for API Endpoint Host (host)
const char fingerprint[] PROGMEM = "93 BF 06 D9 08 7D 83 82 CF 80 00 9F 14 68 A4 EB 17 E5 68 F3";

// Port used to connect to API Endpoint Host.
// Since we're using SSL, we'll use port 443.
const int httpsPort = 443;