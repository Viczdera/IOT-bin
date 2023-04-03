#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial unoSerial(0, 1);  //rx tx
//
#define ledPin 10
// Define the maximum size of JSON message
const int JSON_OBJECT_SIZE = 100;


void setup() {
  // put your setup code here, to run once:
  unoSerial.begin(9600);
  Serial.begin(9600);
  while (!Serial) {
    // Wait for serial connection
  }
  pinMode(ledPin, OUTPUT);
}
int d;

void loop() {
  int data = 50;
  if (unoSerial.available() > 0) {
    d = unoSerial.read();

    // const char* json = "{\"bin1\":{\"level\":46,\"servo\":\"on\"},\"bin2\":{\"level\":32,\"servo\":\"on\"}}";
    // StaticJsonDocument<200> doc;

    // String jsonString = unoSerial.readString();
    // create a DynamicJsonDocument
    // DynamicJsonDocument doc(256);

    StaticJsonDocument<JSON_OBJECT_SIZE> doc;
    DeserializationError error = deserializeJson(doc, Serial);

    // Check for errors
    if (error) {
      Serial.println("Error during deserialization!");
      return;
    }

    // Access the values
    int bin1Level = doc["bin1"]["level"];
    String bin1Servo = doc["bin1"]["servo"];
    int bin2Level = doc["bin2"]["level"];
    String bin2Servo = doc["bin2"]["servo"];

    // Print the values
    Serial.print("Bin1 Level: ");
    Serial.println(bin1Level);
    Serial.print("Bin1 Servo: ");
    Serial.println(bin1Servo);
    Serial.print("Bin2 Level: ");
    Serial.println(bin2Level);
    Serial.print("Bin2 Servo: ");
    Serial.println(bin2Servo);

   
    if (bin1Servo=="on") {
      digitalWrite(ledPin, HIGH);
    } else if (bin1Servo=="off") {
      Serial.println("LED not turned on");
      digitalWrite(ledPin, LOW);
    }
    //Serial.println(d);
    unoSerial.write(data);
  } else {
    Serial.println("Serial unavailable");
    digitalWrite(ledPin, LOW);
  }
  delay(1000);
}
