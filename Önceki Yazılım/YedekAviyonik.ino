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
#define Tilt_Sensor 12

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

uint8_t relay1_cnt=0;
bool relay1_en=0;
uint8_t relay2_cnt=0;
bool relay2_en=0;

uint8_t buzzer_count=0;


bool mpu_status=false;
bool bmp_status=false;
float mov_alt_values[alt_mov_size];

uint8_t System_Status=0;

int16_t max_speed=0;
int16_t max_altitude=0;

bool rf_Request=false;

bool tilt_sensor=false;
uint8_t tilt_counter=0;

int launch_state=0;

void Sensor_Init_Sound(void);
int32_t get_apg_mov_values(int16_t new_value);
float get_alt_mov_values(float new_value);

void setup() {
    pinMode(Blink_Led, OUTPUT); 
    pinMode(Status_Led,OUTPUT);
    pinMode(Buzzer_Pin, OUTPUT);
    pinMode(Relay1_Pin, OUTPUT);
    pinMode(Relay2_Pin, OUTPUT);
    pinMode(Tilt_Sensor, INPUT);
    digitalWrite(Relay1_Pin,LOW);
    digitalWrite(Relay2_Pin,LOW);
    digitalWrite(Buzzer_Pin,HIGH);
    
    Serial.begin(9600);

    delay(500);

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
    Timer3.attachInterrupt(Timer_Interrupt);
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
          }
        break;
        case 1:  //Bu aşamada Apogee noktası tespit edilecek
          if(bmp_altitude<old_bmp_altitude)
          {
             apg_cnt++;
             if(apg_cnt>=10)
             {
                apg_cnt=10;
                if(tilt_sensor==true)
                {
                  apg_cnt=0;
                  launch_state=2; 
                  digitalWrite(Relay2_Pin,HIGH);   //Ayrılmayı yaptık. 
                  relay2_cnt=0;
                  relay2_en=true;
                }
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
                 relay1_en=true;
                 relay1_cnt=0;
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
     
     if(relay2_en)
     {
        relay2_cnt++;
        if(relay2_cnt>50)
        {
           digitalWrite(Relay2_Pin,LOW);
           relay2_en=false;
           relay2_cnt=0;  
        } 
     }
     
/*     Serial.print('*');//Başlangıç Karakteri
     Serial.print(bmp_alt);//Basınç Sensörü yükseklik verisi
     Serial.print('|'); 
     Serial.print(local_altitude);//Local yükseklik verisi
     Serial.print('|'); 
     Serial.print(bmp_altitude); //Basınç Sensörü yükseklik verisi
     Serial.print('|'); 
     Serial.print(max_altitude); //Max yükseklik verisi
     Serial.print('|'); 
     Serial.print(bmp_speed);   //hız bilgisi
     Serial.print('|'); 
     Serial.print(max_speed);   //Max hız bilgisi
     Serial.print('|'); 
     Serial.print(tilt_sensor); //tilt_sensor
     Serial.print('|'); 
     Serial.print(launch_state); //tilt_sensor
     Serial.println('#');    
             */
     sensor_ready=0;
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

  if(digitalRead(Tilt_Sensor))
  {
     tilt_counter++;
     if(tilt_counter>=5) 
     {
        tilt_counter=5;
        tilt_sensor=true; 
     } 
  }
  else
  {
     tilt_counter=0;
     tilt_sensor=0;  
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
