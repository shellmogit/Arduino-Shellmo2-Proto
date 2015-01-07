/* Arduino code for  Shellmo2 Prototype */
/* Version = 1.0 */

#include <SoftwareSerial.h>
#include <MsTimer2.h>
#include <EEPROM.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

//Motor A
int PWMA = 5; //Speed control 
int AIN1 = 10; //Direction
int AIN2 = 11; //Direction
//Motor B
int PWMB = 6; //Speed control
int BIN1 = 12; //Direction
int BIN2 = 13; //Direction

int PINA0 = 14;
int PINA1 = 15;
int PINA2 = 16;
int PINA3 = 17;
int PINA6 = 20;
int PINA7 = 21;

//Heart LED
int HRT = 3;

//Bluetooth pins
int bluetoothTx = 2;
int bluetoothRx = 4;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

char c=0;//Serial data
short BLUETOOTH_AVAILABLE = 0;
int DATA_TYPE=0;

int heart = 0,heartrate = 10,heartcount=0,eyecount=0;
short PHASE_HEART = 0;

int spd=99,speednow=0,speedmax,accel;
boolean moveflag = false,eyeflag=false; 
int mode = 10;// 10 = Remote, 20,21 = Line follower, 30 = Obstacle avoiding
int speed_L,speed_R,Low,High;
int obscount = 0; short obsback = false, obsleft = false, obsright = false;

//Sounds
int sC1,sD1,sE1,sF1,sG1,sA1,sB1; 
int sC2,sD2,sE2,sF2,sG2,sA2,sB2; 
int sC3,sD3,sE3,sF3,sG3,sA3,sB3; 
int beatrate,b1,b2,b4,b8,b16;
int se = 0, bgm = 0;
short pushA = 0, pushB = 0, pushC = 0;

//NeoPixel
int ambientValue, proximityValue;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 3, NEO_RGB + NEO_KHZ800);

//Servo
Servo SERVO_1; 
Servo SERVO_2; 

void setup() {    
  pinMode(PWMA, OUTPUT);pinMode(AIN1, OUTPUT);pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);pinMode(BIN1, OUTPUT);pinMode(BIN2, OUTPUT);
  pinMode(PINA0, INPUT);pinMode(PINA1, INPUT);pinMode(PINA2, INPUT);
  pinMode(PINA3, INPUT);pinMode(PINA6, INPUT);pinMode(PINA7, INPUT);
  pinMode(HRT, OUTPUT); pinMode(9, OUTPUT);

  //Setting up sounds and length     
  sC1 = 130; sD1=146; sE1=164; sF1=174; sG1=195; sA1=220; sB1=247;
  sC2 = 262; sD2=293; sE2=329; sF2=349; sG2=391; sA2=440; sB2=493;
  sC3 = 523; sD3=587; sE3=659; sF3=698; sG3=783; sA3=880; sB3=988;  
  beatrate = 80;
  b4=60000/beatrate/2; b1=b4*4;b2=b4*2;b8=b4/2;b16=b4/4; //b4 = 375
  
  strip.begin();
  strip.setPixelColor(0,strip.Color(128,128,64));
  strip.show(); // Initialize all pixels to 'off'
  
speedmax= EEPROM.read(0);if(speedmax==0){speedmax=255;}
accel= EEPROM.read(1); if(accel==0){accel=128;}  

  MsTimer2::set(50, timer_main); 
  MsTimer2::start();
  
  SERVO_1.attach(7); 
  SERVO_1.writeMicroseconds(1500);
  SERVO_2.attach(8); 
  SERVO_2.writeMicroseconds(1500);

  bluetooth.begin(9600);
  bluetooth.println("Start");
  
}

void timer_main(){
if(mode == 20){
 if(digitalRead(PINA2) == 1 && digitalRead(PINA3) == 1){move_forward();}
 else if(digitalRead(PINA2) == 0 && digitalRead(PINA3) == 1){move_right();}
 else if(digitalRead(PINA2) == 1 && digitalRead(PINA3) == 0){move_left();}
 else if(digitalRead(PINA2)  == 0 && digitalRead(PINA3) == 0 ){move_forward();} 
}

else if(mode == 21){
  Low = 0; High = 150;
 if(digitalRead(PINA2) == 1 && digitalRead(PINA3) == 1){ speed_R = High; speed_L = High;  }
 else if(digitalRead(PINA2) == 0 && digitalRead(PINA3) == 1){speed_R = Low; speed_L = High;}
 else if(digitalRead(PINA2) == 1 && digitalRead(PINA3) == 0){speed_R = High; speed_L = Low;}
 else if(digitalRead(PINA2)  == 0 && digitalRead(PINA3) == 0 ){speed_R = High; speed_L = High;} 
analogWrite(PWMA, speed_L);analogWrite(PWMB, speed_R);
move_forward();
}

else if(mode == 30){
  if(obsleft == true)
  {
    if(obscount<5){move_back();}
    else if(obscount<10){move_right();}
    else {obsleft = false; obsright = false; move_forward();}    
    obscount++;
  }
  else if(obsright == true)
  {
    if(obscount<5){move_back();}
    else if(obscount<10){move_left();}
    else{obsleft = false; obsright = false; move_forward();}    
    obscount++;
  }   
  else if(digitalRead(PINA0) == 1){obsright = true; obscount = 0;}
  else if(digitalRead(PINA1) == 1){obsleft = true; obscount = 0;}
  else {move_forward();}
}

if(se==1)
{
  long f = sC3;long l;int d;
  l=1000000/f; d = l/2;
  for(int i=0; i<100; i++ ){digitalWrite(9, HIGH);delayMicroseconds(d);digitalWrite(9, LOW);delayMicroseconds(d);}
   se=0;
 }
 else if(se==2)
{
  long f = sE3;long l;int d;
  l=1000000/f; d = l/2;
  for(int i=0; i<100; i++ ){digitalWrite(9, HIGH);delayMicroseconds(d);digitalWrite(9, LOW);delayMicroseconds(d);}
   se=0;
 }
 else if(se==3)
{
  long f = sG3;long l;int d;
  l=1000000/f; d = l/2;
  for(int i=0; i<100; i++ ){digitalWrite(9, HIGH);delayMicroseconds(d);digitalWrite(9, LOW);delayMicroseconds(d);}
   se=0;
 }
  else if(se==4)
{
  long f = sB3;long l;int d;
  l=1000000/f; d = l/2;
  for(int i=0; i<100; i++ ){digitalWrite(9, HIGH);delayMicroseconds(d);digitalWrite(9, LOW);delayMicroseconds(d);}
   se=0;
 }


}//timer


void loop() {

 BLUETOOTH_AVAILABLE = bluetooth.available();
     if(BLUETOOTH_AVAILABLE==1){
      c = bluetooth.read();
       bluetooth.println(c);
       switch(c){
       case 'w':move_forward();break;
       case 's':move_back();break;
       case 'a':move_left();break;
       case 'd':move_right();break;
       case 'x':move_stop();break;
       case 'z':move_stop();break;
       case 'c':move_brakes();break;
       case 'h':move_stop(); mode = 10; setcolor(128,128,64);bluetooth.println("Remote");se = 1;  break;
       case 'j':if(mode == 20){mode = 21; setcolor(255,16,0);bluetooth.println("LineB"); se = 4;} else{mode = 20; setcolor(0,128,255);bluetooth.println("LineA"); se = 2;} break;
       case 'l':mode = 30; setcolor(128,255,0);se = 3;bluetooth.println("Obj");  break;      
       case 'i': break;
       case 'k': break;
       case 'm': break;       
       case 'v':DATA_TYPE=1; break;
       case 'b':DATA_TYPE=3; break;       
       default:
       if(c>=48&&c<=57){
         //read value of speedmax or accel        
      if(DATA_TYPE==1){
        spd = 10*(c -48);
        DATA_TYPE = 2;
      }
      else if(DATA_TYPE==2){
        spd =spd + c -48;
       speedmax=spd*2+spd/2;
        EEPROM.write(0,speedmax); 
        DATA_TYPE = 0;
        analogWrite(PWMA, speedmax);
        analogWrite(PWMB, speedmax);
      }
      else if(DATA_TYPE==3){
        accel = 10*(c -48);
        DATA_TYPE = 4;
      }
      else if(DATA_TYPE==4){
        accel =accel + c -48;
        SERVO_1.writeMicroseconds(accel*30);//not use accel but controlling Serbo

        EEPROM.write(1,accel);
        DATA_TYPE = 0;        
      }
      }
      else{move_stop();}//other data = stop 
      break;
       
       }//switch data

}//bluetooth available

}//loop

void move_forward(){motor(1,0);motor(2,0);moveflag=true;}
void move_back(){motor(1,1);motor(2,1);moveflag=true;}
void move_left(){motor(1,1);motor(2,0);moveflag=true;}
void move_right(){motor(1,0);motor(2,1);moveflag=true;}
void move_stop(){digitalWrite(AIN1, 0);digitalWrite(AIN2, 0);digitalWrite(BIN1, 0);digitalWrite(BIN2, 0);moveflag=false;speednow=0;}
void move_brakes(){digitalWrite(AIN1, 1);digitalWrite(AIN2, 1);digitalWrite(BIN1, 1);digitalWrite(BIN2, 1);moveflag=false;speednow=0;}

void motor(int motor,  int direction){
//  digitalWrite(STBY, 1); //disable standby
  int inPin1 = 1;
  int inPin2 = 0;
  if(direction == 1){
    inPin1 = 0;
    inPin2 = 1;
  }
  if(motor == 1){
    digitalWrite(AIN1, inPin1);
    digitalWrite(AIN2, inPin2);
    if(mode==10){analogWrite(PWMA, speedmax);}
  }else{
    digitalWrite(BIN1, inPin1);
    digitalWrite(BIN2, inPin2);
    if(mode==10){analogWrite(PWMB, speedmax);}
  }
}//motor

//Sound
void sound(long f, int ms){
  
  long l;int d;
  l=1000000/f;
  d = l/2;
    for(int i=0; i<ms/(1000/f); i++ )
  {
    digitalWrite(9, HIGH);
    delayMicroseconds(d);
    digitalWrite(9, LOW);
    delayMicroseconds(d);   
  }
//delayMicroseconds(100);   
  
}

void setcolor(byte r, byte g, byte b){
 strip.setPixelColor(0,strip.Color(r,g,b));
 strip.show(); 
}


