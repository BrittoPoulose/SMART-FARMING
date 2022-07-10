//#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
LiquidCrystal_I2C lcd(0x27,16,2);

/***************************/
    #define DHTPIN 14 
    #define DHTTYPE DHT11
    #define FIREBASE_HOST "iot-irrigation-b3751-default-rtdb.firebaseio.com"
    #define FIREBASE_AUTH "N4CxlLVFXlrsJRVxkSiB7gR3V2xFmTUWUnhiFAis"
    #define WIFI_SSID "Connectify-me"
    #define WIFI_PASSWORD "embedded54321"
    DHT dht(DHTPIN, DHTTYPE);
    SoftwareSerial pmsSerial(0,2); // Red ->D4 orange ->D3
/***************************/
    void wifi_connect();
    void read_temp_hum();
    void timer();
    void pm5003();
    void channel_1_ADC_read();
    void channel_2_ADC_read();
/***************************/
    int temp=0,temp_p=1,hum=0,hum_p=1;
    int pt,ot,dt=0;
    int PM_2_5,PM_10;
    int s0 = 16;  // D0
    int s1 = 12;  // D6
    int s2 = 13;  // D7
    int soil=0,soil_p=1;

    String PhVal="Neutral",PhVal_p="";
    unsigned long int avgValue;  //Store the average value of the sensor feedback
    float b;
    int buf[10],temp1;

    String PM2_5_Dat=" ",PM_10_Dat=" ";
    String PM2_5_Dat_p=".",PM_10_Dat_p=".";
    String SM="",SM_p="1";
/***************************/
    struct pms5003data 
    {
      uint16_t framelen;
      uint16_t pm10_standard, pm25_standard, pm100_standard;
      uint16_t pm10_env, pm25_env, pm100_env;
      uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
      uint16_t unused;
      uint16_t checksum;
    }; 
    struct pms5003data data;
/********************************/  
void setup() 
{
  Serial.begin(9600);
  pmsSerial.begin(9600);
  dht.begin();
  delay(100);
  Serial.println("\n\n\nAutomation");
  lcd.begin();
  lcd.print("Green House");
  lcd.setCursor(0, 1);
  lcd.print("Automation");
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT);
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  wifi_connect();

  pt=ot=millis()/100;
}
/***************************/
void loop() 
{  
  timer();
  delay(100);
}
/***************************/
    void read_temp_hum()
     {
       temp = dht.readTemperature();
       hum  = dht.readHumidity();
       if (isnan(temp) || isnan(hum)) 
       {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      /*Serial.print(F("\nHumidity: "));
      Serial.print(hum);
      Serial.print(F("%  Temperature: "));
      Serial.print(temp);
      Serial.print(F("°C "));
      Serial.println();*/
      
     }
/***************************/
    void timer()
    {
      pt=millis()/100;
      if(pt-ot>10)
      {
        dt++;
        ot=pt;
        pm5003();
        channel_1_ADC_read();
        channel_2_ADC_read();
        read_temp_hum(); 
        if(dt==1)
        {  
            read_temp_hum();          
            lcd.begin();
            lcd.print("Temp :");
            lcd.print(temp);
            lcd.setCursor(0, 1);
            lcd.print("Hum  :");
            lcd.print(hum);
        }
        else if(dt==2)
        {
          lcd.begin();
            lcd.print("2.5 :");
            lcd.print(PM_2_5);
                     if(data.pm25_env < 25)
                      {
                        lcd.print(" Good");
                        PM2_5_Dat = " Good" ;
                      }
                      else if(data.pm25_env < 60 && data.pm25_env > 26)
                      {
                        lcd.print(" Moderate");//30
                        PM2_5_Dat = " Moderate" ;
                      }
                      else if(data.pm25_env < 124 && data.pm25_env > 60)
                      {
                        lcd.print(" Inadequate");//30
                        PM2_5_Dat = " Inadequate" ;
                      }
                      else if(data.pm25_env < 209 && data.pm25_env > 125)
                      {
                        lcd.print(" Bad");//30
                        PM2_5_Dat = "Bad" ;
                      }
                      else if(data.pm25_env < 249 && data.pm25_env > 210)
                      {
                        lcd.print(" Terrible");//30
                        PM2_5_Dat = "Terrible" ;
                      }
                      else if(data.pm25_env >250)
                      {
                        lcd.print(" Critical");//30
                        PM2_5_Dat = "Critical" ;
                        data.pm25_env = 280 ;
                      }
            lcd.setCursor(0, 1);
            lcd.print("10  :");
            lcd.print(PM_10);
                    if(data.pm100_env < 50)
                    {
                      lcd.print(" Good");//30
                      PM_10_Dat = "Good" ;
                    }
                    else if(data.pm100_env < 120 && data.pm100_env > 51)
                    {
                      lcd.print(" Moderate");//30
                      PM_10_Dat = "Moderate" ;
                    }
                    else if(data.pm100_env < 249 && data.pm100_env > 120)
                    {
                      lcd.print(" Inadequate");//30
                      PM_10_Dat = "Inadequate" ;
                    }
                    else if(data.pm100_env < 419 && data.pm100_env > 250)
                    {
                      lcd.print(" Bad");//30
                      PM_10_Dat = "Bad" ;
                    }
                    else if(data.pm100_env < 499 && data.pm100_env > 420)
                    {
                      lcd.print(" errible");//30
                      PM_10_Dat = "Terrible" ;
                    }
                    else if(data.pm100_env > 500)
                    {
                      lcd.print(" Critical");//30
                      PM_10_Dat = "Critical" ;
                      data.pm100_env =550;
                    }
            
        }
        else if(dt==3)
        {
          lcd.begin();
          lcd.print("Soil:");
          lcd.print(soil);
          lcd.setCursor(0, 1);
          lcd.print("PH  :");
          lcd.print(PhVal);
        }
        else if(dt==4)
        {          
          if(temp != temp_p)
          {
            lcd.begin();
            lcd.print("Uploading");
            lcd.setCursor(0, 1);
            lcd.print("Temprature");
            Firebase.setFloat("temp", temp); 
              if (Firebase.failed()) 
              {
                lcd.print("  Fail");
                  Serial.print("setting /temp failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
              lcd.print("  [ok]");
              temp_p=temp;
              delay(500);
          }
          
          if(hum != hum_p)
          {
            lcd.begin();
            lcd.print("Uploading");
            lcd.setCursor(0, 1);
            lcd.print("Humidity");
              Firebase.setFloat("hum", hum); 
              if (Firebase.failed()) 
              {
                lcd.print("  Fail");
                  Serial.print("setting /hum failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
              lcd.print("  [ok]");
              hum_p=hum;
              delay(500);
          }
          
          if(PhVal != PhVal_p)
          {
            lcd.begin();
            lcd.print("Uploading");
            lcd.setCursor(0, 1);
            lcd.print("PhVal");
              Firebase.setString("PH", PhVal); 
              if (Firebase.failed()) 
              {
                lcd.print("  Fail");
                  Serial.print("setting /hum failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
              lcd.print("  [ok]");
              PhVal_p = PhVal;
              delay(500);
          }
          
          if(SM != SM_p)
          {
            
            lcd.begin();
            lcd.print("Uploading");
            lcd.print(SM);
            lcd.setCursor(0, 1);
            lcd.print("S-Moist");
              Firebase.setString("soil", SM); 
              if (Firebase.failed()) 
              {
                lcd.print("  Fail");
                  Serial.print("setting /hum failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
              lcd.print("  [ok]");
              SM_p=SM;
              delay(500);
          }
          
          if(PM2_5_Dat != PM2_5_Dat_p )
          {
            
            lcd.begin();
            lcd.print("Uploading");
            lcd.setCursor(0, 1);
            lcd.print("PMS2.5");
              Firebase.setString("PMS2_5", PM2_5_Dat); 
              if (Firebase.failed()) 
              {
                lcd.print("  Fail");
                  Serial.print("setting /pms2.2 failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
              lcd.print("  [ok]");
              PM2_5_Dat_p = PM2_5_Dat;
              delay(500);
          }
          
          if(PM_10_Dat != PM_10_Dat_p)
          {
            lcd.begin();
            lcd.print("Uploading");
            lcd.setCursor(0, 1);
              lcd.print("PMS10");
              Firebase.setString("PMS10", PM_10_Dat); 
              if (Firebase.failed()) 
              {
                lcd.print("  Fail");
                  Serial.print("setting /pms10 failed:");
                  Serial.println(Firebase.error());  
                  return;
              }
              lcd.print("  [ok]");
              PM_10_Dat_p = PM_10_Dat;
              delay(500);
          }
          
          
          
          dt=0;
        }
      }
    }
/***************************/

boolean readPMSdata(Stream *s)
{
  if (! s->available()) 
  {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) 
  {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) 
  {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) 
  {
    sum += buffer[i];
  }
 
  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}
/********************************/
    void pm5003()
     {
        if (readPMSdata(&pmsSerial))
      {
        // reading data was successful!
        //Serial.println();
        /*Serial.println("---------------------------------------");
        Serial.println("Concentration Units (standard)");
        Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
        Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
        Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);*/
       /* Serial.println("---------------------------------------");
        Serial.println("Concentration Units (environmental)");
        Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
        Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
        Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
        Serial.println("---------------------------------------");*/
        PM_2_5 = data.pm25_env ;
        PM_10 = data.pm100_env;
       /* Serial.print("Particles > 0.3um / 0.1L air:"); Serial.println(data.particles_03um);
        Serial.print("Particles > 0.5um / 0.1L air:"); Serial.println(data.particles_05um);
        Serial.print("Particles > 1.0um / 0.1L air:"); Serial.println(data.particles_10um);
        Serial.print("Particles > 2.5um / 0.1L air:"); Serial.println(data.particles_25um);
        Serial.print("Particles > 5.0um / 0.1L air:"); Serial.println(data.particles_50um);
        Serial.print("Particles > 10.0 um / 0.1L air:"); Serial.println(data.particles_100um);
        Serial.println("---------------------------------------");*/
      }
     }
/***************************/
void channel_1_ADC_read()
{
   int adc;      
        digitalWrite(s0, LOW);
        digitalWrite(s1, LOW);
        digitalWrite(s2, LOW);
        delay(20);
        adc = analogRead(A0);
        soil = map(adc, 1023, 530, 0, 100);
        if(soil>100)
          soil=100;
        else if(soil<0)
          soil=0;

        if(soil < 30)
        {
          SM = "LOW";
        }
        else if(soil>=30 && soil <= 60)
        {
          SM ="Medium"; 
        }
        else
        {
          SM = "High";
        }
        
         /*Serial.println("\nChannel-1  selected"); 
         Serial.print("Sensor 1 : ");
         Serial.println(soil);*/
}
/***********************/
void channel_2_ADC_read()
{
  int adc;
        digitalWrite(s0, HIGH);
        digitalWrite(s1, LOW);
        digitalWrite(s2, LOW);
        delay(20);
        adc = analogRead(A0);
        for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
        { 
          buf[i]=analogRead(A0);
          delay(10);
        }
        for(int i=0;i<9;i++)        //sort the analog from small to large
        {
          for(int j=i+1;j<10;j++)
          {
            if(buf[i]>buf[j])
            {
              temp1=buf[i];
              buf[i]=buf[j];
              buf[j]=temp1;
            }
          }
        }
        avgValue=0;
        for(int i=2;i<8;i++)                      //take the average value of 6 center sample
          avgValue+=buf[i];
        float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
        phValue=3.5*phValue;  //convert the millivolt into pH value
        
         Serial.println("\nChannel-2 selected"); 
         Serial.print("Sensor 2 : ");
         Serial.print (phValue);

         if(phValue>15)
         {
          Serial.println("   Base");
          PhVal="Base";
         }
         else if(phValue<6)
         {
          Serial.println("   Acid");
          PhVal="Acid";
         }
         else
         {
           Serial.println("   Neutral");
           PhVal="Neutral";
         }
}

/***********************/
void wifi_connect()
{
  int i=0;
          lcd.begin();
          lcd.print("Wifi Connecting");
          lcd.setCursor(0, 1);
          delay(500);
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) 
    {
      i++;
      if(i>15)
      {
          lcd.begin();
          lcd.print("Wifi Connecting");
          lcd.setCursor(0, 1);
          i=0;
      }
      Serial.print(".");
      delay(500);
      lcd.print("*");
    }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());
    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
          lcd.clear();
          lcd.print("Wifi Connected");
          lcd.setCursor(0, 1);
          delay(1000);
}
