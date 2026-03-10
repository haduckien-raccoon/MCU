//int s0=3,s1=4,s2=5,s3=6;
//int out=2;
//int flag=0;
//byte counter=0;
//byte countR=0,countG=0,countB=0;
//void setup()
// {
// Serial.begin(9600);
// pinMode(s0,OUTPUT);
// pinMode(s1,OUTPUT); 
// pinMode(s2,OUTPUT);
// pinMode(s3,OUTPUT);
// 
// }
//void TCS()
// {
//  flag=0;  
//  digitalWrite(s1,HIGH);
//  digitalWrite(s0,HIGH);
//  digitalWrite(s2,LOW);
//  digitalWrite(s3,LOW);
//  attachInterrupt(0, ISR_INTO, CHANGE);
//  timer0_init();
// }
//void ISR_INTO()
// {
//  counter++;
// }
//void timer0_init(void)
// {
//    TCCR2A=0x00;
//    TCCR2B=0x07;   //the clock frequency source 1024 points
//    TCNT2= 100;    //10 ms overflow again
//  TIMSK2 = 0x01; //allow interrupt
// }
// int i=0;
// ISR(TIMER2_OVF_vect)//the timer 2, 10ms interrupt overflow again. Internal overflow interrupt executive function
//{
//    TCNT2=5000;
//    flag++;
// if(flag==1)
//  {
//    countR=counter;
//    Serial.print("red=");
//    Serial.println(countR,DEC);
//    digitalWrite(s2,HIGH);
//    digitalWrite(s3,HIGH);
//  }
//  else if(flag==2)
//   {
//    countG=counter;
//    Serial.print("green=");
//    Serial.println(countG,DEC);
//    digitalWrite(s2,LOW);
//    digitalWrite(s3,HIGH);
//   }
//   else if(flag==3)
//    {
//    countB=counter;
//    Serial.print("blue=");
//    Serial.println(countB,DEC);
//    Serial.println("\n"); 
//    digitalWrite(s2,LOW);
//    digitalWrite(s3,LOW);
//    
//    }
//    else if(flag==4)
//     {
//     flag=0;
//     }
//    counter=0;
//}
//void loop()
// {
//  TCS();
//  while(1){ 
//    delay (1000);
//    };
// }


/*     Arduino Color Sensing Tutorial
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
 
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
int frequency = 0;
void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
 
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
  Serial.begin(9600);
}
void loop() {

  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  
  frequency = pulseIn(sensorOut, LOW);
  
  frequency = map(frequency, 25,72,255,0);
  
  Serial.print("R= ");
  Serial.print(frequency);
  Serial.print("  ");
  delay(100);
  
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  
  frequency = pulseIn(sensorOut, LOW);
  
  frequency = map(frequency, 30,90,255,0);
  
  Serial.print("G= ");
  Serial.print(frequency);
  Serial.print("  ");
  delay(100);
 
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  
  frequency = pulseIn(sensorOut, LOW);
  
  frequency = map(frequency, 25,70,255,0);
  
  Serial.print("B= ");
  Serial.print(frequency);
  Serial.println("  ");
  delay(1000);
}
