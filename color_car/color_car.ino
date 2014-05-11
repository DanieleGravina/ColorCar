////////////////////////////////////////////////////////////////
/// Color Car:                                                //
/// Firmware for the Robotics And Design project ColorCar,    //
/// an arduino controlled DC car with an RGB sensor, that     //
/// follow colored track by steering the front wheel depending//
/// on the color detected.                                    //
////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <Servo.h>
#include <MsTimer2.h>


#define DEBUG //comment out to disable serial debugging
#define RELEASE

//Pin H bridge for DC motor
#define EN_12 7 
#define IN_1 6
#define IN_S 9
#define IN_2 5


//Pin RGB sensor
#define S0     11
#define S1     8
#define S2     4
#define S3     3
#define OUT    2

#define r_led 12 //red led
#define g_led 13 //green led


#define numColors  7

//delay for RGB sensor timer
#define DELAY_COLOR 30
#define DELAY_START 1000

#define R 0
#define G 1
#define B 2

#define YELLOW  0
#define BLUE 1
#define GREEN 2
#define SKY_BLUE 3
#define RED 4
#define WHITE 5

#define SPEED_2 255
#define SPEED_1 220

#define STRAIGHT 87
#define WHEEL_1 20
#define WHEEL_2 35

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

//Store how many times RGB sensor see no color
int noColor = 0;

//flag that says if RGB see the white for the first time
boolean firstTime = true; 

Servo myservo;

String colorNames[] = {"Yellow", "Blue", "Green", "Sky Blue", "Red", "White", "Black" };

// Hue component of the colors recognised by the sensor
float hValues[] = {-21, 139,  230,  143,  -1,  360};
float range[] =   {  4,   3,   10,   3,  3,   4};


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
  
  straight();
 
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
  go(SPEED_2);
}

void loop(){
  
  g_flag = 0;
  
  Count[R] = g_array[R] * g_SF[R];
  Count[B] = g_array[B] * g_SF[B];
  Count[G] = g_array[G] * g_SF[G];
  
    #ifdef DEBUG
  Serial.print(Count[R]);
  Serial.print("|");
  Serial.print(Count[G]);
  Serial.print("|");
  Serial.println(Count[B]);
  
  #endif

  computeH((float)Count[R], (float)Count[G], (float)Count[B]);
  
  delay(DELAY_COLOR*4); 


  
}

//Compute the Hue component of HSV from RGB
void computeH(float red, float blue, float green)
{
  
    float h;
    
    float maxi = MAX3(red, green, blue);
    
    float mini = MIN3(red, green, blue);
    
    float delta = maxi - mini;
    
    if( (red > 245 && green > 245 && blue > 245) || (red < 20 && green < 20 && blue <20))
    
      h = 360;
      
    // compute hue  
    else{
      
      if(firstTime)
          firstTime = false;
      
      if(maxi == green)
      
            h = 60*(((blue - red)/delta) + 2);
            
      else if(maxi == blue)
      
              h = 60*(((red - green)/delta) + 4);
              
              else {
                  
                  h = 60*fmod(((green - blue)/delta), 6);
                
              }
    }
    
    #ifdef DEBUG
    
    Serial.println(h);
    
    #endif
    
    
    hDecide(h);
}

//Decide what color has been detected 
void hDecide(float h){

  int closest_num = -1;
  
  for(int i = 0; i < numColors; ++i){
    
    if(h >= (hValues[i] - range[i]) && h <= (hValues[i] + range[i])){
      closest_num = i;
       i=numColors;
    }
  }
  
  if(closest_num == -1){
    noColor++;
    
    if(noColor > 5){ //if 5 times no color detected -> game over
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

//Map the color detected with the steering angle of the front wheel
void changeState(int colorDecision){
  
 switch(colorDecision){
   
   case RED:
        straight(); 
        go(SPEED_1);
        break;
        
   case SKY_BLUE:
        left(WHEEL_2);
        break;
        
   case BLUE:
        left(WHEEL_1);
        break;
   
   case YELLOW:
        right(WHEEL_2);
        break;
   
   case GREEN:
        right(WHEEL_1);
        break;
        
   case WHITE:
   
        if(!firstTime)
          stopped();
        
        straight();
        break;
        
 }
 
 
          
}


// Straight
void straight(){
  myservo.write(STRAIGHT);
}

//Speed control
void go(int vel){
  analogWrite(IN_1, vel);
  digitalWrite(IN_2,LOW);
}

//Stop the car
void stopped(){
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW); 
 
  digitalWrite(g_led, LOW); //game over
  digitalWrite(r_led, HIGH);
 
  #ifdef RELEASE 
  //reset to restart
  while(1){
  } 
  #endif
}

//Turn right of degree angle
void right(int degree){
  myservo.write(STRAIGHT + degree); 
}


//Turn left of degree angle
void left(int degree){
  myservo.write(STRAIGHT - degree);
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
 
  digitalWrite(S0, LOW);  // OUTPUT FREQUENCY SCALING 2%
  digitalWrite(S1, HIGH);  
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
