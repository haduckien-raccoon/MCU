//put output pins in an array and count number of pins
const int outputPins[]={ 2,3,4,5,6,7,8,9,10,11 }, numPins=(sizeof(outputPins) / sizeof(outputPins[0]));

//set delay to 1 second, values for on and off, number of desired LEDs to light at a time to 3
int ON=1, OFF=0, analogValue;

//function for turning on LED
void turnOnLED(int currentPin)
{
  for(int i=0;i<numPins;i++)
  {
    if(i<=currentPin)
    {
      digitalWrite(outputPins[i],ON);
    }
    else if(i>currentPin)
    {
      digitalWrite(outputPins[i],OFF);
    }
    
  }
}

void turnOffAllLEDS()
{
  for(int i=0;i<numPins;i++)
  {
    digitalWrite(outputPins[i],OFF);
  }
}

void setup()
{
  
  //set declared pins in outPins array as Output
  for(int i = 0; i < numPins ;i++)
  {
	pinMode(outputPins[i],OUTPUT);
  }
  
  pinMode(A0,INPUT);
}

void loop()
{
  analogValue=analogRead(A0);
  
  if(analogValue>=0 && analogValue<93)
  {
    turnOffAllLEDS();
  }
  else if(analogValue>=93 && analogValue<186)
  {
    turnOnLED(0);
  }
  else if(analogValue>=186 && analogValue<279)
  {
    turnOnLED(1);
  }
  else if(analogValue>=279 && analogValue<372)
  {
    turnOnLED(2);
  }
  else if(analogValue>=372 && analogValue<465)
  {
    turnOnLED(3);
  }
  else if(analogValue>=465 && analogValue<558)
  {
    turnOnLED(4);
  }
  else if(analogValue>=558 && analogValue<651)
  {
    turnOnLED(5);
  }
  else if(analogValue>=651 && analogValue<744)
  {
    turnOnLED(6);
  }
  else if(analogValue>=744 && analogValue<837)
  {
    turnOnLED(7);
  }
  else if(analogValue>=837 && analogValue<930)
  {
    turnOnLED(8);
  }
  else if(analogValue>=930 && analogValue<1024)
  {
    turnOnLED(9);
  }
}