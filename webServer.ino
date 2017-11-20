const char *serverLogin = "Device";
const char *serverPassword = "device12345";

//Создание веб-сервера для первичного изменения параметров сети
void setupWebServer(void) {
  initWiFi();
pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  //WiFi.begin(ssid, pass);
  Serial.println("");

  // Wait for connection
  /*while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }*/
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.println("IP address: ");
  Serial.println(WiFi.status());
  
  //Serial.println(WiFi.localIP());

//  if (MDNS.begin("esp8266")) {
//    Serial.println("MDNS responder started");
//  }

  server.on("/", handleRoot);

  server.on ( "/post", HTTP_POST, handlePOST );

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

//  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void handlePOST() {
//  test();
  Serial.println("POST handler");
  // ElapsedStr( tmpstr );
  // Serial.println( tmpstr );
  String login = server.arg("login");
  String password = server.arg("pass");

  ssid = new char[login.length() + 1];
  strcpy(ssid, login.c_str());
  pass = new char[password.length() + 1];
  strcpy(pass, password.c_str());

  for(int i = 0; i < strlen(ssid); ++i)
    Serial.print(ssid[i]);
  Serial.println();
  for(int i = 0; i < strlen(pass); ++i)
    Serial.print(pass[i]);

  Serial.println("ARGUMENTS " + login + password);
  server.send ( 200, "text/html",  login + password);
//  if (val1.equals("on"))
//    LED_ON;
//  if (val1.equals("off"))
//    LED_OFF;
}

void initWiFi(void) {

  WiFi.softAP(serverLogin, serverPassword );
  delay(50);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", HTML_Root);
  digitalWrite(led, 0);
}

