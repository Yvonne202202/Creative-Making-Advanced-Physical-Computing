static int Press_Val = 0;
//Pin 5 and 6 controls the electric rod
const int Motor1 = 5;
const int Motor2 = 6;
bool Motor_Flag = true;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Motor1, OUTPUT);
  pinMode(Motor2, OUTPUT);

}

void loop() {
  //Pressure sensor module
  Press_Val = analogRead(A0);
  Serial.print("   Press_Val:");
  Serial.println(Press_Val);
  if (Press_Val > 20) //Electric rod down for 5 seconds
                      //when the pressure is greater than 20
  {
  if (Motor_Flag == true) 
    {
      Down();
      delay(5000);
      
      Stop();
      Motor_Flag = false;//Set flag to false when the electric push rod is lowered
    }
    delay(2000); //The electric rod is held in this position for 2 seconds

  }
  else if (Press_Val <= 20 && Motor_Flag == false) //The electric rod rises for 5 seconds
    Up();                      // if it has been lowered and the pressure is less than 20
  {
    delay(5000);
    Stop();
    Motor_Flag = true;
  }

}
//Set the rise value of the electric rod
void Up()
{
  analogWrite(Motor1, 152); //Set the rising voltage
  analogWrite(Motor2, 0);
}
//Set the down value of the electric rod

void Down()
{
  analogWrite(Motor1, 0);
  analogWrite(Motor2, 152);
}
//Set the stop value
void Stop()
{
  analogWrite(Motor1, 0);
  analogWrite(Motor2, 0);
  //delay(2000);
}
