#include <SoftwareSerial.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

SoftwareSerial espSerial(D3, D2);  //rx tx

//firebase setup

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Dera's Hp"
#define WIFI_PASSWORD "012345678"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCP1jmO33av0BW4RDJeEUGXFMqnyiRr4EE"

/* 3. Define the RTDB URL */
#define DATABASE_URL "iot-dera-f860b-default-rtdb.firebaseio.com"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "dera@gmail.com"
#define USER_PASSWORD "deraesp8266"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseJson firebaseJson;
FirebaseConfig config;

//variables
int data;
unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;

void setup() {
  // put your setup code here, to run once:
  espSerial.begin(9600);
  Serial.begin(9600);

  //connect to DB
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("connecting...");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected with IP: ");

  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

#if defined(ESP8266)
  // In ESP8266 required for BearSSL rx/tx buffer for large data handle, increase Rx size as needed.
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
}

void loop() {
  int ta = 95;
  if (Firebase.RTDB.getJSON(&fbdo, "/sensor_data/bins")) {

    if (firebaseJson.setJsonData(fbdo.payload())) {
      // Print the JSON data on the serial monitor
      String jsonStr;
      firebaseJson.toString(jsonStr);
      Serial.println(jsonStr);
      if (espSerial.available() > 0) {
        espSerial.write(jsonStr.c_str(), jsonStr.length());
        Serial.println("Sent to arduino");
      }else{
         Serial.println("arduino unreachable");
      }
    } else {
      Serial.println("Error parsing JSON data");
    }

  } else {
    Serial.println(fbdo.errorReason());
    Serial.println("Firebase connection lost/error. Restarting...");
      ESP.restart();
  }

  if (espSerial.available() > 0) {
    // if (Firebase.ready()) {
    //   espSerial.write(ta);
    // }
    data = espSerial.read();
    Serial.println(data);
  }
  //reconnect if connection lost
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Restarting...");
    ESP.restart();
  }

  delay(1000);
}
