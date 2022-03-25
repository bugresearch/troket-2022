#include "Adafruit_BMP280.h"
#include <TimerThree.h>
#include <MPU9250.h>
#include "string.h"

MPU9250 mpu;
Adafruit_BMP280 bmp; 

#define DEBUG_MODE
#define alt_mov_size 10

#define Buzzer_Pin 6
#define Relay1_Pin 5
#define Relay2_Pin 4
#define Status_Led 21
#define Blink_Led  13

float mpu_roll=0;
float mpu_yaw=0;
float mpu_pitch=0;
float bmp_alt=0;
float old_alt=0;
int16_t bmp_altitude=0;
int16_t old_bmp_altitude=0;
int16_t bmp_speed=0;
float local_altitude=0;
uint8_t apg_cnt=0;
bool sensor_ready=false;

char gps_string[100];
char parse_params[20][20];
int parca=0;
int gps_count=0;
bool gps_state=false;
bool gps_ready=false;

bool mpu_status=false;
bool bmp_status=false;
float mov_alt_values[alt_mov_size];

uint8_t System_Status=0;

uint8_t buzzer_count=0;

int16_t max_speed=0;
int16_t max_altitude=0;

float latitude_val=0;
float longitude_val=0;
uint8_t gps_status=0;
uint8_t nr_of_Satellites=0;
float gps_accuracy=0;
bool rf_Request=false;

int launch_state=0;

uint8_t relay1_cnt=0;
bool relay1_en=0;
uint8_t relay2_cnt=0;

void Sensor_Init_Sound(void);
int32_t get_apg_mov_values(int16_t new_value);
float get_alt_mov_values(float new_value);
int gps_parse_text(char *gps_text, char parse_text[][10]);

void setup() {
    pinMode(Blink_Led, OUTPUT); 
    pinMode(Status_Led, OUTPUT);
    pinMode(Buzzer_Pin, OUTPUT);
    pinMode(Relay1_Pin, OUTPUT);
    pinMode(Relay2_Pin, OUTPUT);
    digitalWrite(Relay1_Pin,LOW);
    digitalWrite(Relay2_Pin,LOW);
    digitalWrite(Buzzer_Pin,HIGH);
    
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial3.begin(9600);    

    #ifdef DEBUG_MODE
       Serial.println("Startup...");
    #endif
    Wire.begin();
    delay(1000);
    digitalWrite(Buzzer_Pin,LOW);    
    delay(100);
        
    if(mpu.setup()==true)
    {
        mpu_status=true;
        bitWrite(System_Status,0,1); //System_Status=xxxxxxx1
        Sensor_Init_Sound();
        #ifdef DEBUG_MODE 
           Serial.println("MPU9250 Init OK");
        #endif
    } 
    else
    {
        bitWrite(System_Status,1,1); //System_Status=xxxxxxx0
        mpu_status=false;
        #ifdef DEBUG_MODE 
           Serial.println("MPU9250 Init ERROR!!");
        #endif
    }
    delay(100);
    if(bmp.begin()==true)
    {
        bitWrite(System_Status,1,1); //System_Status=xxxxxx1x
        bmp_status=true;
        for(int i=0;i<100;i++)
        {
           bmp_alt = get_alt_mov_values(bmp.readAltitude(1013.25)); 
           delay(10); 
        }
        local_altitude=bmp_alt;
         old_alt=bmp_alt;
         
        Sensor_Init_Sound();
        #ifdef DEBUG_MODE 
           Serial.println("BMP280 Init OK");
           Serial.print("Local Altitude:");
           Serial.println(local_altitude);
        #endif
    } 
    else
    {
        bitWrite(System_Status,1,1); //System_Status=xxxxxx0x
        bmp_status=false;
        #ifdef DEBUG_MODE 
           Serial.println("BMP280 Init ERROR!!");
        #endif      
    }

    Timer3.initialize(100000);
    Timer3.attachInterrupt(Timer_Interrupt); // blinkLED to run every 0.15 seconds
}


void loop() {

  if(sensor_ready==true)
  {
     switch(launch_state)
     {
        case 0: //Bu kısımda launch varmı yokmu ona bakacağız 
          if(bmp_altitude>10)
          {
             launch_state=1;//İkinci aşamaya geçiyoruz  
             apg_cnt=0;
          }
        break;
        case 1:  //Bu aşamada Apogee noktası tespit edilecek
          if(bmp_altitude<old_bmp_altitude)
          {
             apg_cnt++;
             if(apg_cnt>30)
             {
                apg_cnt=0;
                launch_state=2; 
             }  
             old_bmp_altitude=bmp_altitude;
          }
          else
          { 
             old_bmp_altitude=bmp_altitude;  
          }
          
        break;
        case 2:
           apg_cnt++;

           if(apg_cnt>=100)
           {
              apg_cnt=100;
              if(bmp_altitude<=600)
              {
                 digitalWrite(Relay1_Pin,HIGH); 
                 relay1_cnt=0;
                 relay1_en=true;
                 launch_state=3;
              }
           }
        break; 
        case 3:
        break;
     }

     if(relay1_en)
     {
        relay1_cnt++;
        if(relay1_cnt>50)
        {
           digitalWrite(Relay1_Pin,LOW);
           relay1_en=false;
           relay1_cnt=0;  
        } 
     }
     
     
     #ifdef DEBUG_MODE
/*         Serial1.print('*');//Başlangıç Karakteri
         Serial1.print(latitude_val,4);//Latitude
         Serial1.print('|');    
         Serial1.print(longitude_val,4);//Longitude
         Serial1.print('|');    
         Serial1.print(gps_status);//GPS Status
         Serial1.print('|'); 
         Serial1.print(nr_of_Satellites);//Number of Satellites   
         Serial1.print('|'); 
         Serial1.print(gps_accuracy);//Relative accuracy of horizontal position
         Serial1.print('|'); 
         Serial1.print(bmp_alt);//Basınç Sensörü yükseklik verisi
         Serial1.print('|'); 
         Serial1.print(local_altitude);//Local yükseklik verisi
         Serial1.print('|'); 
         Serial1.print(bmp_altitude);//Basınç Sensörü yükseklik verisi
         Serial1.print('|'); 
         Serial1.print(max_altitude);   //Max yükseklik verisi
         Serial1.print('|'); 
         Serial1.print(bmp_speed);   //hız bilgisi
         Serial1.print('|'); 
         Serial1.print(max_speed);   //Max hız bilgisi
         Serial1.print('|'); 
         Serial1.print(launch_state);//hız bilgisi
         Serial1.println('#');          */
    
         Serial.print('*');//Başlangıç Karakteri
         Serial.print(latitude_val,4);//Latitude
         Serial.print('|');    
         Serial.print(longitude_val,4);//Longitude
         Serial.print('|');    
         Serial.print(gps_status);//GPS Status
         Serial.print('|'); 
         Serial.print(nr_of_Satellites);//Number of Satellites   
         Serial.print('|'); 
         Serial.print(gps_accuracy);//Relative accuracy of horizontal position
         Serial.print('|'); 
         Serial.print(bmp_alt);//Basınç Sensörü yükseklik verisi
         Serial.print('|'); 
         Serial.print(local_altitude);//Local yükseklik verisi
         Serial.print('|'); 
         Serial.print(bmp_altitude);//Basınç Sensörü yükseklik verisi
         Serial.print('|'); 
         Serial.print(max_altitude);   //Max yükseklik verisi
         Serial.print('|'); 
         Serial.print(bmp_speed);   //hız bilgisi
         Serial.print('|'); 
         Serial.print(max_speed);   //Max hız bilgisi
         Serial.print('|'); 
         Serial.print(launch_state);//hız bilgisi
         Serial.println('#');            
     #endif
     
     sensor_ready=false;
  }
   


  
 if(rf_Request==true)
 {
     Serial1.print('*');//Başlangıç Karakteri
     Serial1.print(latitude_val,4);//Latitude
     Serial1.print('|');    
     Serial1.print(longitude_val,4);//Longitude
     Serial1.print('|');    
     Serial1.print(gps_status);//GPS Status
     Serial1.print('|'); 
     Serial1.print(nr_of_Satellites);//Number of Satellites   
     Serial1.print('|'); 
     Serial1.print(gps_accuracy);//Relative accuracy of horizontal position
     Serial1.print('|'); 
     Serial1.print(bmp_alt);//Basınç Sensörü yükseklik verisi
     Serial1.print('|'); 
     Serial1.print(local_altitude);//Local yükseklik verisi
     Serial1.print('|'); 
     Serial1.print(bmp_altitude);//Basınç Sensörü yükseklik verisi
     Serial1.print('|'); 
     Serial1.print(max_altitude);   //Max yükseklik verisi
     Serial1.print('|'); 
     Serial1.print(bmp_speed);   //hız bilgisi
     Serial1.print('|'); 
     Serial1.print(max_speed);   //Max hız bilgisi
     Serial1.print('|'); 
     Serial1.print(launch_state);//hız bilgisi
     Serial1.println('#');     
     rf_Request=false;
 }
 
 if(gps_ready==true)
 {    
    gps_parse_text(gps_string,parse_params);
    if(strcmp(parse_params[0],"GPGGA")==0)
    {
       latitude_val=atof(parse_params[2]);//Burada Latitude verileri alınır.
       longitude_val=atof(parse_params[4]);//Burada Longitude verileri alınır
       gps_status=atoi(parse_params[6]);
       if(gps_status>0)//Burada GPS Status verisi alınıyor, 0=Not Fix, 1=GPS Fix, 2=DGPS Fix
       {
          bitWrite(System_Status,2,1);   //System_Status=xxxxx1xx
          digitalWrite(Status_Led,HIGH);
       }
       else
       {
          bitWrite(System_Status,2,0);   //System_Status=xxxxx0xx
          digitalWrite(Status_Led,LOW);
       }
       nr_of_Satellites=atoi(parse_params[7]);  //Bu Veri Bağlanılan uydu sayısı 
       gps_accuracy=atof(parse_params[8]);//gps_accurary parametresi alınır
    }
    gps_ready=false;
 }
  
}

void Sensor_Init_Sound(void)
{
   digitalWrite(Buzzer_Pin,HIGH);
   delay(50);
   digitalWrite(Buzzer_Pin,LOW);
   delay(100);
   digitalWrite(Buzzer_Pin,HIGH);
   delay(50);
   digitalWrite(Buzzer_Pin,LOW);
}


void Timer_Interrupt(void)
{
  if(mpu_status==true)
  {
      mpu.update();   //Veriler Güncellendi
      mpu_roll=mpu.getRoll();
      mpu_yaw=mpu.getYaw();
      mpu_pitch=mpu.getPitch();
  }
  if(bmp_status==true)
  {
     bmp_alt = get_alt_mov_values(bmp.readAltitude(1013.25));
     bmp_altitude = (int16_t)bmp_alt-local_altitude;
     bmp_speed = (int16_t)(bmp_alt-old_alt)*10;
     if(bmp_altitude>max_altitude)max_altitude=bmp_altitude;
     if(abs(bmp_speed)>max_speed)max_speed=abs(bmp_speed);
     old_alt=bmp_alt;
  }
  sensor_ready=true;
  digitalWrite(13,1-digitalRead(13));


  if(launch_state==3)
  {
       buzzer_count++;
       if(buzzer_count>10)
       {
         buzzer_count=0;
         digitalWrite(Buzzer_Pin,1-digitalRead(Buzzer_Pin)); 
       }
  }
}

void serialEvent1()  //RF  modülden istek geldimi
{
   char inChar=Serial1.read();
   if(inChar=='F')
   {
      rf_Request=true; 
   }
}

void serialEvent3(){  
  char inchar=Serial3.read();
  
  if(gps_state==false)
  {
    if(inchar=='$')
    {
      gps_string[0]=inchar;
      gps_state=true;  
      gps_ready=false;
      gps_count++;
    }
  }
  else
  {
     if(inchar=='\n')
     {
        gps_string[gps_count]='\0';
        gps_ready=true;
        gps_state=false;
        gps_count=0;
     }
     else
     {
       gps_string[gps_count]=inchar;
       gps_count++;
     }
  }
}
float get_alt_mov_values(float new_value)
{
   int i=0;
   float toplam=0;
   for(i=0;i<alt_mov_size-1;i++)
   {
       mov_alt_values[i]=mov_alt_values[i+1];
       toplam+=mov_alt_values[i];
   }
   mov_alt_values[alt_mov_size-1]=new_value;
   toplam+=mov_alt_values[alt_mov_size-1];
   return toplam/alt_mov_size;
}

int gps_parse_text(char *gps_text, char parse_text[][20])
{
  char *pch;  
  int indx=0;
  
  pch=strtok(gps_text,"$,\0");
  
  while (pch != NULL)
  {
    strcpy(parse_text[indx],pch);
    pch=strtok(NULL,"$,\0");
    indx++;
  }
  return indx;
}
