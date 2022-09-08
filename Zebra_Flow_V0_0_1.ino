//Zebra Flow by PacoGuy
//Adapted from Laser Tachometer by Elite Worm (YouTube.com/c/EliteWorm)
//Version 0.0.1

//Make sure you have all the libraries installed!

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte sensor = 2;

volatile unsigned long t_pulse_started_volatile = 0;
volatile unsigned long t_pulse_duration_volatile = 0;
unsigned long t_pulse_started = 0;
unsigned long t_pulse_duration = 0;

long rpm_sum = 0;
long rpm_reading[100];
long rpm_average = 0;
long lpm = 0;
byte n_max = 0;
byte n = 0;

volatile bool timeout = 1;
volatile bool newpulse = 0;

void setup() {
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }
  
  pinMode(sensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(sensor), ISR_sensor, RISING);
 
}

void loop() { 
   
   noInterrupts();
   t_pulse_started = t_pulse_started_volatile;
   t_pulse_duration = t_pulse_duration_volatile;
   interrupts();
   
   if(((micros() - t_pulse_started) > 2000000) && timeout == 0 && newpulse == 0) {
           
     timeout = 1;
     rpm_average = 0;
     lpm = 0;
     n = 0;

   };
   
   if(timeout == 0){
     
     if(newpulse){
       
       rpm_reading[n] = (60000000 / t_pulse_duration);
       n_max = constrain(map(rpm_reading[n], 60, 100000, 0, 100), 0, 100);
       n++;
       newpulse = 0;
       
       if(n > n_max){
       
         for (byte i = 0; i <= n_max; i++) {
           
           rpm_sum = rpm_sum + rpm_reading[i];
     
         };
         
         rpm_average = rpm_sum / (n_max + 1);
         lpm = rpm_average / 12; //Convert rpm so that 12 revolutions equals 1l per minute
         rpm_sum = 0;
         n = 0;
         
       }
       
     }
   
   }

updatedisplay();
  
}

void updatedisplay() {
  
  byte x = 0;
  
  display.clearDisplay();
  display.setTextSize(3);
  
  if(lpm < 10) x = 80;
  if(lpm >= 10 && lpm < 100) x = 62;
  if(lpm >= 100 && lpm < 1000) x = 44;
  if(lpm >= 1000 && lpm < 10000) x = 26;
  if(lpm >= 10000 && lpm < 100000) x = 8;
  
  display.setTextColor(SSD1306_WHITE);
  
  if(lpm < 100000){
    
    display.setCursor(x, 6); 
    display.print(lpm);
    display.setTextSize(1);
    display.setCursor(104, 20);
    display.print(F("LPM"));
    display.display();
  
  } else {
      
      display.setTextSize(2);
      display.setCursor(13, 8); 
      display.print(F("MAX LIMIT"));
      display.display();
    
    }
  
}

void ISR_sensor() {
 
  t_pulse_duration_volatile = micros() - t_pulse_started_volatile;
  t_pulse_started_volatile = micros();
  timeout = 0;
  newpulse = 1;
 
}
