#include <Wire.h>
#include <Servo.h>
#include <MsTimer2.h>


#define DEBUG


#define EN_12 7 //Posteriore
#define IN_1 6
#define IN_S 9
#define IN_2 5

#define S0     11
#define S1     8
#define S2     4
#define S3     3
#define OUT    2

#define r_led 12 //red led
#define g_led 13 //green led


#define numColors  7

#define DELAY_COLOR 30
#define DELAY_START 1000

#define R 0
#define G 1
#define B 2

#define YELLOW  0
#define PURPLE 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4
#define RED 5
#define WHITE 6

#define SPEED_2 255
#define SPEED_1 220
#define GEAR 180

#define DRITTO 93
#define RUOTA_1 20
#define RUOTA_2 35

#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))

int   g_count = 0;    // count the frequecy
int   g_array[3];     // store the RGB value
int   g_flag = 0;     // filter of RGB queue
float g_SF[3];        // save the RGB Scale factor

int Count[3];

int noColor = 0;

boolean firstTime = true; 

Servo myservo;

String colorNames[] = {"Yellow", "Purple", "Green", "Blue", "Orange", "Red", "White" };

//float hValues[] = {27, -33, 140, 221, 14, -4, 360};
//float hValues[] = {-27, 25, 215, 139, -14, 4, 360};
float hValues[] = {-27, 25, 230, 142, -7, -1, 360};
float range[] = {4, 4, 4, 4, 2, 2, 4};


void setup(){
  
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  
  /////////////
  /////led/////
  /////////////
  
  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  
  /////////////////
  /////DC motor////
  /////////////////
  
  pinMode(EN_12, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  
  myservo.attach(IN_S);
  digitalWrite(EN_12,HIGH);
  
  dritto();
 
  /////////////////
  //color sensor//
  ///////////////
  
  delay(DELAY_START);
  
  TSC_Init();
  MsTimer2::set(DELAY_COLOR, TSC_Callback); // DELAY_COLOR ms period
  MsTimer2::start();

  attachInterrupt(0, TSC_Count, RISING);  
 
  delay(DELAY_COLOR*4);
 
  for(int i=0; i<3; i++)
    Serial.println(g_array[i]);
 
  g_SF[0] = 255.0/ g_array[0];     //R Scale factor
  g_SF[1] = 255.0/ g_array[1] ;    //G Scale factor
  g_SF[2] = 255.0/ g_array[2] ;    //B Scale factor
 
  Serial.println(g_SF[0]);
  Serial.println(g_SF[1]);
  Serial.println(g_SF[2]);
  
  digitalWrite(g_led, HIGH); //calibration's over
  avanti(SPEED_2);
}

void loop(){
  
  g_flag = 0;
  
  Count[R] = g_array[R] * g_SF[R];
  Count[B] = g_array[B] * g_SF[B];
  Count[G] = g_array[G] * g_SF[G];

  decideV2((float)Count[R], (float)Count[G], (float)Count[B]);
  
  delay(DELAY_COLOR*4); 

  #ifdef DEBUG
  /*Serial.print(Count[R]);
  Serial.print("|");
  Serial.print(Count[G]);
  Serial.print("|");
  Serial.println(Count[B]);*/
  
  #endif
  
}

void decideV2(float red, float blue, float green)
{
  
    float h;
    
    float maxi = MAX3(red, green, blue);
    
    float mini = MIN3(red, green, blue);
    
    float delta = maxi - mini;
    
    if(red > 245 && green > 245 && blue > 245)
    
      h = 360;
      
    // compute hue  
    else{
      
      if(maxi == green)
      
            h = 60*(((blue - red)/delta) + 2);
            
      else if(maxi == blue)
      
              h = 60*(((red - green)/delta) + 4);
              
              else {
                  
                  h = 60*fmod(((green - blue)/delta), 6);
                
              }
    }
    
              /*red /= maxi;
              green /= maxi;
              blue /= maxi;
              maxi = MAX3(red, green, blue);
              mini = MIN3(red, green, blue);
              
              red = (red - mini)/(maxi - mini);
              green = (green - mini)/(maxi - mini);
              blue = (blue - mini)/(maxi - mini);
              maxi = MAX3(red, green, blue);
              mini = MIN3(red, green, blue);
              
              if (maxi == red) {
                    h = 0.0 + 60.0*(green - blue); 
              } else if (maxi == green) {
                      h = 120.0 + 60.0*(blue - red);
                     } else {
                        h = 240.0 + 60.0*(red - green);
              }
    
          }*/
    
    
    
    #ifdef DEBUG
    
    Serial.println(h);
    
    #endif
    
    
    hDecide(h);
}


void hDecide(float h){

  int closest_num = -1;
  
  for(int i = 0; i < numColors; ++i){
    
    if(h >= (hValues[i] - range[i]) && h <= (hValues[i] + range[i])){
      closest_num = i;
 
    }
  }
  
  if(closest_num == -1){
    noColor++;
    
    if(noColor > 5){
      stopped();
    }

  }
  else{
    noColor = 0;
    changeState(closest_num);
  }
  
  
  #ifdef DEBUG
  Serial.println(colorNames[closest_num]);
  #endif
}


void changeState(int colorDecision){
  
 switch(colorDecision){
   
   case RED:
        firstTime = false;
        dritto(); 
        avanti(SPEED_1);
        break;
        
   case PURPLE:
        firstTime = false;
        sinistra(RUOTA_2);
        break;
        
   case BLUE:
        firstTime = false;
        sinistra(RUOTA_1);
        break;
   
   case YELLOW:
        firstTime = false;
        sinistra(RUOTA_2);
        break;
   
   case GREEN:
        firstTime = false;
        destra(RUOTA_1);
        break;
        
   case ORANGE:
        firstTime = false;
        destra(RUOTA_2);
        break;
        
   case WHITE:
   
        if(!firstTime)
          stopped();
          
        dritto();
        break;
   
        
 }
 
 
          
}


void dritto(){
  myservo.write(DRITTO);
}

void avanti(int gear){
  analogWrite(IN_1, gear);
  digitalWrite(IN_2,LOW);
}

void stopped(){
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW); 
 
  digitalWrite(g_led, LOW); //game over
  digitalWrite(r_led, HIGH);
 
  #ifdef RELEASE 
  while(1){
  } 
  #endif
}

void destra(int degree){
  myservo.write(DRITTO + degree); 
}
void sinistra(int degree){
  myservo.write(DRITTO - degree);
}

///////////////////
//color sensor
//////////////////

// Init TSC230 and setting Frequency.
void TSC_Init()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
 
  /*digitalWrite(S0, LOW);  // OUTPUT FREQUENCY SCALING 2%
  digitalWrite(S1, HIGH); */
  
  digitalWrite(S0, HIGH);  // OUTPUT FREQUENCY SCALING 20%
  digitalWrite(S1, LOW); 
}
 
// Select the filter color 
void TSC_FilterColor(int Level01, int Level02)
{
  if(Level01 != 0)
    Level01 = HIGH;
 
  if(Level02 != 0)
    Level02 = HIGH;
 
  digitalWrite(S2, Level01); 
  digitalWrite(S3, Level02); 
}
 
void TSC_Count()
{
  g_count ++ ;
}
 
void TSC_Callback()
{
  
  switch(g_flag)
  {
    case 0: 
         TSC_WB(LOW, LOW);              //Filter without Red
         break;
    case 1:
         g_array[0] = g_count;
         TSC_WB(HIGH, HIGH);            //Filter without Green
         break;
    case 2:
         g_array[1] = g_count;
         TSC_WB(LOW, HIGH);             //Filter without Blue
         break;
 
    case 3:
         g_array[2] = g_count;
         TSC_WB(HIGH, LOW);             //Clear(no filter)   
         break;
   default:
         g_count = 0;
         break;
  }
}
 
void TSC_WB(int Level0, int Level1)      //White Balance
{
  g_count = 0;
  g_flag ++;
  TSC_FilterColor(Level0, Level1);
}

///////////////////
//end color sensor
//////////////////
