//Written by Yu Wang
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm_W1 = Adafruit_PWMServoDriver(); //默认地址 0x40
Adafruit_PWMServoDriver pwm_W2 = Adafruit_PWMServoDriver(0x41); 
Adafruit_PWMServoDriver pwm_U1 = Adafruit_PWMServoDriver(0x42); 
Adafruit_PWMServoDriver pwm_U2 = Adafruit_PWMServoDriver(0x43); 
#define SERVOMIN  150    // this is the 'minimum' pulse length count (out of 4096)  //0度
#define SERVOMAX  600    // this is the 'maximum' pulse length count (out of 4096)  //180度
bool Flag=true;

const int TrgPin = 3;     
const int EcoPin = 2;
static int dist;
static int Dist;
static int Average[3] = {0};
static int Average_Dist = 0;
static int Count = 0;

static int Press_Val1 = 0;
static int Press_Val2 = 0;

const int Motor1 = 5;     
const int Motor2 = 6;
int Pos1 = 0;
bool Dist_Flag = true;     //控制舵机0的标志
bool Servo_1 = true;       //控制舵机1的标志
bool Motor_Flag = true;     //控制舵机2/3/4的标志

bool Pos_Flag = true;
int Pos=0;
int Num=0;

void setup() 
{
    Serial.begin(9600);             
    pinMode(TrgPin, OUTPUT);    //设置TrgPin为输出状态
    pinMode(EcoPin, INPUT);     //设置EcoPin为输入状态
    pinMode(Motor1,OUTPUT);
    pinMode(Motor2,OUTPUT);
    Serial.println("16 channel Servo test!");
    pwm_W1.begin(); 
    pwm_W1.setPWMFreq(60);  // Analog servos run at ~60 Hz updates   
    W_Pos_Ctrl(0,0);         
    W_Pos_Ctrl(1,0);      
    W_Pos_Ctrl(2,0);   
    W_Pos_Ctrl(3,0);    
    W_Pos_Ctrl(4,0);    
}

void loop() 
{
    /********获取超声波数值**************/
    Average_Dist = Get_Dist();
    Serial.print("Distance:");
    Serial.print(Average_Dist);
    Serial.print("cm"); 
    /********获取两个压力传感器数值******/
    Press_Val1 = analogRead(A0);
    Press_Val2 = analogRead(A1); 
    Serial.print("   Press_Val1:");
    Serial.print(Press_Val1);
    Serial.print("   Press_Val2:");
    Serial.println(Press_Val2);
    /****************超声波控制底下舵机0******************/
    //超声波控制舵机0朝向人
    if(Average_Dist<40 && Dist_Flag==true)
    {
          for(int Pos=0;Pos<=60;Pos++)
          {
              W_Pos_Ctrl(0,Pos);  
              delay(6);
          }
          Dist_Flag=false;
    }
    //人走开，舵机0复位
    if(Average_Dist>=80 && Dist_Flag==false)
    {
          for(int Pos=60;Pos>=0;Pos--)
          {
              W_Pos_Ctrl(0,Pos);  
              delay(6);
          }
          Dist_Flag=true;
    } 
    /****************第一个压力传感器控制舵机1转动，按住就转动******************/
    /****the first pressure sensor controls servo 1 *****/
    if(Press_Val1>100)
    {
        if(Servo_1==true)
        {
            Pos1++;
            if(Pos1>=60)
            {
                Servo_1=false;
            }
            delay(6);
        }else{
            Pos1--;
            if(Pos1<=0)
            {
                Servo_1=true;
            }
            delay(6);
        }
        W_Pos_Ctrl(1,Pos1); 
    }
    /****************第二个压力传感器控制舵机2、3、4转动，按住就转动，推杆电机也动******************/
    //**************** The second pressure sensor controls servo 2, 3, 4 and the/

    if(Press_Val2>100)
    {
         if(Motor_Flag==true)    //推杆电机Up 2秒
         {
            Up();
            delay(2000);
            Stop();
            Motor_Flag=false;
         }
         //控制摇摆
         //舵机2/3拉，4不动
         if(Num==1)
         {
              if(Pos_Flag==true)
              {
                  Pos++;
                  if(Pos>40)
                  {
                      Pos_Flag=false; 
                  }
              }else{
                  Pos--;
                  if(Pos<=0)
                  {
                      Num=2;
                      Pos_Flag=true;
                  }
              }
              W_Pos_Ctrl(2,Pos);  
              W_Pos_Ctrl(3,Pos); 
              delay(6);
         }
         //舵机2/4拉，3不动
         if(Num==2)
         {
              if(Pos_Flag==true)
              {
                  Pos++;
                  if(Pos>40)
                  {
                      Pos_Flag=false; 
                  }
              }else{
                  Pos--;
                  if(Pos<=0)
                  {
                      Num=3;
                      Pos_Flag=true;
                  }
              }
              W_Pos_Ctrl(2,Pos);  
              W_Pos_Ctrl(4,Pos); 
              delay(6);
         }
         //舵机3/4拉，2不动
         if(Num==3)
         {
              if(Pos_Flag==true)
              {
                  Pos++;
                  if(Pos>40)
                  {
                      Pos_Flag=false; 
                  }
              }else{
                  Pos--;
                  if(Pos<=0)
                  {
                      Num=1;
                      Pos_Flag=true;
                  }
              }
              W_Pos_Ctrl(3,Pos);  
              W_Pos_Ctrl(4,Pos); 
              delay(6);
         }
    }else{
         if(Motor_Flag==false)    //推杆电机Down 2秒
         {
            Down();
            delay(2000);
            Stop();
            Motor_Flag=true;
         }
         Pos_Flag=true;
         Pos=0;
         Num=1;
    }
}

void Up()
{
    analogWrite(Motor1,152);
    analogWrite(Motor2,0); 
}
void Down()
{
    analogWrite(Motor1,0);
    analogWrite(Motor2,152); 
}
void Stop()
{
    analogWrite(Motor1,0);
    analogWrite(Motor2,0); 
}
//舵机
void setServoPulse(uint8_t n, double pulse) 
{
    double pulselength; 
    pulselength = 1000000;   // 1,000,000 us per second
    pulselength /= 60;   // 60 Hz
    Serial.print(pulselength); Serial.println(" us per period"); 
    pulselength /= 4096;  // 12 bits of resolution
    Serial.print(pulselength); Serial.println(" us per bit"); 
    pulse *= 1000;
    pulse /= pulselength;
    Serial.println(pulse);
    //pwm_W.setPWM(n, 0, pulse);
}

void W_Pos_Ctrl(int Num,int Angle)     
{
    int Val = map(Angle,0,180,150,600);
    pwm_W1.setPWM(Num, 0, Angle);
}

int Get_Dist()
{
    static int Val=0;
    digitalWrite(TrgPin, LOW);
    delayMicroseconds(8);
    digitalWrite(TrgPin, HIGH);// 维持10毫秒高电平用来产生一个脉冲
    delayMicroseconds(10);
    digitalWrite(TrgPin, LOW); // 读取脉冲的宽度并换算成距离
    dist = pulseIn(EcoPin, HIGH)/58.00;
    
    if (dist<=200)            //滤波处理
    {
        if (dist<=200)       //滤波处理
        {
            Dist =dist;
        }
    }
    else if (dist>200)        //滤波处理
    {
        if (dist>200)         //滤波处理
        {
            Dist =Dist;
        }
    } 
    
    if(Count<3)
    {
        Average[Count++] = Dist;
    }
    else if(Count>=3)
    {       
        Val = Filter(Average,(sizeof(Average) / sizeof(Average[0])));
        Count=0; 
    }
    return Val; 
}


int Filter(int *Buf,int length)
{
    int filter_sum = 0;
    for(int i = 0; i < length; i++) 
    {
        filter_sum +=Buf[i];
        delay(1);
    }
    return (int)(filter_sum / length);
}
