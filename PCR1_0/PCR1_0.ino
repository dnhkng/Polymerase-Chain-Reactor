/********************************************************
 * Polymerase Chair Reactor v0.2.1
 * This Library is licensed under a GPLv3 License
 * Uses the Arduino Nano
 * Requires the Arduino PID Library by Brett Beauregard
 ********************************************************/

#include <PID_v1.h>
#include <HIDSerial.h>

HIDSerial serial;

//Define Variables we'll be connecting to
double Setpoint1 = 95;
double Setpoint2 = 72;
double Input1, Output1, Input2, Output2;
float ADC1 = 0, ADC2 = 0, temp1 = 0, temp2=0;
unsigned long serialTime;
int led = 0, Probe1 = 1, Probe2 = 0, PWM1 = 6, PWM2 = 5;
boolean state = LOW;

//Specify the links and initial tuning parameters
PID myPID1(&Input1, &Output1, &Setpoint1, 80,0,0, DIRECT);
PID myPID2(&Input2, &Output2, &Setpoint2, 80,0,0, DIRECT);

void setup()
{
  //initialize the variables we're linked to
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH); 

  ADC1 = analogRead(Probe1);
  ADC2 = analogRead(Probe2);  

  serial.begin();
  //turn the PID on
  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
}

void loop()
{
  serial.poll();
  
  
  ADC1 = analogRead(Probe1);
  ADC2 = analogRead(Probe2);
  

  temp1 = ADC1 * -0.3059 + 198.39; //Calculate these contants from a linear fit with a thermometer
  temp2 = ADC2 * -0.3059 + 198.39;
  
  Input1 = temp1;
  Input2 = temp2;
  
  myPID1.Compute();
  myPID2.Compute();
  
  analogWrite(PWM1,Output1);
  analogWrite(PWM2,Output2);
  
  if(millis()>serialTime)
  {
    //SerialReceive();
    SerialSend();
    //serial.println("Hello World!");
    //serial.println(raw);
    serialTime+=1000;
    
    if(state) {
      state = LOW;
      digitalWrite(led, LOW);
    }  
    else {
      state = HIGH;
      digitalWrite(led, HIGH);
    }
  }
 
}

void SerialSend()
{
  serial.print(ADC1);
  serial.print(", ");
  serial.print(ADC2);
  serial.print(", ");
  serial.print(temp1);
  serial.print(", ");  
  serial.print(temp2);
  serial.print(", ");
  serial.print(Output1);
  serial.print(", ");
  serial.println(Output2); 
  
  
  
  /*Print the temperature in Celsius to the serial port
  Serial.print("Celsius: ");
  Serial.println(temp_in_celsius);                  

  Print the temperature in Fahrenheit to the serial port
  Serial.print("Ouput: ");
  Serial.println(Output);
  Serial.println();  
  
  //Print the temperature in Fahrenheit to the serial port
  Serial.print("Raw: ");
  Serial.println(raw);
  Serial.println(millivolts);  
  Serial.println();   */
}
