#include <DHT.h>

/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "DHT.h"

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#ifndef STASSID
#define STASSID "wifi_network"
#define STAPSK  "password"
#endif


Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_AlphaNum4 matrix2 = Adafruit_AlphaNum4();
const char* ssid = STASSID;
const char* password = STAPSK;

// Web Server on port 80
//WiFiServer server(80);
ESP8266WebServer server(80);

// DHT Sensor
const int DHTPin = 14;
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

//// Temporary variables
//static char celsiusTemp[7];
//static char fahrenheitTemp[7];
//static char humidityTemp[7];


float humidity, temp_c;  // Values read from sensor

// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor


void handle_root() {
  server.send(200, "text/plain", "Homebridge http Temperature and humidity Sensor, read from /dht");
  delay(100);
}

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);

  dht.begin();
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.on("/", handle_root);

  server.on("/dht", []() {
    gettemperature();
    server.send(200, "application/json", "{\n" "\"temperature\": " + String(temp_c) + ",\n ""\"humidity\": " + String(humidity) + "" "\n}");
  });
  server.on("/dhtcsv", []() {
    gettemperature();
    server.send(200, "text/plain",  "" + String(temp_c) + "," + String(humidity));
  });

  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  matrix.begin(0x70);
  matrix2.begin(0x72);
  
}

// runs over and over again
void loop() {
  gettemperature();
  server.handleClient();
}

void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor
    previousMillis = currentMillis;

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_c = dht.readTemperature(false);     // Read temperature as Fahrenheit
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_c)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  
   Serial.print("Humidity: ");
              Serial.print(humidity);
              Serial.print(" %\t Temperature: ");
              Serial.print(temp_c);
              Serial.print(" *C ");
              Serial.println(" done..");
    matrix.print(temp_c);   
     matrix.writeDisplay();     
     
     char humid[5];
//     sprintf(humid, "@%d$", humidity);
     dtostrf(humidity, 6, 2, humid);
     Serial.print(humid[0]);
     Serial.print(humid[1]);
     Serial.print(humid[2]);
     Serial.print(humid[3]);
     Serial.print(humid[4]);
     Serial.print(humid[5]);
     Serial.print(humid[6]);
     matrix2.writeDigitAscii(0, humid[1]);
     matrix2.writeDigitAscii(1, humid[2], true);
//     matrix2.writeDigitAscii(1, '8', true);
     matrix2.writeDigitAscii(2, humid[4]);
//     matrix2.writeDigitRaw(3,  0b0000110000100100);
    matrix2.writeDigitAscii(3, humid[5]);
//     matrix2.print(humidity);
     matrix2.writeDisplay();  
     }
}

//
//void derp(){
//  return;
//  
//  // Listenning for new clients
//  WiFiClient client = server.available();
//  
//  if (client) {
//    Serial.println("New client");
//    // bolean to locate when the http request ends
//    boolean blank_line = true;
//    while (client.connected()) {
//      if (client.available()) {
//        char c = client.read();
//        
//        if (c == '\n' && blank_line) {
//            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//            float h = dht.readHumidity();
//            // Read temperature as Celsius (the default)
//            float t = dht.readTemperature();
//            // Read temperature as Fahrenheit (isFahrenheit = true)
//            float f = dht.readTemperature(true);
//            // Check if any reads failed and exit early (to try again).
//            if (isnan(h) || isnan(t) || isnan(f)) {
//              Serial.println("Failed to read from DHT sensor!");
//              strcpy(celsiusTemp,"Failed");
//              strcpy(fahrenheitTemp, "Failed");
//              strcpy(humidityTemp, "Failed");         
//            }
//            else{
//              // Computes temperature values in Celsius + Fahrenheit and Humidity
//              float hic = dht.computeHeatIndex(t, h);//, false);       
//              dtostrf(hic, 6, 2, celsiusTemp);             
//              float hif = dht.computeHeatIndex(f, h);
//              dtostrf(hif, 6, 2, fahrenheitTemp);         
//              dtostrf(h, 6, 2, humidityTemp);
//              // You can delete the following Serial.print's, it's just for debugging purposes
//              Serial.print("Humidity: ");
//              Serial.print(h);
//              Serial.print(" %\t Temperature: ");
//              Serial.print(t);
//              Serial.print(" *C ");
//              Serial.print(f);
//              Serial.print(" *F\t Heat index: ");
//              Serial.print(hic);
//              Serial.print(" *C ");
//              Serial.print(hif);
//              Serial.print(" *F");
//              Serial.print("Humidity: ");
//              Serial.print(h);
//              Serial.print(" %\t Temperature: ");
//              Serial.print(t);
//              Serial.print(" *C ");
//              Serial.print(f);
//              Serial.print(" *F\t Heat index: ");
//              Serial.print(hic);
//              Serial.print(" *C ");
//              Serial.print(hif);
//              Serial.println(" *F");
//            }
//            client.println("HTTP/1.1 200 OK");
//            client.println("Content-Type: text/html");
//            client.println("Connection: close");
//            client.println();
//            // your actual web page that displays temperature and humidity
//            client.println("<!DOCTYPE HTML>");
//            client.println("<html>");
//            client.println("<head></head><body><h1>ESP8266 - Temperature and Humidity</h1><h3>Temperature in Celsius: ");
//            client.println(celsiusTemp);
//            client.println("*C</h3><h3>Temperature in Fahrenheit: ");
//            client.println(fahrenheitTemp);
//            client.println("*F</h3><h3>Humidity: ");
//            client.println(humidityTemp);
//            client.println("%</h3><h3>");
//            client.println("</body></html>");     
//            break;
//        }
//        if (c == '\n') {
//          // when starts reading a new line
//          blank_line = true;
//        }
//        else if (c != '\r') {
//          // when finds a character on the current line
//          blank_line = false;
//        }
//      }
//    }  
//    // closing the client connection
//    delay(1);
//    client.stop();
//    Serial.println("Client disconnected.");
//  }
//}   
