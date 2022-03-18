// Wemos Mega2560 + ESP8266 - AT firmware on ESP
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <WiFiEspAT.h>
#include <PubSubClient.h>

IPAddress local_ip(10, 1, 210, 215); // CHANGE THIS ACCORDING TO IP TABLE
IPAddress dns(0, 0, 0, 0);
IPAddress gateway(0, 0, 0, 0);
IPAddress subnet(255, 255, 255, 0);

IPAddress rpi(10, 1, 210, 10);


// MQTT connection
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//the definiens of 16bit mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV mylcd(ILI9486,40,38,39,-1,41); //model,cs,cd,wr,rd,reset

//define some colour values
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

String checkTopic = "";
String checkPayload = "";
char serialMsgTopic[10];
char serialMsgPayload[20];
String msgTopic;
String msgPayload;
String serialString;
String blankString = "                         ";

void setup() 
{
  mylcd.Init_LCD();
  mylcd.Fill_Screen(BLACK); 
  Serial.begin(115200);
  delay(50);
  Serial3.begin(115200);
  delay(50);
  Serial.flush();
  mylcd.Set_Text_Mode(0);
  mylcd.Set_Text_Size(2);
  mylcd.Set_Rotation(1);
  mylcd.Set_Text_colour(GREEN);
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Print_String("Initializing", 20, 20);
  delay(100);
  mylcd.Fill_Screen(BLACK);

  mqttClient.setServer("10.1.210.200", 1883); // Change this 10.3.210.10 - TAC
  mqttClient.setKeepAlive(15);

  WiFi.init(&Serial3);
  WiFi.config(local_ip, gateway, subnet);
      
  delay(100);
  int status = WiFi.begin("IoT", "$IoT$21hv");
  
  while (WiFi.status() != WL_CONNECTED) {
    mylcd.Set_Text_colour(BLACK);
    mylcd.Print_String(blankString, 20, 20);
    mylcd.Set_Text_colour(YELLOW);
    mylcd.Print_String("Connecting to IoT", 20, 20);
    Serial.println("Connecting to IoT");
    mylcd.Set_Text_colour(GREEN);
    delay(2000);
  }
  mylcd.Set_Text_colour(BLACK);
  mylcd.Print_String(blankString, 20, 20);
  mylcd.Set_Text_colour(YELLOW);
  mylcd.Print_String("Connected to IoT", 20, 20);
  //Serial.println("Connected to IoT");
  delay(100);
  mylcd.Set_Text_colour(BLACK);
  mylcd.Print_String(blankString, 20, 20);
  mylcd.Set_Text_colour(YELLOW);
  mylcd.Print_String("Connecting to MQTT", 20, 20);
  //Serial.println("Connecting to MQTT");

  if(!mqttClient.connect("Melco1")) { // CHANGE THIS
    mylcd.Set_Text_colour(BLACK);
    mylcd.Print_String(blankString, 20, 20);
    mylcd.Set_Text_colour(RED);
    mylcd.Print_String("MQTT Connection Failed!", 20, 20);
    //mylcd.Print_String(mqttClient.state(), 20, 40);
    //Serial.print("MQTT Connection Failed! Error Code: ");
    //Serial.println(mqttClient.state());
  }
    
  mylcd.Set_Text_colour(BLACK);
  mylcd.Print_String(blankString, 20, 20);
  mylcd.Set_Text_colour(YELLOW);
  mylcd.Print_String("Connected to MQTT", 20, 20);
  //Serial.println("Connected to MQTT");
  delay(500);
  mylcd.Fill_Screen(BLACK);  
  
}


void reconnect()
{
  int wifi_status=WL_IDLE_STATUS;
  WiFi.disconnect();
  while(wifi_status != WL_CONNECTED) {
  wifi_status = WiFi.begin("IoT", "$IoT$21hv");
    mylcd.Set_Text_colour(BLACK);
    mylcd.Print_String(blankString, 20, 20);
    mylcd.Set_Text_colour(YELLOW);
    mylcd.Print_String("Connecting to IoT", 20, 20);
    mylcd.Set_Text_colour(GREEN);
    delay(1000);
  }
  while (!mqttClient.connected()) {
    mylcd.Set_Text_colour(BLACK);
    mylcd.Print_String(blankString, 20, 20);
    mylcd.Set_Text_colour(YELLOW);
    mylcd.Print_String("Connecting to RasPiTAC", 20, 20);
    if(!mqttClient.connect("Melco1")) { // CHANGE THIS
      mylcd.Set_Text_colour(BLACK);
      mylcd.Print_String(blankString, 20, 20);
      mylcd.Set_Text_colour(RED);
      mylcd.Print_String("MQTT Connection Failed!", 20, 20);
      //mylcd.Print_String(mqttClient.state(), 20, 40);
    while (1);
    }
  mylcd.Set_Text_colour(BLACK);
  mylcd.Print_String(blankString, 20, 20);
  mylcd.Set_Text_colour(YELLOW);
  mylcd.Print_String("Connected to MQTT", 20, 20);
  delay(500);
  mylcd.Fill_Screen(BLACK); 
      // Wait 5 seconds before retrying
      delay( 5000 );

  }
}

void loop()
{

  mqttClient.loop();
  if ( !mqttClient.connected() ) {
    mqttClient.disconnect();
    reconnect();
  }

char inByte[20];
  //if(Serial.available()) {
  int i = 0;
    while(Serial.available()>0)
    {
      inByte[i] = Serial.read();
      delay(1);
      i++;
    }
    inByte[i] = '\0'; // terminate the string
    mylcd.Print_String("Data received", 20, 20); //move to LCD
    
    serialString = String(inByte);
    mylcd.Set_Text_colour(BLUE);
    mylcd.Print_String("received:", 20, 40);
    mylcd.Print_String(serialString, 160, 40);
    
    delay(50);
    if(serialString == "open") {
      Serial.print("ready");
      mylcd.Set_Text_colour(GREEN);
      mylcd.Print_String("ready", 20, 60);
    }
    
    if (inByte[0] == '#')
    {
      mylcd.Set_Text_colour(BLACK);
      mylcd.Print_String(blankString, 20, 80);
      mylcd.Set_Text_colour(GREEN);
      int k = 0;
      for(int j = 1; j <= i; j++) {
        serialMsgTopic[k] = inByte[j]; // xfer serial to char array
        k++;
      }
      msgTopic = String(serialMsgTopic); // generate mqtt topic
      checkTopic = msgTopic; // store new topic for check
      mylcd.Print_String(serialMsgTopic, 20, 80);
      delay(50);
      Serial.println("payload");
    } else if (msgTopic == checkTopic && inByte[0] == '!') {
      
      mylcd.Set_Text_colour(BLACK);
      mylcd.Print_String(blankString, 20, 100);
      mylcd.Set_Text_colour(GREEN);
      
      int n = 0;
      for(int m = 1; m <= i; m++) {
        serialMsgPayload[n] = inByte[m]; // read stitch count into mqtt payload
        n++;
      }
      msgPayload = String(serialMsgPayload); // generate mqtt payload
      mylcd.Print_String(serialMsgPayload, 20, 100);
      mqttClient.publish(serialMsgTopic, serialMsgPayload); // send mqtt message
      delay(100);
      Serial.println("next");      
      //delay(4500);
      checkTopic = ""; //erase checkTopic
    }
  //}
}
