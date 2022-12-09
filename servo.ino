#include <Servo.h>
#define posMin 70
#define posMax 100
int fsrPin = 0;  // the FSR and 10K pulldown are connected to a0
int fsrReading;  // the analog reading from the FSR resistor divider

//pin 3, 4, 5 control the three servos to make the branches move
int s1pin = 3;
Servo serv1;
int s2pin = 4;
Servo serv2;
int s3pin = 5;
Servo serv3;

//Pin 8 controls servo4 to turn the flower
int deg=0;
int degMax=180;
int degMin=0;
int s4pin = 8;
Servo serv4;

//Pin 10 controls servo5 to turn the branches of the flower
int s5pin = 10;
Servo serv5;


//Acoustic sensor module
const int TrgPin = 13;
const int EcoPin = 12;
int pos = posMax;
static int dist;
static int Dist;
static int Average[3] = { 0 };
static int Average_Dist = 0;
static int Count = 0;
int Val = 0;

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);
  serv1.attach(s1pin);
  serv2.attach(s2pin);
  serv3.attach(s3pin);
  serv4.attach(s4pin);
  serv5.attach(s5pin);
  pinMode(TrgPin, OUTPUT);  //Set TrgPin to output state
  pinMode(EcoPin, INPUT);   //Set EcoPin to the input state
}

void loop(void) {
  digitalWrite(TrgPin, LOW);
  delayMicroseconds(8);
  digitalWrite(TrgPin, HIGH);  // Maintain a high level for 10 
  delayMicroseconds(10);      //milliseconds to generate a pulse
  digitalWrite(TrgPin, LOW);  // Read the width of the pulse and convert it to distance
  dist = pulseIn(EcoPin, HIGH) / 58.00;

  if (dist <= 200)  //Filter processing
  {
    
    if (dist <= 200)  
    {
      Dist = dist;
    }
  } else if (dist > 200)  
  {
    if (dist > 200)  
    {
      Dist = Dist;
    }
  }

  if (Count < 3) {
    Average[Count++] = Dist;
  } else if (Count >= 3) {
    Val = Filter(Average, (sizeof(Average) / sizeof(Average[0])));
    Count = 0;
  }
  // dist = Get_Dist();
  Serial.print("Distance:");
  
  Serial.print(Val);
  Serial.print("cm");

  if (Val < 15) {
    if (pos > posMin ) {
      for (pos = posMax; pos > posMin; pos--)  // Servo rotates from 0 to 180 degrees. 
      {                               
        serv5.write(pos);             // Specify the steering Angle of the steering gear
        delay(27);                    // Wait 27ms for the steering gear to reach the
      }                               // designated position
    } else if (pos == posMin) {
      pos = posMin;
    }
  } else if(Val>100) {
    if (pos < posMax) {
      for (pos = posMin; pos < posMax; pos++)  // Servo rotates from 0 degrees to 180 degrees
      {                               // One degree per rotation
        serv5.write(pos);             // Specify the steering Angle of the steering gear
        delay(27);                    //Wait 27ms for the steering gear to reach the
      }                               // designated position
      
    } else if (pos == posMax) {
      pos = posMax;
    }
  } 


  //Pressure sensor module
  fsrReading = analogRead(fsrPin);
  Serial.print("  press = ");
  Serial.println(fsrReading);
  // Pressure sensors control the servo
  if (fsrReading > 80) {
    
    // Control the flower rotation
    for(deg=degMin; deg<=degMax;deg++){
      serv4.write(deg);
      delay(5000/(degMax-degMin));
    }
    //Control the flower towards the person
     serv1.write(180);
     serv2.write(180);
     delay(1000);
     
     serv1.write(0);
     serv2.write(0);
     delay(1000);
    serv3.write(180);
    delay(1000);
    serv3.write(0);





  } else {
    //Control the flower away from the person
    serv1.write(0);
    serv2.write(0);
    serv3.write(0);

    //servo which control flower reset
    if(deg==degMax+1) {
      for(deg=degMax;deg>=degMin;deg--){
        serv4.write(deg);
        delay(5000/(degMax-degMin));
      }
    }
  }
}

int Filter(int *Buf, int length) 
{
  int filter_sum = 0;
  for (int i = 0; i < length; i++) 
  {
    filter_sum += Buf[i];
    delay(1);
  }
  return (int)(filter_sum / length);
}
