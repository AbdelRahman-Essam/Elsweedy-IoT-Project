#include <WiFi.h>
#include <WiFiClient.h>
#include "EEPROM.h"
#include <FirebaseESP32.h>

int val = 50 ;
String header;
int Error;
int Error_prev;
int ResetFlag;

#define LENGTH(x) (strlen(x) + 1)   // length of char string
#define EEPROM_SIZE 200             // EEPROM size
#define WiFi_rst 0                  //WiFi credential reset pin (Boot button on ESP32)
#define LED 2

#define FIREBASE_HOST "proja-8b55a-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "wYecROzY1L5T4bJ85BjQdpuWD2He7VmVlthluQ6H"

String ssid;
String password;
unsigned long rst_millis;

WiFiServer server(80);
FirebaseData firebaseData;

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

void setup(void)
{ 
  Setup();      
  EEPROM_Setup();
  connectWifi();
  Firebase_Setup();
} 

void loop(void)
{
  FirbaseStatments();
  handleSeverClient();
  CheckSmartConfig();
  firebaseErrorDetect();
}


void resetCounter()
{
  Serial.println("Reset Button Clicked");
  val ++ ;
  Serial.println("new value posted");

}
void handleSeverClient()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {  // loop while the client's connected
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
            client.println(returnHtml(val));
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
void connectWifi()
{
  // Configures static IP address
  if(readStringFromFlash(100)=="1")
  {
    SmartConfig();
  }
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("");
  delay(5000);
  // Wait for connection
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
}
String returnHtml(int value)
{
  return "<DOCTYPE html>"\
         "<html lang=\"en\">"\
         "<head>"\
         "<meta charset=\"UTF-8\">"\
         "<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"\
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
void EEPROM_Setup(void)
{
  if (!EEPROM.begin(EEPROM_SIZE)) { //Init EEPROM
    Serial.println("failed to init EEPROM");
    delay(1000);
  }
  else
  {
    ssid = readStringFromFlash(0); // Read SSID stored at address 0
    Serial.print("SSID = ");
    Serial.println(ssid);
    password = readStringFromFlash(40); // Read Password stored at address 40
    Serial.print("passwords = ");
    Serial.println(password);
  }
}
void Firebase_Setup(void)
{
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  
  Serial.println("------------------------------------");
  Serial.println("Firebase Connected...");
  Firebase.get(firebaseData, "/WhichHeater");
  Serial.print(firebaseData.stringData());
}
void Setup(void)
{
  Serial.begin(115200);
  pinMode(WiFi_rst, INPUT);
  pinMode(LED, OUTPUT);
}
void SmartConfig(void)
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("SmartConfig received.");

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected.");
  digitalWrite(LED,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // read the connected WiFi SSID and password
  ssid = WiFi.SSID();
  password = WiFi.psk();
  Serial.print("SSID:");
  Serial.println(ssid);
  Serial.print("password:");
  Serial.println(password);
  Serial.println("Store SSID & password in Flash");
  writeStringToFlash(ssid.c_str(), 0); // storing ssid at address 0
  writeStringToFlash(password.c_str(), 40); // storing password at address 40
  writeStringToFlash("",100);
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
void FirbaseStatments(void)
{
  Firebase.setFloat(firebaseData, "/Chickens/Temperature", random(10,20));
}
void CheckSmartConfig(void)
{
  if (digitalRead(WiFi_rst) == LOW)
  {
    delay(3000);
    if (digitalRead(WiFi_rst) == LOW)
    {
      Serial.println("Reseting the WiFi credentials");
      writeStringToFlash("", 0); // Reset the SSID
      writeStringToFlash("", 40); // Reset the Password
      Serial.println("Wifi credentials erased");
      Serial.println("Restarting ESP...\n\n\n");
      writeStringToFlash("1", 100);
      ESP.restart();
    }
  }
}
int firebaseErrorDetect(void)
{
  Error = firebaseData.httpCode();
  Serial.print("HTTPC_ERROR_NOT_CONNECTED : "); Serial.println(Error);
  if ((Error != Error_prev) && (Error != 200))  ResetFlag = 1;
  return (Error);
}
