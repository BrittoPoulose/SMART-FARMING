#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "iot-irrigation-b3751-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "N4CxlLVFXlrsJRVxkSiB7gR3V2xFmTUWUnhiFAis"
#define WIFI_SSID "Connectify-me"
#define WIFI_PASSWORD "embedded54321"

void wifi_connect();

String msg="";
String msg1="";

void setup() 
{
 Serial.begin(9600);
 pinMode(16,OUTPUT);
 pinMode(16,HIGH);
 wifi_connect();
}

void loop() 
{
  msg = Firebase.getString("msg");
  Serial.print(msg);
  if(msg =="0")
  {
    Serial.println("Automode");
    msg1 = Firebase.getString("soil");
    if(msg1=="LOW")
    {
      Serial.println("Auto MODE ON");
      pinMode(16,LOW);
    }
    else if(msg1=="HIGH")
    {
      Serial.println("Auto MODE OFF");
      pinMode(16,HIGH);
    }
    delay(1000);
  }
  else if(msg =="1")
  {
    Serial.println("MANUAL MODE ON");
    pinMode(16,LOW);
  }
  else if(msg =="2")
  {
    Serial.println("MANUAL MODE OFF");
    pinMode(16,HIGH);  
  }
  delay(1000);  
}

void wifi_connect()
{
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
