#include <Wire.h>
#include <Servo.h>
#include <TimerOne.h>

#define DEBUG
#define MINIMIZE

#define EN_12 7 //Posteriore
#define IN_1 10
#define IN_2 11
#define IN_S 9

#define S0     6
#define S1     5
#define S2     4
#define S3     3
#define OUT    2

#define numColors  7

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

#define SPEED_2 2
#define SPEED_1 1

#define DRITTO 90
#define RUOTA_1 30
#define RUOTA_2 60

int   g_count = 0;    // count the frequecy
int   g_array[3];     // store the RGB value
int   g_flag = 0;     // filter of RGB queue
float g_SF[3];        // save the RGB Scale factor

int Count[3]; 



typedef enum state{
  
  start,
  game,
  outRoad
  
} state;

state myState = outRoad;

Servo myservo;

String colorNames[] = {"Yellow", "Purple", "Green", "Blue", "White", "Orange", "Red", "White" };
String stateNames[] = {"start", "game", "end"};

float hValues[] = {27, -33, 140, 221, 14, -4, 360};
float range[] = {2, 2, 3, 2, 2, 2, 2};


void setup(){
  
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  
  
  pinMode(EN_12, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  
  myservo.attach(IN_S);
  digitalWrite(EN_12,HIGH);
  myservo.write(90);
  
  while(1){
    avanti(3);
  }
  
  
  //color sensor
  
  /*TSC_Init();
  Timer1.initialize();             // defaulte is 1s
  Timer1.attachInterrupt(TSC_Callback);  
  attachInterrupt(0, TSC_Count, RISING);  
 
  delay(4000);
 
  for(int i=0; i<3; i++)
    Serial.println(g_array[i]);
 
  g_SF[0] = 255.0/ g_array[0];     //R Scale factor
  g_SF[1] = 255.0/ g_array[1] ;    //G Scale factor
  g_SF[2] = 255.0/ g_array[2] ;    //B Scale factor
 
  Serial.println(g_SF[0]);
  Serial.println(g_SF[1]);
  Serial.println(g_SF[2]);*/
}

void loop(){
  
  g_flag = 0;
  
  Count[R] = g_array[R] * g_SF[R];
  Count[B] = g_array[B] * g_SF[B];
  Count[G] = g_array[G] * g_SF[G];
  
  decideV2((float)Count[R], (float)Count[G], (float)Count[B]);
  
  #ifdef DEBUG
  Serial.println("");
  delay(200);
  #endif
  
}

void decideV2(float red, float blue, float green)
{
  
    float h;
    
    float maxi = max(red, blue);
    maxi = max(maxi, green);
    
    float mini = min(red, blue);
    mini = min(mini, green);
    
    float delta = maxi - mini;
    
    if(delta == 0)
    
      h = 360;
      
    else{
      
      int q = ((green - blue)/delta)/6;
      h= 60*(((green - blue)/delta) - 6*q);
      if(maxi == green)
            h = 60*(((blue - red)/delta) + 2);
      else if(maxi == blue)
              h = 60*(((red - green)/delta) + 4);
    }
    hDecide(h);
}

#ifdef MINIMIZE
void hDecide(float h){
  
  float closest_distance = 360;
  String closest_name = "black";
  int closest_num = 0;
  
  if(h != 360){
  
    for(int i = 0; i< numColors; i++)
    {
       float h_distance = abs(h - hValues[i]);
   
       if (h_distance < closest_distance)
       {
          closest_name = colorNames[i];
          closest_distance = h_distance;
          closest_num = i;
       }
    }
    
  }
  else 
    closest_num = WHITE;
  
  
  changeState(closest_num);
  
  #ifdef DEBUG
  Serial.println(colorNames[closest_num]);
  //Serial.print("|");
  //Serial.println(stateNames[myState]);
  delay(200);
  #endif
}
#endif

#ifdef RANGE
void hDecide(float h){
  
  float closest_distance = 360;
  String closest_name = "black";
  int closest_num = 0;
  
  if(h != 360){
  
    for(int i = 0; i< numColors; i++)
    {
       float h_distance = abs(h - hValues[i]);
   
       if (h_distance < closest_distance)
       {
          closest_name = colorNames[i];
          closest_distance = h_distance;
          closest_num = i;
       }
    }
    
  }
  else 
    closest_num = WHITE;
  
  
  changeState(closest_num);
  
  #ifdef DEBUG
  Serial.println(colorNames[closest_num]);
  //Serial.print("|");
  //Serial.println(stateNames[myState]);
  delay(200);
  #endif
}
#endif


void changeState(int colorDecision){
  
 switch(colorDecision){
   
   case RED:
        dritto(); 
        avanti(SPEED_1);
        break;
        
   case PURPLE:
        dritto(); 
        avanti(SPEED_2);
        break;
        
   case BLUE:
        sinistra(RUOTA_1);
        break;
   
   case YELLOW:
        sinistra(RUOTA_2);
        break;
   
   case GREEN:
        destra(RUOTA_1);
        break;
        
   case ORANGE:
        destra(RUOTA_2);
        break;
        
   case WHITE:
        stopped();
        break;
   
        
 } 
 
          
}


void dritto(){
  myservo.write(DRITTO);
}

void avanti(int gear){
  analogWrite(IN_1, 127*gear);
  digitalWrite(IN_2,LOW);
}

void stopped(){
  digitalWrite(IN_1, LOW);
  digitalWrite(IN_2, LOW);  
}

void destra(int degree){
  myservo.write(DRITTO - degree); 
}
void sinistra(int degree){
  myservo.write(degree); 
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
         //Serial.println("->WB Start");
         TSC_WB(LOW, LOW);              //Filter without Red
         break;
    case 1:
         //Serial.print("->Frequency R=");
         //Serial.println(g_count);
         g_array[0] = g_count;
         TSC_WB(HIGH, HIGH);            //Filter without Green
         break;
    case 2:
         //Serial.print("->Frequency G=");
         //Serial.println(g_count);
         g_array[1] = g_count;
         TSC_WB(LOW, HIGH);             //Filter without Blue
         break;
 
    case 3:
         //Serial.print("->Frequency B=");
         //Serial.println(g_count);
         //Serial.println("->WB End");
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
  Timer1.setPeriod(100000);             // set 1s period
}

///////////////////
//end color sensor
//////////////////
