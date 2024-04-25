#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#define turn_1 110
#define turn_2 550
#define move_f1 1500

const char* ssid     = "Justin";
const char* password = "pomelo is me";

int I1 = 17;
int I2 = 5;
int I3 = 22;
int I4 = 23;
int M1 = 13;
int M2 = 12;
int M3 = 14;
int M4 = 32;

uint8_t broadcastAddress[] = {0xCC,0xDB,0xA7,0x47,0x09,0x9C};  //改mac
typedef struct struct_message { //發送資料的結構
  int data;
} struct_message;
struct_message myData;

// 資料發送回調函式
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println();
}

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

WiFiServer server(8888);


void WiFiReset() {
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

//兩個電池盒(的那一邊)在右
//move1,2(HIGH,LOW,HIGH,LOW)前進
//move_1(LOW,HIGH,LOW,HIGH)後退
//move1(HIGH,LOW,HIGH,LOW)前方馬達
//move2(HIGH,LOW,HIGH,LOW)後方馬達
//move1(HIGH(左,27),LOW(左,27),HIGH(右,26),LOW)(右,26))前方馬達
//move2(HIGH(左),LOW(左),HIGH(右),LOW)(右))後方馬達
void move_1(int a,int b, int c, int d){
  digitalWrite(M1,a);
  digitalWrite(M2,b);
  digitalWrite(M3,c);
  digitalWrite(M4,d);
}
void move_2(int a,int b, int c, int d){
  digitalWrite(I1,a);
  digitalWrite(I2,b);
  digitalWrite(I3,c);
  digitalWrite(I4,d);
}

void m_front(){
  move_1(HIGH,LOW,HIGH,LOW);
  move_2(HIGH,LOW,HIGH,LOW);
}

void m_back(){
  move_1(LOW,HIGH,LOW,HIGH);
  move_2(LOW,HIGH,LOW,HIGH);
}

void m_spin(){
  move_1(HIGH,LOW,LOW,HIGH);
  move_2(HIGH,LOW,LOW,HIGH);
}

void m_stop(){
  move_1(LOW,LOW,LOW,LOW);
  move_2(LOW,LOW,LOW,LOW);
}

void setup()
{
    Serial.begin(115200);
    pinMode(I1,OUTPUT);
    pinMode(I2,OUTPUT);
    pinMode(I3,OUTPUT);
    pinMode(I4,OUTPUT);
    pinMode(M1,OUTPUT);
    pinMode(M2,OUTPUT);
    pinMode(M3,OUTPUT);
    pinMode(M4,OUTPUT);
    
    move_1(LOW,LOW,LOW,LOW);
    move_2(LOW,LOW,LOW,LOW);

    // 初始化 ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      return;
    } 

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
   
    server.begin();
}

char c;

void loop(){
  m_stop();
  WiFiClient client = server.available();   // Listen for incoming clients
  myData.data = 0;
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        c = client.read();                  // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            Serial.println("auto test mode");
            Serial.println("");
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            client.println("");
            
            client.println("<body><h1>delivery system</h1>");
            
            client.println("<p> auto test </p>");      
            client.println("<p><a href=/con/start><button class=\"button\">start</button></a></p>"); 

            client.println("</body></html>");
            
            if (header.indexOf("GET /con/start") >= 0) {//測試開始
              m_stop();
              m_front();
              delay(3350);
              m_stop();
              
              WiFi.disconnect();
              //
              esp_now_register_send_cb(OnDataSent);
              esp_now_peer_info_t peerInfo;
              memcpy(peerInfo.peer_addr, broadcastAddress, 6);
              peerInfo.channel = 6;
              peerInfo.encrypt = false;


              // 檢查裝置是否配對成功
              if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                Serial.println("Failed to add peer");
                return;
              }

              delay(50);
              myData.data = 1;
              esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
              
              // 檢查數據是否發送成功
              if (result == ESP_OK) {
                Serial.println("Sent with success");
              }
              else {
                Serial.println("Error sending the data");
              }
              
              delay(4000);
              m_front();
              delay(3000);
              m_stop();
              delay(1500);
              m_back();
              delay(4350);
              WiFi.reconnect();
            }

            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
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

  


