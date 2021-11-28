#include "config.h"

Button button1 = {SensorPin, false};
Button resetButton = {ResetButtonPin, false};

WiFiServer server(80);
WiFiServer server2(80);




void setup(void)
{
  Setup();
  EEPROM_Setup();
  connectWifi();
}

void loop(void)
{
  Serial.println("in loop");
  interpt();
  button_Click();
  handleSeverClient();
  resetCounter();
}





















void IRAM_ATTR isr2()
{
  Serial.println("ISR2 Called");
  resetButton.pressed = true;
  ResetFlag = 1;
}

void IRAM_ATTR isr1()
{
  Serial.println("ISR1 Called");
  button1.pressed = true;
  prev_millis = millis();
}
/********************************************** Setup Functions *********************************************/
void Setup(void)
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(button1.PIN, INPUT_PULLUP); // agreed
  pinMode(ResetButtonPin, INPUT_PULLUP);
  pinMode(AccessPointPin, INPUT_PULLUP);

  attachInterrupt(button1.PIN, isr1, HIGH);
  attachInterrupt(resetButton.PIN, isr2, HIGH);
  Serial.println("Done Setup Func");
}
void EEPROM_Setup(void)
{
  EEPROM.begin(EEPROM_SIZE);
  ssid = readStringFromFlash(0); // Read SSID stored at address 0
  Serial.print("SSID = ");
  Serial.println(ssid);
  password = readStringFromFlash(40); // Read Password stored at address 40
  Serial.print("passwords = ");
  Serial.println(password);
  Serial.println("Done EEPROM Setup Func");
}
void connectWifi()
{
  if (readStringFromFlash(100) == "1")
  {
    Gen_access_point();
  }
  button_Click();
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    if (readStringFromFlash(100) == "1")
      {
        Gen_access_point();
      }
    Serial.print(".");
    delay(500);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    Serial.println("HTTP server started");
  }
  Serial.println("Done WiFi_Setup Func");
}


/********************************************** Loop Functions *********************************************/
void short_interpt()
{
  now_millis = millis();
  if ((button1.pressed == true) && (now_millis > prev_millis) && (digitalRead(button1.PIN) == 1))
  {
    button1.pressed = false;
    SensorValue++;
    Serial.println(SensorValue);
  }
}
void interpt()
{
  now_millis = millis();
  if ((button1.pressed == true) && (now_millis > prev_millis) && (digitalRead(button1.PIN) == 1))
  {
    button1.pressed = false;
    Serial.println("Interrupt Release");
    SensorValue++;
    Serial.printf("Button 1 has been pressed %u times\n", SensorValue);
  }
  Serial.print(button1.pressed);
  Serial.print("  ");
  Serial.print(digitalRead(button1.PIN));
  Serial.print("  ");
  Serial.println(SensorValue);
  delay(1000);
}
void button_Click()
{
  Serial.println("Inside Button_Click");
//  if ( !digitalRead(ResetButtonPin) ) {
//    delay(4000);
//    if ( !digitalRead(ResetButtonPin) ) {
//      Serial.println("Inside Button_Click_Reset_Loop");
//      Serial.println("ResetButton clicked");
//      resetCounter();
//    }
//  }
  if ( !digitalRead(AccessPointPin) ) {
    delay(4000);
    if ( !digitalRead(AccessPointPin) ) {
      Serial.println("Inside Button_Click_Access_loop");
      Serial.println("AccessPoint clicked");
      writeStringToFlash("1", 100);
      ESP.restart();
    }
  }
}
void handleSeverClient()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      short_interpt();
      delay(1);
      //Serial.println("Help iam stuck !!!!!");
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /reset") >= 0) {
              resetCounter();
            }
            client.println(returnHtml(SensorValue));
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}




/********************************************** Helpful Functions *********************************************/
void resetCounter()
{
  if(ResetFlag==1)
  {
    Serial.println("Reset Button Clicked");
    sendData("value=" + String(SensorValue));
    SensorValue = 0;
    Serial.println("new value posted");
  }
}
void Gen_access_point()
{
  Serial.println("Inside Gen_accesss_point");
  WiFi.softAP("wifi", "88888888");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server2.begin();

  while (true) {
    client_handle();
    if (exit_but) {
      exit_but = false;
      break;
    }
  }
  writeStringToFlash("0", 100);
  ESP.restart();
}
void sendData(String params)
{

  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  Serial.println(url);
  Serial.println("Making a request");
  http.begin(url);
  delay(2000);
  int httpCode = http.GET();
  http.end();
  if (httpCode == HTTP_CODE_OK) {
    Serial.println(":OK ");
  }
  Serial.println(": done " + httpCode);
}
void writeStringToFlash(const char* toStore, int startAddr)
{
  int i = 0;
  for (; i < LENGTH(toStore); i++) {
    EEPROM.write(startAddr + i, toStore[i]);
  }
  EEPROM.write(startAddr + i, '\0');
  EEPROM.commit();
}
String readStringFromFlash(int startAddr)
{
  char in[128]; // char array of size 128 for reading the stored data
  int i = 0;
  for (; i < 128; i++) {
    in[i] = EEPROM.read(startAddr + i);
  }
  return String(in);
}
void client_handle()
{
  WiFiClient client = server2.available();
  ssid = "";
  password = "";

  if (client) {
    Serial.println("New Client");

    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {

          if (currentLine.length() == 0) {

            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println(htmlConfigurationString);

            if (header.indexOf("GET /?ssid") >= 0) {

              String url = header;
              ssid = "";
              password = "";
              String p2 = "";

              url = url.substring(4);
              url = url.substring(url.indexOf('='), url.indexOf(' '));
              Serial.println("URL  :  " + url);

              ssid = url;
              ssid = ssid.substring(url.indexOf('=') + 1, url.indexOf('&'));
              Serial.println("ssid  :  " + ssid);
              p2 = url.substring(url.indexOf('&') + 1);

              password = p2;
              password = password.substring(p2.indexOf('=') + 1, p2.indexOf('&'));
              Serial.println("password  :  " + password);


              writeStringToFlash(ssid.c_str(), 0);
              writeStringToFlash(password.c_str(), 40);

              count = 0;
              Serial.println("before");

              Serial.println(ssid);
              Serial.println(password);

              ssid.replace("%20", " ");
              password.replace("%20", " ");
              Serial.println("after");
              Serial.println(ssid);
              Serial.println(password);
              WiFi.begin(ssid.c_str(), password.c_str());
              delay(500);
              Serial.println("Connecting");
              while (WiFi.status() != WL_CONNECTED) {
                delay(700);
                count++;
                if (count >= 10) {
                  //client.println(returnHtml(999));
                  //client.println("Failed");
                  break;
                }
              }

              if (count < 10) {
                client.println(returnHtml(888));
                //client.println("OK");
                WiFi.mode(WIFI_STA);
                exit_but = true;
              }
            }
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();

  }
}
String returnHtml(int value)
{
  return "<DOCTYPE html>"\
         "<html lang=\"en\">"\
         "<head>"\
         "<meta charset=\"UTF-8\">"\
         "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"\
         "<meta http-equiv=\"refresh\" content=\"5;/\" />"\
         "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"\
         "<title>Boards Counter</title>"\
         "<style>"\
         "* {"\
         "padding: 0;"\
         "margin: 0;"\
         "font-weight: bold;"\
         "}"\
         "body {"\
         "background-color: rgb(34, 45, 57);"\
         "}"\
         ".NavBar {"\
         "background-color: #0AD666;"\
         "padding: 20px 10px;"\
         "}"\
         ".NavBar p1 {"\
         "color: rgb(34, 45, 57);"\
         "font-size: 25px;"\
         "}"\
         "#reset {"\
         "height: 50px;"\
         "display: inline;"\
         "position: absolute;"\
         "margin-right: 10px;"\
         "right: 0px;"\
         "top: 12px;"\
         "}"\
         ".NavBar button {"\
         "background-color: rgb(34, 45, 57);"\
         "color: #0AD666;"\
         "border: 0;"\
         "border-radius: 25px;"\
         "padding: 15px;"\
         "font-size: 18px;"\
         "}"\
         ".NavBar button:hover {"\
         "background-color: gray;"\
         "}"\
         ".mainScreen {"\
         "position: absolute;"\
         "top: 50%;"\
         "left: 50%;"\
         "transform: translateX(-50%) translateY(-50%);"\
         "text-align: center;"\
         "border-radius: 25px;"\
         "padding: 10px;"\
         "}"\
         ".mainScreen p1 {"\
         "color: white;"\
         "font-size: 60px;"\
         "}"\
         ".mainScreen p {"\
         "color: white;"\
         "font-size: 30px;"\
         "}"\
         "#value {"\
         "color: #0AD666;"\
         "font-size: 100px;"\
         "}"\
         "#footer {"\
         "width: 100%;"\
         "position: fixed;"\
         "bottom: 0;"\
         "padding: 10px;"\
         "background-color: white;"\
         "color: rgb(34, 45, 57);"\
         "text-align: center;}"\
         "</style>"\
         "</head>"\
         "<body>"\
         "<div class=\"NavBar\">"\
         "<p1>Elswedey Electric</p1>"\
         "<div id=\"reset\"><a href=\"/reset\"><button class=\"button\">Reset</button></a></div>"\
         "</div>"\
         "<div class=\"mainScreen\">"\
         "<p><span id=\"value\">" + String(value) + "</span> Item</p>"\
         "<p1>Is Checked</p1>"\
         "</div>"\
         "<section id=\"footer\">"\
         "Powered By EMEIH"\
         "</section>"\
         "</body>"\
         "</html>" ;
}




