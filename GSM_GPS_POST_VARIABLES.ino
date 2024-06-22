const char apn[]      = "internet.movistar.com.co"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "movistar"; // GPRS User
const char gprsPass[] = "movistar"; // GPRS Password
#include "max6675.h"
// termocupla
int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;
const int numMuestras = 10;
int valores[numMuestras]; // Array para almacenar las muestras
int indice = 0; // Índice actual en el array
int sumaTotal = 0; // Suma total de las muestras

float lat = 0;
float lon = 0;
float speed  = 0;

int arranque = 23;
int bateria = 21;
int panic_button = 32; //15
// Server details
// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
const char server[] = "migote.000webhostapp.com"; // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/map5_mg/insert.php";         // resource path, for example: /post-data.php
const int  port = 80;                             // server port number

int rele = 13;

// Set serial for AT commands (to SIM800 module)
#define INTERVAL 120000
#define INTERVAL_1 5000

uint32_t lastTime = 0;
uint32_t lastTime_1 = 0;
// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM808      // Modem is SIM800
#include <TinyGsmClient.h>
#include <Arduino_JSON.h>

TinyGsm modem(Serial2);
TinyGsmClient client(modem);
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  Serial.begin(115200);
  pinMode(rele, OUTPUT);
  pinMode(bateria, INPUT_PULLDOWN);
  pinMode(arranque, INPUT_PULLDOWN);
  pinMode(panic_button, INPUT_PULLDOWN);
  
  // Set GSM module baud rate and UART pins
  //Serial2.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  Serial2.begin(115200);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();
  // use modem.init() if you don't need the complete restart
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
  
  float tempe = thermocouple.readCelsius();
  float temperatura = lectura_temperatura(tempe);
  Serial.println(temperatura);
  
  if(lat>0.000 && lat <2.5 && lon>-80.00 && lon <-75.000){
    Serial.println("enviando_1");  
  
  unsigned long now_1 = millis();
  if(now_1 - lastTime_1 > INTERVAL_1)
  {

  if(!client.connected())
    {
      Serial.println("Conectando a servidor");
      if (!client.connect(server, port)) {
        Serial.println(" fail");
        return;
      }
    }
    Serial.println("Enviando_2");
 
  float lec = map(analogRead(34),3105,3560,0,100);// gasolina
  lec = map(lec,216,165,0,100);
  //float lec = map(analogRead(34),0,4095,0,100);// potenciometro
    
      // Making an HTTP POST request
      Serial.println("Performing HTTP POST request...");
      // Prepare your HTTP POST request data (Temperature in Celsius degrees)
      String RequestData = "lat="+String(lat, 8)+"&lng="+String(lon, 8)+"&var_1="+String(temperatura)+"111&var_2="+String(speed*1.852)+"&var_3="+String(lec)+"&switch="+String(digitalRead(bateria))+"&door="+String(digitalRead(arranque))+"&alarma="+String(!digitalRead(panic_button));
    Serial.println(RequestData);
    HTTP(RequestData);
      

    
  client.stop();
       lastTime_1 = now_1;
  }
  }
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
          String datos = json(c);
          Serial.println(datos);
          
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
        }      
      }
}

String json(String dat){
  int startIndex = dat.indexOf("{");
  int endIndex = dat.lastIndexOf("}");

  if (startIndex != -1 && endIndex != -1) {
    // Extraer la parte del JSON
    String jsonPart = dat.substring(startIndex, endIndex + 1);
    return jsonPart;

}
}

void GPS_1(){

  float alt     = 0;
  int   vsat     = 0;
  int   usat     = 0;
  float accuracy = 0;
  int   year     = 0;
  int   month    = 0;
  int   day      = 0;
  int   hour     = 0;
  int   min      = 0;
  int   sec      = 0;
  unsigned long now = millis();
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
      lastTime = now;
      break;
    } 
    else {
      Serial.println("Couldn't get GPS/GNSS/GLONASS location, retrying in 15s.");
      delay(100);
    }
  }

  //Se passou o intervalo de envio
  if(now - lastTime > INTERVAL)
  {
    Serial.println("reiniciando");
        ESP.restart();
         return;
        lastTime = now;
  }
 }

 float lectura_temperatura(float lec){
  // Almacenar el valor en la lista y actualizar la suma total
  sumaTotal += lec - valores[indice];
  valores[indice] = lec;

  // Actualizar el índice para la siguiente lectura
  indice = (indice + 1) % numMuestras;

  // Calcular y mostrar el promedio excluyendo el valor máximo y mínimo
  int minValor = valores[0];
  int maxValor = valores[0];

  for (int i = 1; i < numMuestras; i++) {
    if (valores[i] < minValor) {
      minValor = valores[i];
    } else if (valores[i] > maxValor) {
      maxValor = valores[i];
    }
  }

  int promedio = (sumaTotal - minValor - maxValor) / (numMuestras - 2);

  delay(500); // Ajusta el tiempo según sea necesario
  return promedio;

}

  
