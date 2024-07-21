// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM808      // Modem is SIM800
#include <TinyGsmClient.h>
#include <Arduino_JSON.h>

const char apn[]      = "internet.movistar.com.co"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "movistar"; // GPRS User
const char gprsPass[] = "movistar"; // GPRS Password

//const char* serverName = "http://10.0.0.41/login/archivo-php/insertar.php";
const char* serverName = "http://4.246.229.161/dimote/archivo-php/insertar.php";

// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
const char server[] = "4.246.229.161"; // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/dimote/archivo-php/insertar.php";         // resource path, for example: /post-data.php
const int  port = 80;  

float lat = 0;
float lon = 0;
float speed  = 0;
float alt     = 0;
int   vsat     = 0;
int   usat     = 0;
float accuracy = 0;



TinyGsm modem(Serial2);
TinyGsmClient client(modem);

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  Serial.begin(115200);
  
  // Set GSM module baud rate and UART pins
  //Serial2.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  Serial2.begin(115200);
  delay(3000);

  // use modem.init() if you don't need the complete restart
  //modem.init();
  Serial.print("conectando a");
  Serial.println(apn);
  if (!modem.gprsConnect("internet.movistar.com.co", "movistar", "movistar")) {
    Serial.println(" fail");
    return;
  }
  else {
    Serial.print("Connecting to ");
    Serial.print(server);
    if (!client.connect(server, port)) {
      Serial.println(" fail");
      return;
    }
  }

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
    if(!client.connected())
    {
      Serial.println("Conectando a servidor");
      if (!client.connect(server, port)) {
        Serial.println(" fail");
        return;
      } return;
    }
    Serial.println("Enviando ..");
  
    Serial.println("Performing HTTP POST request...");
    // Prepare your HTTP POST request data (Temperature in Celsius degrees)
    String RequestData = "input="+demoCreation();
    Serial.println(RequestData);
    if(lat != 0){
      HTTP(RequestData);
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

  myArraycontext["lat"]= lat;
  myArraycontext["lng"]= lon;
  
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

void HTTP(String httpRequestData){
  client.print(String("POST ") + resource + " HTTP/1.1\r\n");
      client.print(String("Host: ") + server + "\r\n");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(httpRequestData.length());
      client.println();
      client.println(httpRequestData);
      String c;
      while (client.connected()) {
        // Print available data (HTTP response from server)
        while (client.available()) {
          Serial.println("INICIO DE LA RESPUESTA.....");
          c = client.readString();
          Serial.print(c);
          Serial.println("FIN DE LA RESPUESTA....");
          //String datos = json(c);
          //Serial.println(datos);
          /*
          JSONVar json = JSON.parse(datos);
      
          if (JSON.typeof(json) == "undefined") {
            Serial.println("Error al deserializar JSON");
          } else {
            // Acceder a los datos del JSON
            String request = json["request"];
            String estado_pin = json["valor"];
            
            
            // Imprimir los datos obtenidos
            Serial.print("request: ");
            Serial.println(request);
            Serial.print("estado: ");
            Serial.println(estado_pin);
            if (estado_pin == "1"){
            digitalWrite(rele, HIGH);
            }else{
              digitalWrite(rele, LOW);
            }
            //datos = "";
            
          }
          */
        }      
      }
}
