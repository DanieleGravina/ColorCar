/* 
 * //////////////////////////////////////////////////
 * //making sense of the Parallax PIR sensor's output
 * //////////////////////////////////////////////////
 *
 * Switches a LED according to the state of the sensors output pin.
 * Determines the beginning and end of continuous motion sequences.
 *
 * @author: Kristian Gohlke / krigoo (_) gmail (_) com / http://krx.at
 * @date:   3. September 2006 
 *
 * kr1 (cleft) 2006 
 * released under a creative commons "Attribution-NonCommercial-ShareAlike 2.0" license
 * http://creativecommons.org/licenses/by-nc-sa/2.0/de/
 *
 *
 * The Parallax PIR Sensor is an easy to use digital infrared motion sensor module. 
 * (http://www.parallax.com/detail.asp?product_id=555-28027)
 *
 * The sensor's output pin goes to HIGH if motion is present.
 * However, even if motion is present it goes to LOW from time to time, 
 * which might give the impression no motion is present. 
 * This program deals with this issue by ignoring LOW-phases shorter than a given time, 
 * assuming continuous motion is present during these phases.
 *  
 */
 
#include <LiquidCrystal.h>


//Definição dos periodos para cada nota (*0.0000001)
#define tone_C 1911
#define tone_C1 1804
#define tone_D 1703
#define tone_Eb 1607
#define tone_E 1517
#define tone_F 1432
#define tone_F1 1352
#define tone_G 1276
#define tone_Ab 1204
#define tone_A 1136
#define tone_Bb 1073
#define tone_B 1012
#define tone_c 955
#define tone_c1 902
#define tone_d 851
#define tone_eb 803
#define tone_e 758
#define tone_f 716
#define tone_f1 676
#define tone_g 638
#define tone_ab 602
#define tone_a 568
#define tone_bb 536
#define tone_b 506
 
#define p       0  //pausa

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 7;    //the digital pin connected to the PIR sensor's output
int ledPin = 13;

int speaker = 6;    //porta do arduino
long vel = 20000;

int mario_m[] = {tone_e, tone_e, tone_e, tone_c, tone_e, tone_g, tone_G, tone_c, tone_G, tone_E, tone_A, tone_B, tone_Bb, tone_A, tone_G, tone_e, tone_g, tone_a, tone_f, tone_g, tone_e, tone_c, tone_d, tone_B, tone_c};
int mario_r[] = {6, 12, 12, 6, 12, 24, 24, 18, 18, 18, 12, 12, 6, 12, 8, 8, 8, 12, 6, 12, 12, 6, 6, 6, 12};
 

/////////////////////////////
//SETUP
void setup(){
  
  Serial.begin(9600);
  
  pinMode(speaker, OUTPUT);
  
  pinMode(ledPin, OUTPUT);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  
  
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
  }

//
//////////////////////////
//LOOP
void loop(){
  
      // set the cursor to column 0, line 1
      // (note: line 1 is the second row, since counting begins with 0):
      //lcd.setCursor(0, 1);

     if(digitalRead(pirPin) == HIGH){
       digitalWrite(ledPin, HIGH);
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         lcd.print("Victory!");
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         playMelody();
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       digitalWrite(ledPin, LOW);
       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;   
           lcd.clear();
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");           
           delay(50);
           }
       }
  }
  
  
 void playMelody(){
    const int LEN = (sizeof(mario_r) + 1) / 2;
    Serial.println(LEN);
    for (int i=0; i < LEN; i++)
    {
        int tom = mario_m[i];
        int tempo = mario_r[i];
 
        long tvalue = tempo * vel;
 
        tocar(tom, tvalue);
 
        delayMicroseconds(1000); //pause between notes
    }
  }
  
  void tocar(int tom, long tempo_value)
{
     long tempo_gasto = 0;
     while (tempo_gasto < tempo_value && tempo_value < 640000) // enters an infinite loop when tempo_value is a big value
     {  
        digitalWrite(speaker, HIGH);
        delayMicroseconds(tom / 2);
 
        digitalWrite(speaker, LOW);
        delayMicroseconds(tom/2);
 
        tempo_gasto += tom;
    }
}

