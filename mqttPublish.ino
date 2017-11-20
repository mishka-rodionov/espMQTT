// Функция отправки показаний с термодатчика
void TempSend(){
  if (tm==0)
  {
  sensors.requestTemperatures();   // от датчика получаем значение температуры
  float temp = sensors.getTempCByIndex(0);
  client.publish("test/temp",String(temp)); // отправляем в топик для термодатчика значение температуры
  //Serial.println(temp);
  tm = 300;  // пауза меду отправками значений температуры  коло 3 секунд
  }
  tm--; 
  delay(10);  
}
