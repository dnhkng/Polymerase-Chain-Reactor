#include <Wire.h>
#include <PID_v1.h>
#include <SerialCommand.h>

SerialCommand sCmd; 

int SensorA = 0b0011001; // Denaturation
int SensorB = 0b0011010; // Annealing
int SensorC = 0b0011000; // Elongation

int TempReg = 0b00000101;

int HeaterPinA = 5; // Denaturaion
int HeaterPinB = 3; // Annealing
int HeaterPinC = 6; // Elongation

double TargetA = 1.0;
double TargetB = 1.0;
double TargetC = 1.0;

double InputA, InputB, InputC;
double OutputA, OutputB, OutputC;

double P = 100.0;
double I = 0.0;
double D = 0.0;

boolean stream = LOW;

PID HeaterA(&InputA, &OutputA, &TargetA, 100,0,0, DIRECT);
PID HeaterB(&InputB, &OutputB, &TargetB, 100,0,0, DIRECT);
PID HeaterC(&InputC, &OutputC, &TargetC, 100,0,0, DIRECT);

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(38400);
  
  HeaterA.SetMode(AUTOMATIC);
  HeaterB.SetMode(AUTOMATIC);
  HeaterC.SetMode(AUTOMATIC);
  
  HeaterA.SetTunings(P,I,D);
  HeaterB.SetTunings(P,I,D);
  HeaterC.SetTunings(P,I,D);
  
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("SETPID",   setPID);     // Set the PID values for a heating element
  sCmd.addCommand("SETTEMP",  setTEMP);    // Set the target temperature for a heating element
  sCmd.addCommand("GETPID",   getPID);     // Return the PID values for a heating element
  sCmd.addCommand("GETTARGET", getTARGET); // Return the temperature of a heating element  
  sCmd.addCommand("GETTEMP",  getTEMP);    // Return the temperature of a heating element
  sCmd.addCommand("STARTSTREAM",  streamSTART);  // turns on and off continuous temperature reporting
  sCmd.addCommand("STOPSTREAM",  streamSTOP);  // turns on and off continuous temperature reporting
  sCmd.addCommand("RESET",  resetTWI);  // turns on and off continuous temperature reporting
  sCmd.addCommand("OFF",  powerDown);      // turns off all heaters by setting target temp to zero
  sCmd.setDefaultHandler(unrecognized);    // Handler for command that isn't matched  (says "Unrecognized Command?")
  
  // Sets temperature sensors for measurement mode
  Wire.beginTransmission(SensorA); // transmit to I2C
  Wire.write(TempReg);              // sends one byte.   
  Wire.endTransmission();    // stop transmitting
  Wire.beginTransmission(SensorB); // transmit to I2C
  Wire.write(TempReg);              // sends one byte.   
  Wire.endTransmission();    // stop transmitting
  Wire.beginTransmission(SensorC); // transmit to I2C
  Wire.write(TempReg);              // sends one byte.   
  Wire.endTransmission();    // stop transmitting
  
  Serial.println("PCR Device Ready");
}


float temperature(int I2C_address) {
  Wire.requestFrom(I2C_address, 2);    // request 2 bytes from address. Because sensor produces 9 bit
  int ambientUpper = 0;
  int ambientLower = 0;
  
  while(Wire.available()) {   // slave may send less than requested
    ambientUpper = Wire.read(); // receive a byte as integer
    ambientLower = Wire.read(); // receive a byte as integer Can change resolution of by chaning 2nd and 3rd bit of the register pointers.
  }  
  
  int a = ambientUpper << 6; // bitshift 6 to the left to allow for 9bit data from temp sensor
  int b = ambientLower >> 2; //bitshift 7 to the right to allow for 9 bit data from temp sensor.
  int tempRaw = a|b;  // define int rawtemp as the combination of integer a and b. We now have 9 bits of data. 
  float dec = 0.0;
  float power = 0.25;
  int mask; 
  int weight;
  
  for (mask = 1; mask<256; mask <<= 1) {
    if (tempRaw & mask) {
      weight = 1;     
    }
    else {
      weight = 0;     
    }  
    dec = dec + (power * weight);
    power = power * 2; 
 }
  return dec;
}  


void setPID() {
  char *arg;
  float p;
  float i;
  float d;
  boolean malformed = LOW;

  arg = sCmd.next();
  if (arg != NULL) {
    if (atof(arg) > 0 && atof(arg) < 300) {
      p = atof(arg);
    }
    else {
      malformed = HIGH;
    }  
  }    
  else {
    malformed = HIGH;
  } 
  
  arg = sCmd.next();
  if (arg != NULL) {
    if (atof(arg) > 0 && atof(arg) < 300) {
      i = atof(arg);
    }
    else {
      malformed = HIGH;
    }
  }      
  else {
    malformed = HIGH;
  } 
  
  arg = sCmd.next();
  if (arg != NULL) {
    if (atof(arg) > 0 && atof(arg) < 300) {
      d = atof(arg);
    }
    else {
      malformed = HIGH;
    }  
  }  
  else {
    malformed = HIGH;
  }  
  
  arg = sCmd.next();
  if (arg != NULL) {
    malformed = HIGH;
  }  
  
  if (!malformed) {
    HeaterA.SetTunings(p,i,d);
    HeaterB.SetTunings(p,i,d);
    HeaterC.SetTunings(p,i,d);
    Serial.print("PID Values: ");
    Serial.print(p);
    Serial.print(", ");
    Serial.print(i);
    Serial.print(", ");
    Serial.println(d);
  }
  else {
    Serial.println("Unrecognized Command");
  }  
}

void setTEMP() {
  char *arg;
  float tempA;
  float tempB;
  float tempC;
  boolean malformed = LOW;
  

  arg = sCmd.next();
  if (arg != NULL) {
    if (atof(arg) > 0 && atof(arg) < 100) {
      tempA = atof(arg);
    }
    else {
      malformed = HIGH;
    }  
  }    
  else {
    malformed = HIGH;
  } 
  
  arg = sCmd.next();
  if (arg != NULL) {
    if (atof(arg) > 0 && atof(arg) < 100) {
      tempB = atof(arg);
    }
    else {
      malformed = HIGH;
    }  
  }    
  else {
    malformed = HIGH;
  } 
  
  arg = sCmd.next();
  if (arg != NULL) {
    if (atof(arg) > 0 && atof(arg) < 100) {
      tempC = atof(arg);
    }
    else {
      malformed = HIGH;
    }  
  }    
  else {
    malformed = HIGH;
  }   
  
  arg = sCmd.next();
  if (arg != NULL) {
    malformed = HIGH;
  }  
  
  if (!malformed) {
    TargetA = tempA;
    TargetB = tempB;
    TargetC = tempC;
    Serial.print("Temperature Target Values: ");
    Serial.print(TargetA);
    Serial.print(", B:");
    Serial.print(TargetB);
    Serial.print(", C:");
    Serial.println(TargetC);
  }
  else {
    Serial.println("Unrecognized Command");
  }  
}


void getPID() {
  Serial.print("PID Values: P:");
  Serial.print(HeaterA.GetKp());
  Serial.print(", I:");
  Serial.print(HeaterA.GetKi());
  Serial.print(", D:");
  Serial.println(HeaterA.GetKd());
}


void getTARGET() {
  Serial.print("Temperature Target Values: A:");
  Serial.print(TargetA);
  Serial.print(", B:");
  Serial.print(TargetB);
  Serial.print(", C:");
  Serial.println(TargetC);
}

void getTEMP() {
  float tempA = temperature(SensorA);
  float tempB = temperature(SensorB);
  float tempC = temperature(SensorC);
  Serial.print("Temperature Values: A:");  
  Serial.print(tempA);
  Serial.print(", B:");
  Serial.print(tempB);
  Serial.print(", C:");
  Serial.println(tempC);

}  

void streamSTART() {
  stream = HIGH;
  Serial.println("Stream Started");
}

void streamSTOP() {
  stream = LOW;
  Serial.println("Stream Stopped");
}


void powerDown() {
  TargetA = 1.0;
  TargetB = 1.0;
  TargetC = 1.0;
  Serial.print("TEMP Targets: A:");
  Serial.print(TargetA);
  Serial.print(", B:");
  Serial.print(TargetB);
  Serial.print(", C:");
  Serial.println(TargetC);
}  


void resetTWI() {
  // Sets temperature sensors for measurement mode
  Wire.beginTransmission(SensorA); // transmit to I2C
  Wire.write(TempReg);              // sends one byte.   
  Wire.endTransmission();    // stop transmitting
  Wire.beginTransmission(SensorB); // transmit to I2C
  Wire.write(TempReg);              // sends one byte.   
  Wire.endTransmission();    // stop transmitting
  Wire.beginTransmission(SensorC); // transmit to I2C
  Wire.write(TempReg);              // sends one byte.   
  Wire.endTransmission();    // stop transmitting
  Serial.print("TWI Temperature Sensors Reset");
}  
  
void unrecognized(const char *command) {
  Serial.println("Unrecognized Command");
}

void loop() {
  sCmd.readSerial(); 

  InputA = temperature(SensorA);
  HeaterA.Compute();
  analogWrite(HeaterPinA,OutputA);
  
  InputB = temperature(SensorB);
  HeaterB.Compute();
  analogWrite(HeaterPinB,OutputB);  
  
  InputC = temperature(SensorC);
  HeaterC.Compute();
  analogWrite(HeaterPinC,OutputC);  

  if (stream){
    Serial.print("DATA ");
    Serial.print(InputA);
    Serial.print(' ');
    Serial.print(InputB);
    Serial.print(' ');
    Serial.print(InputC);
    Serial.print(' ');
    Serial.print(OutputA);
    Serial.print(' ');
    Serial.print(OutputB);
    Serial.print(' ');
    Serial.println(OutputC); 
  }
  
  delay(500);
}
