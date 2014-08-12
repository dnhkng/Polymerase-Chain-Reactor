/********************************************************
 * Polymerase Chair Reactor v0.2.1
 * This Library is licensed under a GPLv3 License
 * Uses the Arduino Nano
 * Requires the Arduino PID Library by Brett Beauregard
 ********************************************************/

#include <PID_v1.h>

double SetpointA, SetpointB, InputA, InputB, OutputA, OutputB;
float rawA = 0,temp_in_celsiusA = 0,rawB = 0,temp_in_celsiusB = 0, temp_in_kelvin=0, millivolts = 0;
unsigned long serialTime;

//Specify the links and initial tuning parameters, P is enough for now.
PID myPIDa(&InputA, &OutputA, &SetpointA,150,0,0, DIRECT);
PID myPIDb(&InputB, &OutputB, &SetpointB,150,0,0, DIRECT);

void setup()
{
  //initialize the variables we're linked to
  InputA = analogRead(7);
  InputB = analogRead(6);

  SetpointA = 95; // Denaturing Temperature
  SetpointB = 72; // Extension Temperature
  
  Serial.begin(9600); 
  //turn the PID on
  myPIDa.SetMode(AUTOMATIC);
  myPIDb.SetMode(AUTOMATIC);  
}

void loop()
{
  rawA = analogRead(7);
  temp_in_celsiusA = rawA*0.6841  - 395.29;
  InputA = temp_in_celsiusA;
  
  rawB = analogRead(6);
  temp_in_celsiusB = rawB*0.6841  - 395.29;
  InputB = temp_in_celsiusB;  

  myPIDa.Compute();
  analogWrite(6,OutputA);
  
  myPIDb.Compute();
  analogWrite(3,OutputB);

  if(millis()>serialTime)
  {
    //SerialReceive(); used later to set tepmerature from the PC
    SerialSend();
    serialTime+=500;
  }
 
}

// Reports the temperature, and raw ADC data needed to calibrate the sensors.
void SerialSend() 
{
  Serial.print(serialTime);  
  Serial.print(",");  
  Serial.print("PID-A, ");
  Serial.print(SetpointA);  
  Serial.print(",");
  Serial.print(rawA);
  Serial.print(",");
  Serial.print(temp_in_celsiusA);
  Serial.print(",");
  Serial.print(OutputA);
  Serial.print(",");
  Serial.print("   PID-B, ");
  Serial.print(SetpointB);  
  Serial.print(",");
  Serial.print(rawB);
  Serial.print(",");
  Serial.print(temp_in_celsiusB);
  Serial.print(",");  
  Serial.println(OutputB); 
}
