#include <TimerOne.h>

int minRed = 1025;
int maxRed = 0;

int minGreen = 1025;
int maxGreen = 0;

int minBlue = 1025;
int maxBlue = 0;

boolean cond = false;
char val = 'f';

#define S0     6
#define S1     5
#define S2     4
#define S3     3
#define OUT    2

#define RED  0
#define GREEN 1
#define BLUE 2

int   g_count = 0;    // count the frequecy
int   g_array[3];     // store the RGB value
int   g_flag = 0;     // filter of RGB queue
float g_SF[3];        // save the RGB Scale factor

int Count[3]; 
 
 
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
 
void setup()
{
  TSC_Init();
  
  
  
  Serial.begin(9600);
  Timer1.initialize(100000);             // defaulte is 1s
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
  Serial.println(g_SF[2]);
 
}

void decideV2(float red, float green, float blue)
{
  
    float h;
    
    float maxi = max(red, blue);
    maxi = max(maxi, green);
    
    float mini = min(red, blue);
    mini = min(mini, green);
    
    float delta = maxi - mini;
    
    if(red > 250 && green > 250 && blue > 250)
   
      h = 360;
      
    else{
      
      int q = ((green - blue)/delta)/6;
      h= 60*(((green - blue)/delta) - 6*q);
      if(maxi == green)
            h = 60*(((blue - red)/delta) + 2);
      else if(maxi == blue)
              h = 60*(((red - green)/delta) + 4);
    }
    Serial.print("h:");
    Serial.println(h);
}


void loop(){
  
   g_flag = 0;
    
   Serial.println("");
   delay(200);
  
  val = Serial.read();
  
  if(val == 't'){
    cond = true;
  }
  
   if(val == 'f'){
    cond = false;
  }
  
  Count[RED] = g_array[RED] * g_SF[RED];
  Count[BLUE] = g_array[BLUE] * g_SF[BLUE];
  Count[GREEN] = g_array[GREEN] * g_SF[GREEN];
  
  
  
  if(cond){
    
   if(Count[RED] < minRed)
     minRed = Count[RED];
     
   if(Count[RED] > maxRed)
     maxRed = Count[RED];
    
    if(Count[BLUE] < minBlue)
     minBlue = Count[BLUE];
     
    if(Count[BLUE] > maxBlue)
     maxBlue = Count[BLUE];
        
    if(Count[GREEN] < minGreen)
     minGreen = Count[GREEN];  
     
    if(Count[GREEN] > maxGreen)
     maxGreen = Count[GREEN];  
     
     Serial.print(minRed);
     Serial.print("|");
     Serial.print(maxRed);
     Serial.print("||");
     Serial.print(minGreen);
     Serial.print("|");
     Serial.print(maxGreen);
     Serial.print("||");
     Serial.print(minBlue);
     Serial.print("|");
     Serial.println(maxBlue);
     
     
     
  }
  
  else{
    
     for(int i=0; i<3; i++){
      Serial.print(int(g_array[i] * g_SF[i]));
      Serial.print("|");
      
      
      minRed = 1025;
      maxRed = 0;
      
      minGreen = 1025;
      maxGreen = 0;
      
      minBlue = 1025;
      maxBlue = 0;
   }
   
   decideV2((float)Count[RED], (float)Count[GREEN], (float)Count[BLUE]);
  
  }
  
  delay(2000);
  
  
}
