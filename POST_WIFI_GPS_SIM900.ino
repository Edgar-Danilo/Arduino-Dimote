// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM808      // Modem is SIM800
#include <TinyGsmClient.h>
#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char* ssid = "Flia MORILLO";
const char* password = "27203227NG";
//const char* serverName = "http://10.0.0.41/login/archivo-php/insertar.php";
const char* serverName = "http://4.246.229.161/dimote/archivo-php/insertar.php";

float lat = 0;
float lon = 0;
float speed  = 0;
float alt     = 0;
int   vsat     = 0;
int   usat     = 0;
float accuracy = 0;



TinyGsm modem(Serial2);

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  Serial.begin(115200);

  //wifi
    WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  
  // Set GSM module baud rate and UART pins
  //Serial2.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  Serial2.begin(115200);
  delay(3000);



    Serial.print("Encendiendo GPS");
    Serial2.println("AT+CGNSPWR=1");
    delay(500);
    Serial.println("iniciando GPS");

     if (modem.waitResponse(10000L) != 1) {
    Serial.println(" SGPIO=0,4,1,1 false ");
     }
   modem.enableGPS();

  
}

void loop() {
  
  GPS_1();

      // wait for WiFi connection
    if(WiFi.status()== WL_CONNECTED && lat != 0) {
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      

     String httpRequestData = "input="+demoCreation();
     Serial.println(httpRequestData);        
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        // httpCode will be negative on error
        if(httpResponseCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.println("conectado con exito");

            // file found at server
            if(httpResponseCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
        }

        http.end();
    }
  delay(5000);
  
}


void GPS_1(){


  int   year     = 0;
  int   month    = 0;
  int   day      = 0;
  int   hour     = 0;
  int   min      = 0;
  int   sec      = 0;
  for (int8_t i = 15; i; i--) {
    Serial.println("Requesting current GPS/GNSS/GLONASS location");
    if (modem.getGPS(&lat, &lon, &speed, &alt, &vsat, &usat, &accuracy,
                     &year, &month, &day, &hour, &min, &sec)) {
      Serial.println("Latitude: " + String(lat, 8) + "\tLongitude: " + String(lon, 8));
      Serial.println("Speed: " + String(speed) + "\tAltitude: " + String(alt));
      Serial.println("Visible Satellites: " + String(vsat) + "\tUsed Satellites: " + String(usat));
      Serial.println("Accuracy: " + String(accuracy));
      Serial.println("Year: " + String(year) + "\tMonth: " + String(month) + "\tDay: " + String(day));
      Serial.println("Hour: " + String(hour) + "\tMinute: " + String(min) + "\tSecond: " + String(sec));
      break;
    } 
    else {
      Serial.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
      delay(100);
    }
  }

 }
 String demoCreation() {
  Serial.println("creation");
  Serial.println("========");

  JSONVar myArray;
  JSONVar myArray1;
  JSONVar myArraycontext;
  JSONVar myArrayinput;

  myArraycontext["lat"]= String(lat, 8);
  myArraycontext["lng"]= String(lon, 8);
  
  myArrayinput["Altura"]=String(alt);
  myArrayinput["Velocidad"]=String(speed);
  myArrayinput["Precision"]=String(accuracy,2);
  myArrayinput["Sat_Visibles"]=String(vsat);
  myArrayinput["Sat_Usados"]=String(usat);

  myArray1["disp"]="Esp32";
  myArray1["context"]=myArraycontext;
  myArray1["input"]=myArrayinput;
  
  myArray["api"] = "Danilo";
  myArray["data"] = myArray1;


  //Serial.print("myArray.length() = ");
  //Serial.println(myArray.length());

  // JSON.stringify(myVar) can be used to convert the JSONVar to a String
  String jsonString = JSON.stringify(myArray);

  //Serial.print("JSON.stringify(myArray) = ");
  //Serial.println(jsonString);
  Serial.println();

  //Serial.println(jsonString);
  return(jsonString);
}
