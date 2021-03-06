#include <Time.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <ESP8266HTTPClient.h>


#include "SSD1306.h"
#include "confi.h"
#include "NTP.h"
#include "OLED_Display.h"
#include "web.h"

//#include "sensores.h"

#define USE_SERIAL Serial
#define WiFi_Logo_width 60
#define WiFi_Logo_height 36



ESP8266WiFiMulti WiFiMulti;

NTP ntp;


const int id = ESP.getChipId();

//inicializacion con valores no validos
int hora = 25;
int minuto= 62;
int segundo = 62;

const char WiFi_Logo_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0x07, 0xC0, 0x83, 0x01, 0x80, 0xFF, 0xFF, 0xFF,
  0x01, 0x00, 0x07, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0C, 0x00,
  0xC0, 0xFF, 0xFF, 0x7C, 0x00, 0x60, 0x0C, 0x00, 0xC0, 0x31, 0x46, 0x7C,
  0xFC, 0x77, 0x08, 0x00, 0xE0, 0x23, 0xC6, 0x3C, 0xFC, 0x67, 0x18, 0x00,
  0xE0, 0x23, 0xE4, 0x3F, 0x1C, 0x00, 0x18, 0x00, 0xE0, 0x23, 0x60, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xE0, 0x03, 0x60, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x07, 0x60, 0x3C, 0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C,
  0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00, 0xE0, 0x8F, 0x71, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xC0, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x08, 0x00,
  0xC0, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x0C, 0x00, 0x80, 0xFF, 0xFF, 0x1F,
  0x00, 0x00, 0x06, 0x00, 0x80, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x07, 0x00,
  0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};




int progress;


void confHora(){
  while ( hora > 24 ){hora = ntp.get_hour();}
  while ( minuto > 60 ){minuto = ntp.get_minutes();}
  while ( segundo > 60 ){segundo = ntp.get_secons();}
  hora = hora + UTC;//Implementar deteccion de horairo de verano/invierno
  setTime(hora,minuto,segundo,16,04,2017);//¡Implentar la getFecha en la libreria NTP!  
}

String return_hora(){
  time_t t = now();  
  return String(hour(t))+":"+String(minute(t));
}

void setup() {

    display.init();
    if(flipDisplay) display.flipScreenVertically();
    display.drawXbm(34, 20, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    display.display();
    USE_SERIAL.begin(115200);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    
    WiFi.begin(ssid,password);
    rgb_begin();

  while ( WiFi.status() != WL_CONNECTED ) {
    progress= progress + 20;
    delay ( 500 );
    Serial.print ( "." );
    //display.print(".");
    if(progress>=100) progress=0;
    display.drawProgressBar(0, 0, 120, 10, progress);
    display.display();
  }

 Serial.println ( "" );
 Serial.print ( "Connected to " );
 Serial.println ( ssid );
 Serial.print ( "IP address: " );
 Serial.println ( WiFi.localIP() );
 Serial.print("Chip ID");
 Serial.println(id);

 ntp.begin();
 confHora();
 
 if ( MDNS.begin ( "esp8266" ) ) {
  Serial.println ( "MDNS responder started" );
 }

  //Configuracion de la navegacion
  server.on("/", bienvenida);
  server.on("/control", control);
  server.on("/lecturas", lecturas);
  server.on("/about", about);


//Inicializacion del servidor web
  server.begin();

 Serial.println ( "HTTP server started" );
 display.clear();
 
 String ssid_str = String(ssid);
 pantalla_info(id,ssid_str, WiFi.localIP().toString());
 delay(3000);
}//Fin de la función Setup

void loop() {
  //Lectura de sensores 
  
  Serial.print("H:");
  Serial.println(readHumedadStr());
  Serial.print("T:");
  Serial.println(readTemperaturaStr());
  
  pantalla_datos(return_hora(), readTemperatura(), readHumedad());
    
    //Si tenemos conexión a internet enviamos los datos al servidor
    
    if((WiFiMulti.run() == WL_CONNECTED)) {
        //cast de datos
        //String humedadNow = String(humedad); 
        //String temperaturaNow = String(temperatura); 
        
        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        
        //Envia la info para ser almacenada en el servidor
        url = url += id;
        url = url += "&temperatura=";
        url = url += readTemperaturaStr();
        url = url += "&humedad=";
        url = url += readHumedadStr();
        
        http.begin(url);

        //Respuesta del servidor (¡¡IMPLEMENTAR SISTEMA DE COMPROBACIÓN!!)
        USE_SERIAL.print("[HTTP] GET...\n");
      
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        //Cierra la conexion
        http.end();
    }

    delay(frecuencia);
    
    server.handleClient();
}

