#include "flash.h"

//Конфигурация выводов под SPI
void setupSPI(){
    pinMode(14, OUTPUT);
    pinMode(MOSI, OUTPUT);
    pinMode(CS, OUTPUT);
    pinMode(MISO, INPUT);
    Serial.println();
    Serial.println("In setupSPI.");
  }

//Отправка 
void sendByteArray(char* data, int len){
    digitalWrite(CS, LOW);
    for(int i = 0; i < len; ++i){
        sendByte(data[i]);
      }
    digitalWrite(CS, HIGH);
  }

//Отправка одного байта. Старшим битом вперед.
void sendByte(char data){
//  Serial.println();
//  Serial.println("In sending.");
  for(int i = 7; i >= 0; --i){
      digitalWrite(CLK, LOW);
      digitalWrite(MOSI, (data>>i)&0x01);
      digitalWrite(CLK, HIGH);
    }
  }

//Разрешение записи. Эта функция необходима перед операциями PROGRAM, ERASE, WRITE.
void writeEnable(){
  digitalWrite(CS, LOW);
  sendByte(WRITE_ENABLE);
  digitalWrite(CS, HIGH);
  }

void subsectorErase(int address){
  char adr[3];                              //Массив для адреса.
  adr[0] = address & 0x000000ff;            //Младший байт адреса.
  adr[1] = (address >> 8) & 0x000000ff;
  adr[2] = (address >> 16) & 0x000000ff;    //Старший байт адреса.
  writeEnable();                            //Разрешение записи.
  digitalWrite(CS, LOW);        
  sendByte(SUBSECTOR_ERASE);                //Отправка команды очистки подсектора.
  for(int i = 2; i >= 0; --i)
    sendByte(adr[i]);                       //Отправка трех байт адреса.
  digitalWrite(CS, HIGH);
  delay(100);
  }

//Страничная запись данных. Записывает во флешку массив data начиная с адреса address, len байт.
void pageProgram( int address, char* data, int len){
  char adr[3];                              //Массив для адреса.
  adr[0] = address & 0x000000ff;            //Младший байт адреса. Если он не 00, то запись всех данных начнется с нулевого адреса следующей страницы памяти.
  adr[1] = (address >> 8) & 0x000000ff;
  adr[2] = (address >> 16) & 0x000000ff;    //Старший байт адреса.
  writeEnable();                            //Разрешение записи.
  digitalWrite(CS, LOW);        
  sendByte(PAGE_PROGRAM);                   //Отправка команды записи.
  for(int i = 2; i >= 0; --i)
    sendByte(adr[i]);                       //Отправка трех байт адреса.
  for(int i = 0; i < len; ++i)
    sendByte(data[i]);                      //Отправка данных.
  digitalWrite(CS, HIGH);
  delay(100);
  }

//Страничная запись данных. Записывает во флешку массив data начиная с адреса address, len байт.
void pageProgramByte( int address, char data){
  char adr[3];                              //Массив для адреса.
  adr[0] = address & 0x000000ff;            //Младший байт адреса. Если он не 00, то запись всех данных начнется с нулевого адреса следующей страницы памяти.
  adr[1] = (address >> 8) & 0x000000ff;
  adr[2] = (address >> 16) & 0x000000ff;    //Старший байт адреса.
  digitalWrite(CS, LOW);
  writeEnable();                            //Разрешение записи.
  delay(10);
  digitalWrite(CS, HIGH);
  delay(10);
  digitalWrite(CS, LOW);        
  sendByte(PAGE_PROGRAM);                   //Отправка команды записи.
  delay(10);
  for(int i = 2; i >= 0; --i)
    sendByte(adr[i]);                       //Отправка трех байт адреса.
    delay(10);
//  for(int i = 0; i < len; ++i)
    sendByte(data);                         //Отправка данных.
  digitalWrite(CS, HIGH);
    Serial.print("Print writing byte. = ");
    Serial.print((int)data);
    Serial.println();
    delay(100);
  }

//Чтение байта данных.
char readByte(){
  char temp = 0x00;                 //Временная переменная для чтения
  char data = 0x00;                 //Переменная для считывания байта данных
  digitalWrite(CLK, HIGH);
    for(int i = 7; i >= 0; --i){
      digitalWrite(CLK, LOW);
      digitalWrite(CLK, HIGH);
      temp = digitalRead(MISO);     //Запись во временную переменную текущего значения
//      Serial.print(temp);         //Вывод текущего значения в сериал порт. Отладка.
//      Serial.print(" ");
      
      data |= (temp << i);
    }
  Serial.print("Print reading byte. = ");
  Serial.print((int)data);             //Вывод считанного байта в сериал порт. Отладка.
  Serial.println();
  return data;
  }

//Чтение данных из памяти в массив.Чтение из флешки начиная с адреса address в массив data, len байт.
void readDataBytes(int address,char* data, int len){
  data = new char[len];
  for(int i = len - 1; i >= 0; --i)
    data[i] = 0;
  char adr[3];                              //Массив для адреса.
  adr[0] = address & 0x000000ff;            //Младший байт адреса. Если он не 00, то запись всех данных начнется с нулевого адреса следующей страницы памяти.
  adr[1] = (address >> 8) & 0x000000ff;
  adr[2] = (address >> 16) & 0x000000ff;    //Старший байт адреса.
  digitalWrite(CS, LOW);        
  sendByte(READ_DATA_BYTES);                //Отправка команды чтения данных из памяти.
  for(int i = 2; i >= 0; --i)
    sendByte(adr[i]);                       //Отправка трех байт адреса.
  for(int i = len - 1; i >= 0; --i)
    data[i] = readByte();                   //Чтение данных в массив.
  digitalWrite(CS, HIGH);
//  Serial.println("Print reading byte array.");
//  for(int i = 0; i < len; ++i)
//    Serial.print(data[i]);                  //Вывод считанного массива в последовательнай порт. Отладка.
//  Serial.println();
  //return data;
  }
  
//Чтение данных из памяти
char readDataByte(int address){
  char data;
  char adr[3];                              //Массив для адреса.
  adr[0] = address & 0x000000ff;            //Младший байт адреса. Если он не 00, то запись всех данных начнется с нулевого адреса следующей страницы памяти.
  adr[1] = (address >> 8) & 0x000000ff;
  adr[2] = (address >> 16) & 0x000000ff;    //Старший байт адреса.
  digitalWrite(CS, LOW);        
  sendByte(READ_DATA_BYTES);                //Отправка команды чтения данных из памяти.
  for(int i = 2; i >= 0; --i)
    sendByte(adr[i]);                       //Отправка трех байт адреса.
//  for(int i = len - 1; i >= 0; --i)
    data = readByte();                   //Чтение данных в массив.
  digitalWrite(CS, HIGH);
//  Serial.println("Print reading byte array.");
////  for(int i = 0; i < len; ++i)
//    Serial.print(data);                  //Вывод считанного массива в последовательнай порт. Отладка.
//  Serial.println();
  return data;
  }
