// Светодиод подлкючен к 5 пину
// Датчик температуры ds18b20 к 2 пину

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <string.h>
#include <ESP8266WebServer.h>
#include "flash.h"

#define ONE_WIRE_BUS 2
#define CLK   14
#define MOSI  13
#define MISO  12
#define CS    15
#define READ_ID 0x9E
#define PAGE_PROGRAM 0x02
#define WRITE_ENABLE 0x06
#define READ_DATA_BYTES 0x03
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/*const*/ char *ssid = "0";//"Keenetic-6947";  // Имя вайфай точки доступа
/*const*/ char *pass;//"SRtRzM8a"; // Пароль от точки доступа

const char *mqtt_server = "m20.cloudmqtt.com"; // Имя сервера MQTT
const int mqtt_port = 15305; // Порт для подключения к серверу MQTT
const char *mqtt_user = "pavgnvtr"; // Логи от сервер
const char *mqtt_pass = "fpvVr7L5Ke3y"; // Пароль от сервера

String HTML_Root = "<html> <body>\
 <form  action = \"/post\" method = \"POST\">\
 <input name = \"login\">\
 <input type = \"password\" name = \"pass\">\
 <input type = \"submit\"> </form>\
</body> </html>";

int addressUserSubsector = 0x100000;
int addressSSID       = 0x100100;   //Адрес хранения пользовательского SSID сети WiFi
int addressSSIDlength = 0x100200;   //Адрес хранения длины в символах пользовательского SSID
int addressPASS       = 0x100300;   //Адрес хранения пользовательского пароля сети WiFi
int addressPASSlength = 0x100400;   //Адрес хранения длины в символах пользовательского пароля
int addressDummy      = 0x100501;   //Адрес хранения заглушки для первичной настройки устройства

#define BUFFER_SIZE 100

bool LedState = false;
int tm=300;
float temp=0;
const int led = 13;
char dummyData = 17;
char ssidLength = 0;
char passLength = 0;
int cnt = 0;

ESP8266WebServer server ( 80 );

WiFiClient wclient;      
PubSubClient client(wclient, mqtt_server, mqtt_port);

void setup() {
  
  sensors.begin();
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  pinMode(5, OUTPUT);
  setupSPI();
  //initWiFi();
  setupWebServer();
}

//Основной цикл
void loop() {
  delay(2000);
 //Часть кода отвечающая за первичную инициализацию устройства. Специальная заглушка определяет, нужно ли запускать HTTP сервер
 //для изменения параметров WiFi сети устройства. При однократной смене логина и пароля WiFi сети устройства, данный кусок кода 
 //больше не используется и для этого происходит запись заглушки во флеш память.
  dummyData = readDataByte(addressDummy);
  Serial.print("Dummy data = ");
  Serial.print((int)dummyData);
  Serial.println();
  /*subsectorErase(0x100000);
  
  pageProgramByte(addressDummy, 1 + cnt);
  delay(100);
  dummyData = readDataByte(addressDummy);
  Serial.print("Dummy data = ");
  Serial.print((int)dummyData);
  Serial.println();
  cnt++;
  //while(1);
  pageProgramByte(addressSSIDlength, 7);
  pageProgramByte(addressPASSlength, 9);
  
  ssidLength = readDataByte(addressSSIDlength);
  Serial.print("ssidLength data = ");
  Serial.print((int)ssidLength);
  Serial.println();
  passLength = readDataByte(addressPASSlength);
  Serial.print("passLength data = ");
  Serial.print((int)passLength);
  Serial.println();
  readDataBytes(addressSSID, ssid, (int)ssidLength);
  readDataBytes(addressPASS, pass, (int)passLength);
  Serial.println("Print array");
    for(int i = 0; i < strlen(ssid); ++i){
      Serial.print((char)ssid[i]);
      Serial.print(" ");
    }
    Serial.println();
    for(int i = 0; i < strlen(pass); ++i){
      Serial.print((char)pass[i]);
      Serial.print(" ");
    }
    Serial.println();
  */ 
  if((int)dummyData != DUMMY){
    Serial.print("Length ssid = ");
    Serial.println(strlen(ssid));
    while(strlen(ssid) <= 1){
      server.handleClient();
      //delay(10);  
    }
    Serial.println("ok");
    server.stop();
    server.close();
    //Запись введенных пользователем данных во флешку.
    subsectorErase(addressUserSubsector);
    pageProgram(addressSSID, ssid, strlen(ssid));
    pageProgram(addressPASS, pass, strlen(pass));
    pageProgramByte(addressSSIDlength, (char) strlen(ssid));
    pageProgramByte(addressPASSlength, (char) strlen(pass));
    dummyData = DUMMY;
    pageProgramByte(addressDummy, dummyData);
  }
  Serial.println("OK");
  //Считывание пользовательских данных WiFi сети из флешки.
  ssidLength = readDataByte(addressSSIDlength);
  passLength = readDataByte(addressPASSlength);
  readDataBytes(addressSSID, ssid, (int)ssidLength);
  readDataBytes(addressPASS, pass, (int)passLength);
//  char readingSSID[strlen(ssid)];
//  for(int i = 0; i < strlen(ssid); ++i)
//    readingSSID[i] = 0;
//  char readingPASS[strlen(pass)];
//  for(int i = 0; i < strlen(pass); ++i)
//    readingPASS[i] = 0;
//  readDataBytes(addressSSID, readingSSID, strlen(ssid));
//  readDataBytes(addressPASS, readingPASS, strlen(pass));
    Serial.println("Print array");
    for(int i = 0; i < strlen(ssid); ++i){
      Serial.print((char)ssid[i]);
      Serial.print(" ");
    }
    Serial.println();
    for(int i = 0; i < strlen(pass); ++i){
      Serial.print((char)pass[i]);
      Serial.print(" ");
    }
    Serial.println();
  

  // подключаемся к wi-fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  // подключаемся к MQTT серверу
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server");
      if (client.connect(MQTT::Connect("arduinoClient2")
                         .set_auth(mqtt_user, mqtt_pass))) {
        Serial.println("Connected to MQTT server");
        client.set_callback(callback);
        client.subscribe("test/led"); // подписывааемся по топик с данными для светодиода
      } else {
        Serial.println("Could not connect to MQTT server");   
      }
    }

    if (client.connected()){
      client.loop();
      TempSend();
  }
  
}
} // конец основного цикла

// Функция получения данных от сервера

void callback(const MQTT::Publish& pub)
{
  Serial.print(pub.topic());   // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print(pub.payload_string()); // выводим в сериал порт значение полученных данных
  
  String payload = pub.payload_string();
  
  if(String(pub.topic()) == "test/led") // проверяем из нужного ли нам топика пришли данные 
  {
  int stled = payload.toInt(); // преобразуем полученные данные в тип integer
  digitalWrite(5,stled);  //  включаем или выключаем светодиод в зависимоти от полученных значений данных

  //Отладка работы с флеш памятью.

  //Конец отладки работы с флеш памятью.
}
}
